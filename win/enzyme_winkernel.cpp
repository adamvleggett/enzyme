///
/// @file    enzyme_winkernel.cpp
/// @brief   Enzyme Hardware Abstraction Layer: Kernel Service
///
/// @author  Adam Leggett
///

// ---------------------------------------------------------------------------


#include "enzyme_win.h"
#include "../win_kernel/enzyme_winkernel.h"

#include <iostream>
#include <tchar.h>


// ---------------------------------------------------------------------------


namespace enzyme
{
    namespace kernel
    {
        Service* gService;

        static const TCHAR* dvname64 = TEXT("\\\\.\\enzyme64");
        static const TCHAR* bsname64 = TEXT("enzyme64");
        static const TCHAR* koname64 = TEXT("enzyme64.sys");
#ifndef _WIN64
        static const TCHAR* dvname32 = TEXT("\\\\.\\enzyme32");
        static const TCHAR* bsname32 = TEXT("enzyme32");
        static const TCHAR* koname32 = TEXT("enzyme32.sys");

        typedef UINT (WINAPI* GETSYSTEMWOW64DIRECTORY)(LPTSTR, UINT);

        ///
        /// @brief Return true if we are running on Win64
        ///

        static bool iswow64()
        {
            HMODULE dll = GetModuleHandle(TEXT("kernel32.dll"));
            if(dll == NULL)
                return false;

            GETSYSTEMWOW64DIRECTORY _GetSystemWow64Directory = (GETSYSTEMWOW64DIRECTORY)GetProcAddress(dll, "GetSystemWow64DirectoryW");
            if(_GetSystemWow64Directory == NULL)
                return false;

            TCHAR* buf = new TCHAR[32768];
            bool result = !((_GetSystemWow64Directory(buf, 32767) == 0) && (GetLastError() == ERROR_CALL_NOT_IMPLEMENTED));
            delete [] buf;
            return result;
        }
#endif

        static const TCHAR* dvname = dvname64;
        static const TCHAR* bsname = bsname64;
        static const TCHAR* koname = koname64;
    };
};


// ---------------------------------------------------------------------------


///
/// @brief Install the kernel service
///

enzyme::kernel::Service::Service()
{
    SC_HANDLE scm = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if(scm == NULL)
    {
        if(GetLastError() == ERROR_ACCESS_DENIED)
            throw std::runtime_error("Access Denied: Please run this program as Administrator");
        else
            throw std::runtime_error("Kernel Driver: " + lasterror());
    }

    TCHAR path[MAX_PATH];
    TCHAR* ptr;
    if(GetModuleFileName(NULL, ptr = path, MAX_PATH - 12))
        ptr = _tcsrchr(ptr, '\\') + 1;
    _tcscpy(ptr, koname);

    mSCHandle = OpenService(scm, bsname, SERVICE_ALL_ACCESS);
    if(mSCHandle != NULL)
        ChangeServiceConfig(mSCHandle, SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL, path, NULL, NULL, NULL, NULL, NULL, NULL);
    else
        mSCHandle = CreateService(scm, bsname, bsname, SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL, path, NULL, NULL, NULL, NULL, NULL);

    if(mSCHandle == NULL)
    {
        CloseServiceHandle(scm);
        throw std::runtime_error("Kernel Driver: " + lasterror());
    }

    BOOL r = start(scm, path);
    if(!r)
    {
        DWORD error = GetLastError();
        if(error == ERROR_PATH_NOT_FOUND)   // Possibly due to running via network/removable drive
        {
/*            if(GetWindowsDirectory(ptr = path, MAX_PATH - 22))
            {
                ptr += _tcslen(ptr);
                _tcscpy(ptr, _T("\\System32\\"));
                ptr += _tcslen(ptr);
            }
            _tcscpy(ptr, koname);

            static bool copied = false;
            if(!copied)
            {
                env::filesystem::copy_file(koname, path);
                copied = true;
            }
            r = start(scm, path);*/
        }
    }
    CloseServiceHandle(scm);
    if(!r)
    {
        if(GetLastError() != ERROR_SERVICE_ALREADY_RUNNING)
        {
            std::string error(lasterror());
            stop();
            throw std::runtime_error("Kernel Driver: " + error);
        }
    }
}


///
/// @brief Uninstall the kernel service
///

enzyme::kernel::Service::~Service()
{
    stop();
}


///
/// @brief Install the kernel service
///

BOOL enzyme::kernel::Service::start(SC_HANDLE scm, const TCHAR* path)
{
    mSCHandle = OpenService(scm, bsname, SERVICE_ALL_ACCESS);
    if(mSCHandle != NULL)
        ChangeServiceConfig(mSCHandle, SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL, path, NULL, NULL, NULL, NULL, NULL, NULL);
    else
        mSCHandle = CreateService(scm, bsname, bsname, SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL, path, NULL, NULL, NULL, NULL, NULL);

    if(mSCHandle == NULL)
    {
        CloseServiceHandle(scm);
        throw std::runtime_error("Kernel Driver: " + lasterror());
    }

    return StartService(mSCHandle, 1, &bsname);
}


///
/// @brief Uninstall the kernel service
///

BOOL enzyme::kernel::Service::stop()
{
    if(mHandle != INVALID_HANDLE_VALUE)
    {
        CloseHandle(mHandle);
    }

    if(mSCHandle != INVALID_HANDLE_VALUE)
    {
        SERVICE_STATUS_PROCESS status;
        if(ControlService(mSCHandle, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS)&status))
        {
            DWORD timeout = GetTickCount() + 5000;
            while(status.dwCurrentState != SERVICE_STOPPED)
            {
                DWORD tmp;
                if(GetTickCount() >= timeout)
                {
                    std::clog << "Timeout stopping kernel driver" << std::endl;
                    break;
                }
                Sleep(50);
                if(!QueryServiceStatusEx(mSCHandle, SC_STATUS_PROCESS_INFO, (LPBYTE)&status, sizeof(SERVICE_STATUS_PROCESS), &tmp))
                {
                    break;
                }
            }
        }
        else if(GetLastError() != ERROR_SERVICE_NOT_ACTIVE)
        {
            std::clog << lasterror() << std::endl;
        }

        DeleteService(mSCHandle);
        return CloseServiceHandle(mSCHandle);
    }

    return TRUE;
}


///
/// @brief Handle termination of user process
///

void enzyme::kernel::Service::exit()
{
    if(gService)
    {
        HANDLE handle = gService->handle();
        if(handle != INVALID_HANDLE_VALUE)
        {
            DWORD rlen;
            DeviceIoControl(handle, IOCTL_ENZYME_TERMINATE, NULL, 0, NULL, 0, &rlen, NULL);
        }
        delete gService;
    }
}


// ---------------------------------------------------------------------------


std::string enzyme::kernel::lasterror()
{
    CHAR buf[512];
    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&buf, sizeof(buf), NULL);
    return buf;
}
