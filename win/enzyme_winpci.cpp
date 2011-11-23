///
/// @file    enzyme_winpci.cpp
/// @brief   Enzyme Hardware Abstraction Layer: PCI Device
///
/// @author  Adam Leggett
///

// ---------------------------------------------------------------------------


#include "enzyme_winpci.h"

#include <iostream>
#include <tchar.h>
#include <RegStr.h>


// ---------------------------------------------------------------------------


namespace enzyme
{
    namespace pci
    {
        namespace os
        {

            ///
            /// @brief Encapsulate SetupDiGetDeviceRegistryPropertyA with dynamic allocation
            ///

            inline BOOL getregprop(HDEVINFO di, SP_DEVINFO_DATA& didata, DWORD prop, PTSTR& buf, DWORD& len)
            {
                DWORD tmp;
                BOOL r = SetupDiGetDeviceRegistryProperty(di, &didata, prop, &tmp, reinterpret_cast<PBYTE>(buf), len, &len);
                if(!r && (GetLastError() == ERROR_INSUFFICIENT_BUFFER))
                {
                    delete [] buf;
                    len = len * 2;
                    buf = new TCHAR[len];
                    r = SetupDiGetDeviceRegistryProperty(di, &didata, prop, &tmp, reinterpret_cast<PBYTE>(buf), len, &len);
                }
                return r;
            }
        };
    };
};


// ---------------------------------------------------------------------------


///
/// @brief Enumerate all PCI devices
///

enzyme::pci::os::Enumerator::Enumerator()
{
    mDI = SetupDiGetClassDevs(NULL, REGSTR_KEY_PCIENUM, 0, DIGCF_PRESENT | DIGCF_ALLCLASSES);
    if(mDI == INVALID_HANDLE_VALUE)
        throw std::runtime_error("Cannot access setupapi. Please run as administrator.");

    SP_DEVINFO_DATA didata = { sizeof(didata) };

    DWORD ind = 0;
    DWORD len = 1024;
    PTSTR buf = new TCHAR[len];

    enzyme::Node* rootnode = this;
    enzyme::Device* rootdev = NULL;

    while(SetupDiEnumDeviceInfo(mDI, ind++, &didata))
    {
        // Read PCI configuration (vendor/device) information
        USHORT cfg_vendor;
        USHORT cfg_device;
        USHORT cfg_subvendor = 0xFFFF;
        USHORT cfg_subdevice = 0xFFFF;
        USHORT cfg_class;

        if(!getregprop(mDI, didata, SPDRP_HARDWAREID, buf, len))
            continue;

        PTSTR field = buf;
        while(*field)
        {
            if((_stscanf_s(field, TEXT("PCI\\VEN_%x&DEV_%x&CC_%x"), &cfg_vendor, &cfg_device, &cfg_class) == 3) && (cfg_class & 0xFF0000))
                break;
            field += _tcslen(field) + 1;
        }

        // Read PCI bus location (bn/dn/fn) information
        USHORT loc_bn;
        USHORT loc_dn;
        USHORT loc_fn;

        if(!getregprop(mDI, didata, SPDRP_LOCATION_INFORMATION, buf, len))
            continue;
        if((_stscanf_s(buf, TEXT("PCI bus %u, device %u, function %u"), &loc_bn, &loc_dn, &loc_fn) != 3))
            continue;

        if(!getregprop(mDI, didata, SPDRP_SERVICE, buf, len))
            *buf = '\0';

        device().push_back(
            Device(*this,
                    Location(0, loc_bn, loc_dn, loc_fn),
                    Config(cfg_vendor, cfg_device, cfg_subvendor, cfg_subdevice, cfg_class),
                    buf,
                    mDI,
                    didata));

        if((loc_bn == 0) && (loc_dn == 0) && (loc_fn == 0))
            rootnode = rootdev = &device().back();
    }

    delete [] buf;

    device().sort();
}


///
/// @brief Release enumerator resources
///

enzyme::pci::os::Enumerator::~Enumerator()
{
    SetupDiDestroyDeviceInfoList(mDI);
}


// ---------------------------------------------------------------------------


///
/// @brief Enumerate PCI device resources
///

enzyme::pci::os::Device::Device(Enumerator& enumerator, const Location& loc, const Config& cfg, const PTSTR service, HDEVINFO di, SP_DEVINFO_DATA& didata)
    : pci::Device(enumerator)
    , mEnumerator(enumerator)
    , mDI(didata)
{
    mLocation   = loc;
    mConfig     = cfg;

#ifdef UNICODE
    size_t len = wcslen(service);
    char* cservice = new char[len];
    wcstombs_s(&len, cservice, len, service, len);
    mService = cservice;
    delete [] cservice;
#else
    mService = service;
#endif

    SetupDiGetDeviceInstallParams(di, &didata, &mDIP);

    LOG_CONF logconf;
    if(CM_Get_First_Log_Conf(&logconf, didata.DevInst, BOOT_LOG_CONF) == CR_SUCCESS)
    {
        RES_DES rd = logconf;
        while(CM_Get_Next_Res_Des(&rd, rd, ResType_Mem, NULL, 0) == CR_SUCCESS)
        {
            MEM_RESOURCE mr;
            if(CM_Get_Res_Des_Data(rd, &mr, sizeof(mr), 0) == CR_SUCCESS)
            {
                mMemResourceOS.push_back(mr);
                mMemResource.insert(&mMemResourceOS.back());
            }
        }
        rd = logconf;
        while(CM_Get_Next_Res_Des(&rd, rd, ResType_IO, NULL, 0) == CR_SUCCESS)
        {
            IO_RESOURCE ior;
            if(CM_Get_Res_Des_Data(rd, &ior, sizeof(ior), 0) == CR_SUCCESS)
            {
                mPortResourceOS.push_back(ior);
                mPortResource.insert(&mPortResourceOS.back());
            }
        }
    }
}


enzyme::pci::os::Device::~Device()
{
}
