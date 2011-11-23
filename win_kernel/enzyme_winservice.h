///
/// @file    enzyme_winservice.h
/// @brief   Enzyme Hardware Abstraction Layer: Kernel Service
///
/// @author  Adam Leggett
///

// ---------------------------------------------------------------------------


#pragma once
#ifndef _enzyme_winservice_h_
#define _enzyme_winservice_h_


#ifndef CTL_CODE
#include <windows.h>
#endif


#define FILE_DEVICE_ENZYME          0x0000E0E0
#define FUNC_ENZYME                 0x800

#define ENZYME_CACHE_UC             0
#define ENZYME_CACHE_WB             1
#define ENZYME_CACHE_WC             2


// ---------------------------------------------------------------------------

#define IOCTL_ENZYME_READPORT       CTL_CODE(FILE_DEVICE_ENZYME, FUNC_ENZYME+0, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct
{
    DWORD   ulAddr;
    DWORD   ulSize;
}
enzyme_ReadPort_in;

#define IOCTL_ENZYME_WRITEPORT      CTL_CODE(FILE_DEVICE_ENZYME, FUNC_ENZYME+1, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct
{
    DWORD   ulAddr;
    DWORD   ulSize;
    DWORD   ulVal;
}
enzyme_WritePort_in;


//-----------------------------------------------------------------------------


#define IOCTL_ENZYME_READPCI        CTL_CODE(FILE_DEVICE_ENZYME, FUNC_ENZYME+2, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_ENZYME_WRITEPCI       CTL_CODE(FILE_DEVICE_ENZYME, FUNC_ENZYME+3, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct
{
    union {
        struct {
            DWORD mDomain   : 16;
            DWORD mBus      : 8;
            DWORD mDevice   : 5;
            DWORD mFunction : 3;
        };
        DWORD mAll;
    };
    DWORD   mOffset : 16;
    DWORD   mLength : 16;
}
enzyme_ReadWritePCI_in;


//-----------------------------------------------------------------------------


#define IOCTL_ENZYME_KMALLOC        CTL_CODE(FILE_DEVICE_ENZYME, FUNC_ENZYME+4, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct
{
    ULONG64     ulMinAddr;
    ULONG64     ulMaxAddr;
    ULONG64     ulBound;
    ULONG64     ulSize;
    ULONG64     ulCache;
}
enzyme_KMalloc_in;

typedef struct
{
    ULONG64     ulPhysicalAddr;
    ULONG64     ulKernelAddr;
    ULONG64     pvUserAddr;
    ULONG64     pvMDL;
}
enzyme_KMalloc_out;

#define IOCTL_ENZYME_KFREE          CTL_CODE(FILE_DEVICE_ENZYME, FUNC_ENZYME+5, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct
{
    ULONG64     ulKernelAddr;
    ULONG64     ulAddr;
    ULONG64     ulSize;
    ULONG64     ulCache;
    ULONG64     ulMdl;
}
enzyme_KFree;


//-----------------------------------------------------------------------------
/*
//WARNING:All calls to LOCK_PAGES must have a corresponding call to UNLOCK_PAGES, or a kernel panic will occur on process exit

#define IOCTL_ENZYME_LOCK_PAGES     CTL_CODE(FILE_DEVICE_ENZYME, FUNC_ENZYME+6, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct
{
    ULONG64     ulBase;         // Must be page aligned
    ULONG64     ulPageCnt;      // Must be equal to length of buffer divided by page size
    ULONG64     ulPageSize;     // Must be equal to kernel page size
}
enzyme_PageAlloc;

#define IOCTL_ENZYME_UNLOCK_PAGES   CTL_CODE(FILE_DEVICE_ENZYME, FUNC_ENZYME+7, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct
{
    ULONG64     pvMDL;
}
enzyme_PageFree;
*/

//-----------------------------------------------------------------------------


#define IOCTL_ENZYME_MAP            CTL_CODE(FILE_DEVICE_ENZYME, FUNC_ENZYME+8, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct
{
    DWORD64 vaddr;
    DWORD64 kaddr;
    DWORD64 mdl;
}
enzyme_MapResource_data;

typedef struct
{
    DWORD64 base;
    DWORD64 size;
    DWORD64 cache;
}
enzyme_MapResource_in;

typedef struct
{
    enzyme_MapResource_data data;
}
enzyme_MapResource_out;


#define IOCTL_ENZYME_UNMAP          CTL_CODE(FILE_DEVICE_ENZYME, FUNC_ENZYME+9, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct
{
    enzyme_MapResource_data data;
    DWORD64 size;
}
enzyme_UnmapResource_in;



//-----------------------------------------------------------------------------


#define IOCTL_ENZYME_TERMINATE      CTL_CODE(FILE_DEVICE_ENZYME, FUNC_ENZYME+666, METHOD_BUFFERED, FILE_ANY_ACCESS)


#endif  // _enzyme_winservice_h_
