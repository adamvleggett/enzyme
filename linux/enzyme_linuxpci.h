///
/// @file    enzyme_linuxpci.h
/// @brief   Enzyme Hardware Abstraction Layer: Linux Platform
///
/// @author  Adam Leggett
///

// ---------------------------------------------------------------------------


#pragma once
#ifndef _enzyme_linuxpci_h_
#define _enzyme_linuxpci_h_


#include <cstring>
#include <vector>
#include <stdexcept>

#include "enzyme_linuxmem.h"
#include "enzyme_linuxport.h"

#include "../enzyme_pci.h"


namespace enzyme
{
    namespace pci
    {
        namespace os
        {

            ///
            /// @brief Linux (Sysfs/Procfs) PCI device enumerator
            ///

            class Enumerator : public pci::Enumerator
            {
            protected:

            public:
                Enumerator();
                ~Enumerator();
            };


            ///
            /// @brief Linux PCI device
            ///

            class Device : public pci::Device
            {
            protected:
                Enumerator& mEnumerator;

                // TODO:make const
                std::vector<mem::os::Resource> mMemResourceOS;
                std::vector<port::os::Resource> mPortResourceOS;

            public:
                Device(Enumerator& enumerator, const Location& location);
                Device(Enumerator& enumerator, const std::string& buf);
                ~Device();

                Enumerator& enumerator() const { return mEnumerator; }
            };


            ///
            /// @brief Linux accessor to PCI device
            ///

            class Client
            {
            private:
                const Device& mDevice;

            public:

                ///
                /// @brief If exclusive, disable the existing kernel service for this device
                ///

                Client(const Device& device, bool writable, bool exclusive)
                    : mDevice(device)
                {
                    if(exclusive)
                    {
                    }
                }


                ///
                /// @brief If exclusive, reenable the existing kernel service for this device
                ///

                ~Client()
                {
                }


                ///
                /// @brief Send ioctl to read PCI configuration space for this device
                ///

                void cfgr(size_t offset, size_t len, void* dst)
                {
                    {
                        throw std::runtime_error("PCI configuration space: " + std::string("Failed to read entire range"));
                    }
                }


                ///
                /// @brief Send ioctl to write PCI configuration space for this device
                ///

                void cfgw(size_t offset, size_t len, const void* src)
                {
                    throw std::runtime_error("PCI configuration space: Unimplemented");
                }
            };
        };
    };
};


#endif  // _enzyme_linuxpci_h_
