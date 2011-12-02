///
/// @file    enzyme_pci.cpp
/// @brief   Enzyme Hardware Abstraction Layer: PCI Device
///
/// @author  Adam Leggett
///

// ---------------------------------------------------------------------------


#include "enzyme_pci.h"
#include "enzyme_pcivendor.h"
#include "enzyme_platform.h"


// ---------------------------------------------------------------------------


///
/// @brief Vendor AutoLex constructor
///

enzyme::pci::Vendor::Vendor(unsigned short vendor)
    : mVendor(vendor)
{
}


///
/// @brief Convert vendor ID to string
///

std::ostream& enzyme::pci::Vendor::lex(std::ostream& os) const
{
    if(mVendorID.empty())
    {
        VendorID* cur = gVendor;
        VendorID* end = gVendor + (sizeof(gVendor) / sizeof(gVendor[0]));

        while(cur < end)
        {
            mVendorID.insert(std::make_pair(cur->id, cur->name));
            cur++;
        }
    }

    return os;
}


// ---------------------------------------------------------------------------


///
/// @brief Search for devices by vendor/device ID
///

std::list<enzyme::pci::Device*> enzyme::pci::Enumerator::device(const Config& config)
{
    std::list<enzyme::pci::Device*> result;
    std::list<enzyme::pci::Device>::iterator i;
    for(i = device().begin(); i != device().end(); i++)
    {
        //TODO
        result.push_back(&*i);
    }
    return result;
}


///
/// @brief Find child device by bus location
///

enzyme::pci::Device* enzyme::pci::Enumerator::device(const Location& location)
{
    std::list<enzyme::pci::Device>::iterator i;
    for(i = device().begin(); i != device().end(); i++)
    {
        if(location == i->location())
            return &*i;
    }

    throw std::runtime_error("Device not found");
}


// ---------------------------------------------------------------------------


///
/// @brief Detect and map MMR and PMR BARs
///

enzyme::pci::Client::Client(const Device& device, bool writable, bool exclusive)
    : mDevice(device)
{
    mImpl = new os::Client(reinterpret_cast<const os::Device&>(device), writable, exclusive);

    // Detect MMR BAR: Not prefetchable and under 1MB
    std::set<const mem::Resource*>::const_iterator mi;
    for(mi = device.mem().begin(); mi != device.mem().end(); mi++)
    {
        if(!((*mi)->flag() & 0x8) && ((*mi)->size() > 0) && ((*mi)->size() < 1 * 1024 * 1024))
            break;
    }

    if(mi != device.mem().end())
        mMMReg32 = new mem::Client<uint32_t>(**mi);
    else
        mMMReg32 = new mem::Client<uint32_t>(mem::dummy::gResource);

        mMMReg16 = new mem::Client<uint16_t>(*mMMReg32);
        mMMReg8 = new mem::Client<uint8_t>(*mMMReg32);

    // Detect PMR BAR: First I/O BAR of at least 1KB
    std::set<const port::Resource*>::const_iterator pi;
    for(pi = device.port().begin(); pi != device.port().end(); pi++)
    {
        if((*pi)->size() >= 1024)
            break;
    }
}


///
/// @brief Unmap MMR and PMR BARs
///

enzyme::pci::Client::~Client()
{
    delete mMMReg32;
    delete mMMReg16;
    delete mMMReg8;

    delete mImpl;
}


///
/// @brief Read a range from PCI configuration space
///

void enzyme::pci::Client::cfgr(size_t offset, size_t len, void* dst)
{
    if(len > 4096)
        throw std::logic_error("PCI configuration space: Length out of range");

    mImpl->cfgr(offset, len, dst);
}


///
/// @brief Write a range to PCI configuration space
///

void enzyme::pci::Client::cfgw(size_t offset, size_t len, const void* src)
{
    if(len > 4096)
        throw std::logic_error("PCI configuration space: Length out of range");

    mImpl->cfgw(offset, len, src);
}
