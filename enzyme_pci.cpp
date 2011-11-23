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


///
/// @brief Populate vendor ID table
///

void enzyme::pci::Enumerator::vendorinit()
{
    Vendor* cur = gVendor;
    Vendor* end = gVendor + (sizeof(gVendor) / sizeof(gVendor[0]));

    while(cur < end)
    {
        mVendor.insert(std::make_pair(cur->id, cur->name));
        cur++;
    }
}


// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------


///
/// @brief Read a range from PCI configuration space
///

void enzyme::pci::Client::cfgr(size_t offset, size_t len, void* dst)
{
    if(len > 4096)
        throw std::logic_error("PCI configuration space: Length out of range");
}


///
/// @brief Write a range to PCI configuration space
///

void enzyme::pci::Client::cfgw(size_t offset, size_t len, const void* src)
{
    if(len > 4096)
        throw std::logic_error("PCI configuration space: Length out of range");
}
