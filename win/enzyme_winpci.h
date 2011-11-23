///
/// @file    enzyme_winpci.h
/// @brief   Enzyme Hardware Abstraction Layer: Windows Platform
///
/// @author  Adam Leggett
///

// ---------------------------------------------------------------------------


#pragma once
#ifndef _enzyme_winpci_h_
#define _enzyme_winpci_h_


#include <cstring>
#include <vector>
#include <stdexcept>
#include <windows.h>
#include <setupapi.h>

#include "enzyme_winkernel.h"
#include "enzyme_winmem.h"
#include "enzyme_winport.h"
#include "../enzyme_pci.h"


namespace enzyme
{
    namespace pci
    {
        namespace os
        {

            ///
            /// @brief Windows PCI device enumerator
            ///

            class Enumerator : public pci::Enumerator
            {
            protected:
                HDEVINFO mDI;

            public:
                Enumerator();
                ~Enumerator();

                HDEVINFO devinfo() const { return mDI; }
            };


            ///
            /// @brief Windows PCI device
            ///

            class Device : public pci::Device
            {
            protected:
                Enumerator& mEnumerator;
                SP_DEVINFO_DATA mDI;
                SP_DEVINSTALL_PARAMS mDIP;

                std::vector<mem::os::Resource> mMemResourceOS;
                std::vector<port::os::Resource> mPortResourceOS;

            public:
                Device(Enumerator& enumerator, const Location& loc, const Config& cfg, const PTSTR service, HDEVINFO di, SP_DEVINFO_DATA& didata);
                ~Device();

                Enumerator& enumerator() const { return mEnumerator; }
                SP_DEVINFO_DATA* devinfodata() { return &mDI; }
            };


            ///
            /// @brief Windows accessor to PCI device
            ///

            class Client : public kernel::Client
            {
            private:
                pci::Client& mOwner;
                Device& mDevice;
                bool mServiceDisabled;

            public:

                ///
                /// @brief If exclusive, disable the existing kernel service for this device
                ///

                Client(pci::Client& owner, Device& device, bool writable, bool exclusive)
                    : mOwner(owner)
                    , mDevice(device)
                {
                    if(exclusive)
                    {
                        SP_PROPCHANGE_PARAMS pcp;
                        pcp.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
                        pcp.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
                        pcp.StateChange = DICS_DISABLE;
                        pcp.Scope       = DICS_FLAG_CONFIGSPECIFIC;
                        pcp.HwProfile   = 0;

                        if(!SetupDiSetClassInstallParams(device.enumerator().devinfo(), device.devinfodata(), &pcp.ClassInstallHeader, sizeof(pcp)))
                        {
                            throw std::runtime_error("Error stopping service: " + kernel::lasterror());
                        }

                        if(!SetupDiCallClassInstaller(DIF_PROPERTYCHANGE, device.enumerator().devinfo(), device.devinfodata()))
                        {
                            throw std::runtime_error("Error stopping service: " + kernel::lasterror());
                        }

                        SP_DEVINSTALL_PARAMS dip = { sizeof(dip) };
                        if(SetupDiGetDeviceInstallParams(device.enumerator().devinfo(), device.devinfodata(), &dip) && (dip.Flags & (DI_NEEDRESTART | DI_NEEDREBOOT)))
                        {
                            throw std::runtime_error("Error stopping service: Need reboot for exclusive access");
                        }

                        mServiceDisabled = true;
                    }
                }


                ///
                /// @brief If exclusive, reenable the existing kernel service for this device
                ///

                ~Client()
                {
                    if(mServiceDisabled)
                    {
                        SP_PROPCHANGE_PARAMS pcp;
                        pcp.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
                        pcp.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
                        pcp.StateChange = DICS_ENABLE;
                        pcp.Scope       = DICS_FLAG_GLOBAL;
                        pcp.HwProfile   = 0;

                        if(SetupDiSetClassInstallParams(mDevice.enumerator().devinfo(), mDevice.devinfodata(), &pcp.ClassInstallHeader, sizeof(pcp)))
                        {
                            SetupDiCallClassInstaller(DIF_PROPERTYCHANGE, mDevice.enumerator().devinfo(), mDevice.devinfodata());
                        }

                        SP_DEVINSTALL_PARAMS dip = { sizeof(dip) };
                        if(SetupDiGetDeviceInstallParams(mDevice.enumerator().devinfo(), mDevice.devinfodata(), &dip) && (dip.Flags & (DI_NEEDRESTART | DI_NEEDREBOOT)))
                        {
//                            std::clog << "Error restarting service: Need reboot for exclusive access" << std::endl;
                        }
                    }
                }


                ///
                /// @brief Send ioctl to read PCI configuration space for this device
                ///

                void cfgr(size_t offset, size_t len, void* dst)
                {
                    enzyme_ReadWritePCI_in in;
                    in.mAll     = mDevice.location().to_i();
                    in.mOffset  = offset;
                    in.mLength  = len;

                    DWORD rlen;
                    if(!DeviceIoControl(handle(), IOCTL_ENZYME_READPCI, &in, sizeof(in), dst, len, &rlen, NULL))
                    {
                        throw std::runtime_error("PCI configuration space: " + kernel::lasterror());
                    }
                    if(rlen != len)
                    {
                        throw std::runtime_error("PCI configuration space: " + std::string("Failed to read entire range"));
                    }
                }


                ///
                /// @brief Send ioctl to write PCI configuration space for this device
                ///

                void cfgw(size_t offset, size_t len, const void* src)
                {
                    union {
                        uint8_t* data;
                        enzyme_ReadWritePCI_in* head;
                    }
                    in = {
                        new uint8_t[sizeof(in.head) + len]
                    };

                    in.head->mAll       = mDevice.location().to_i();
                    in.head->mOffset    = offset;
                    in.head->mLength    = len;
                    memcpy(in.data + sizeof(in.head), src, len);

                    DWORD rlen;
                    BOOL r = DeviceIoControl(handle(), IOCTL_ENZYME_WRITEPCI, &in, sizeof(in), NULL, 0, &rlen, NULL);
                    delete [] in.data;
                    if(!r)
                        throw std::runtime_error("PCI configuration space: " + kernel::lasterror());
                }
            };
        };
    };
};


#endif  // _enzyme_winpci_h_
