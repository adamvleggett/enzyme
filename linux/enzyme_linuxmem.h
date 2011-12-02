///
/// @file    enzyme_linuxmem.h
/// @brief   Enzyme Hardware Abstraction Layer: Linux Platform / Physical Memory Resource
///
/// @author  Adam Leggett
///

// ---------------------------------------------------------------------------


#pragma once
#ifndef _enzyme_linuxmem_h_
#define _enzyme_linuxmem_h_


#include "../enzyme_mem.h"


namespace enzyme
{
    namespace mem
    {
        namespace os
        {

            ///
            /// @brief Memory resource mapped from physical to user virtual
            ///

            class Client : public impl::Client
            {
            private:
                uintmax_t mSize;

            public:
                Client(uintmax_t rbase, uintmax_t rsize, Cache cache, uintmax_t offset, uintmax_t size)
                {
                    {
                        throw std::runtime_error("Failed to map memory resource");
                    }
                }

                ~Client()
                {
                }

                volatile void* vaddr() const
                {
                    return reinterpret_cast<void*>(0);
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
                unsigned int mIndex;

            public:
                Resource(unsigned int index)
                    : mem::Resource(static_cast<uintmax_t>(0),
                                    static_cast<uintmax_t>(0),
                                    static_cast<uintmax_t>(0x00))
                    , mIndex(index)
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


#endif  // _enzyme_linuxmem_h_
