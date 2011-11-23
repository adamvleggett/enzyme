///
/// @file    enzyme_winport.h
/// @brief   Enzyme Hardware Abstraction Layer: Windows Platform / I/O Port Resource
///
/// @author  Adam Leggett
///

// ---------------------------------------------------------------------------


#pragma once
#ifndef _enzyme_winport_h_
#define _enzyme_winport_h_


#include <windows.h>
#if defined(__MINGW32__) && (__MSVCRT_VERSION__ < 0x0700)
#include <ddk/cfgmgr32.h>
#else
#include <cfgmgr32.h>
#endif

#include "enzyme_winkernel.h"
#include "../enzyme_port.h"


namespace enzyme
{
    namespace port
    {
        namespace os
        {
            class Resource : public port::Resource
            {
            protected:
                IO_RESOURCE mIOR;

            public:
                Resource(const IO_RESOURCE& ior)
                    : port::Resource(static_cast<uint16_t>(ior.IO_Header.IOD_Alloc_Base),
                                     static_cast<uint16_t>(ior.IO_Header.IOD_Alloc_End - ior.IO_Header.IOD_Alloc_Base + 1))
                    , mIOR(ior)
                {
                }
            };

            class Client : public kernel::Client
            {
            public:
            };
        };
    };
};


#endif  // _enzyme_winport_h_
