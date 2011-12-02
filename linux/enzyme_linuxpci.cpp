///
/// @file    enzyme_linuxpci.cpp
/// @brief   Enzyme Hardware Abstraction Layer: PCI Device
///
/// @author  Adam Leggett
///

// ---------------------------------------------------------------------------


#include "enzyme_linuxkernel.h"
#include "enzyme_linuxpci.h"

#include <dirent.h>
#include <fstream>


// ---------------------------------------------------------------------------


namespace enzyme
{
    namespace pci
    {
        namespace os
        {
        };
    };
};


// ---------------------------------------------------------------------------


///
/// @brief Enumerate all PCI devices
///

enzyme::pci::os::Enumerator::Enumerator()
{
    std::vector<enzyme::Device*> root(8);

    if(kernel::have_sysfs())
    {
        DIR* devices = opendir("/sys/bus/pci/devices");
        if(devices)
        {
            struct dirent entry;
            struct dirent* entp;
            while(!readdir_r(devices, &entry, &entp) && entp)
            {
                unsigned int loc_dom;
                unsigned int loc_bn;
                unsigned int loc_dn;
                unsigned int loc_fn;
                if(sscanf(entry.d_name, "%x:%x:%x.%x", &loc_dom, &loc_bn, &loc_dn, &loc_fn) == 4)
                {
                    device().push_back(Device(*this, Location(loc_dom, loc_bn, loc_dn, loc_fn)));
                }
            }
        }
    }
    else {
        std::ifstream devices("/proc/bus/pci/devices");
        std::string buf;
        while(std::getline(devices, buf))
        {
            device().push_back(Device(*this, buf));
        }
    }


/*
        if((loc_bn == 0) && (loc_dn == 0) && (loc_fn == 0))
            rootnode = rootdev = &device().back();*/

    device().sort();
}


///
/// @brief Release enumerator resources
///

enzyme::pci::os::Enumerator::~Enumerator()
{
}


// ---------------------------------------------------------------------------


///
/// @brief Enumerate PCI device resources
///

enzyme::pci::os::Device::Device(Enumerator& enumerator, const Location& location)
    : pci::Device(enumerator, location, Config())
    , mEnumerator(enumerator)
{
    // mService = service;

    // TODO

    std::vector<mem::os::Resource>::iterator mi;
    for(mi = mMemResourceOS.begin(); mi != mMemResourceOS.end(); mi++)
        mMemResource.insert(&*mi);

    std::vector<port::os::Resource>::iterator pi;
    for(pi = mPortResourceOS.begin(); pi != mPortResourceOS.end(); pi++)
        mPortResource.insert(&*pi);
}


enzyme::pci::os::Device::~Device()
{
}
