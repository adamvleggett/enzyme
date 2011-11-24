///
/// @file    enzyme_winmem.h
/// @brief   Enzyme Hardware Abstraction Layer: Windows Platform / Physical Memory Resource
///
/// @author  Adam Leggett
///

// ---------------------------------------------------------------------------


#pragma once
#ifndef _enzyme_winmem_h_
#define _enzyme_winmem_h_


#include <windows.h>
#if defined(__MINGW32__) && (__MSVCRT_VERSION__ < 0x0700)
#include <ddk/cfgmgr32.h>
#else
#include <cfgmgr32.h>
#endif

#include "enzyme_winkernel.h"

#include "../enzyme_mem.h"
#include "../win_kernel/enzyme_winservice.h"


namespace enzyme
{
    namespace mem
    {
        namespace os
        {

            ///
            /// @brief Memory resource mapped from physical to user virtual
            ///

            class Client : public impl::Client, public kernel::Client
            {
            private:
                enzyme_MapResource_data mData;
                uintmax_t mSize;

            public:
                Client(uintmax_t rbase, uintmax_t rsize, Cache cache, uintmax_t offset, uintmax_t size)
                {
                    enzyme_MapResource_in in;
                    in.base = rbase;
                    in.size = rsize;

                    switch(cache)
                    {
                    case WB:
                        in.cache = ENZYME_CACHE_WB; break;
                    case WC:
                        in.cache = ENZYME_CACHE_WC; break;
                    default:
                        in.cache = ENZYME_CACHE_UC; break;
                    }

                    DWORD rlen;
                    if(!DeviceIoControl(handle(), IOCTL_ENZYME_MAP, &in, sizeof(in), &mData, sizeof(mData), &rlen, NULL) || (rlen != sizeof(mData)))
                    {
                        throw std::runtime_error("Failed to map memory resource");
                    }
                }

                ~Client()
                {
                    enzyme_UnmapResource_in in;
                    in.data = mData;
                    in.size = mSize;

                    DWORD rlen;
                    DeviceIoControl(handle(), IOCTL_ENZYME_UNMAP, &in, sizeof(in), NULL, 0, &rlen, NULL);
                }

                volatile void* vaddr() const
                {
                    return reinterpret_cast<void*>(mData.vaddr);
                }

                impl::Client* client(Cache cache, uintmax_t offset, uintmax_t size) const
                {
                    return const_cast<Client*>(this); // FIXME!!
                }
            };


            ///
            /// @brief Memory resource
            ///

            class Resource : public mem::Resource
            {
            protected:
                MEM_RESOURCE mMR;

            public:
                Resource(const MEM_RESOURCE& mr)
                    : mem::Resource(static_cast<uintmax_t>(mr.MEM_Header.MD_Alloc_Base),
                                    static_cast<uintmax_t>(mr.MEM_Header.MD_Alloc_End - mr.MEM_Header.MD_Alloc_Base + 1),
                                    static_cast<uintmax_t>((mr.MEM_Header.MD_Flags & fMD_Prefetchable) ? 0x08 : 0x00))
                    , mMR(mr)
                {
                }

                impl::Client* client(Cache cache, uintmax_t offset, uintmax_t size_) const
                {
                    return new Client(base(), size(), cache, offset, size_);
                }
            };
        };
    };
};


#endif  // _enzyme_winmem_h_
