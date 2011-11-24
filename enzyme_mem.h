///
/// @file    enzyme_mem.h
/// @brief   Enzyme Hardware Abstraction Layer: Physical Memory Resource
///
/// @author  Adam Leggett
///

// ---------------------------------------------------------------------------


#pragma once
#ifndef _enzyme_mem_h_
#define _enzyme_mem_h_


#include "enzyme.h"


namespace enzyme
{
    namespace mem
    {

        ///
        /// @brief Memory cache type
        ///

        typedef enum
        {
            UC, WC, WB, Default
        }
        Cache;


        ///
        /// @brief Abstract memory resource accessor
        ///

        namespace impl
        {
            class Client
            {
            public:
                virtual ~Client() { };
                virtual Client* client(Cache cache, uintmax_t offset, uintmax_t size) const = 0;
                virtual volatile void* vaddr() const = 0;
            };
        };


        ///
        /// @brief Physically contiguous memory resource
        ///

        class Resource
        {
        protected:
            uintmax_t mBase;
            uintmax_t mSize;
            uintmax_t mFlag;

        public:
            Resource(uintmax_t base, uintmax_t size, uintmax_t flag)
                : mBase(base)
                , mSize(size)
                , mFlag(flag)
            {
            }

            Resource(const Resource& other)
                : mBase(other.mBase)
                , mSize(other.mSize)
                , mFlag(other.mFlag)
            {
            }

            virtual ~Resource()
            {
            }

            uintmax_t base() const { return mBase; }
            uintmax_t size() const { return mSize; }
            uintmax_t flag() const { return mFlag; }

            bool compare(uintmax_t base, uintmax_t size) const
            {
                return (mBase > base) || (mBase + mSize < base + size);
            }

            bool operator<(const Resource& other) const
            {
                return (mBase < other.mBase);
            }

            virtual impl::Client* client(Cache cache, uintmax_t offset, uintmax_t size) const = 0;
        };


        ///
        /// @brief Physically contiguous memory resource accessor
        ///
        /// The virtual memory pointer is not public because it's possible for future
        /// platforms (e.g. Windows 8 or hardware emulators) to not support mapping
        /// physical memory to user virtual addresses. Instead, read() and write() are
        /// provided.
        ///

        template<typename T> class Client : public Resource
        {
        private:
            impl::Client* mImpl;
            volatile T* mVirt;

            static std::runtime_error mReadError;
            static std::runtime_error mWriteError;

        public:
            Client(const Resource& resource, Cache cache = UC, uintmax_t offset = 0, uintmax_t size = ~(uintmax_t)0)
                : Resource(resource)
            {
                mImpl = client(cache, offset, size);
                mVirt = reinterpret_cast<volatile T*>(mImpl->vaddr());
            }

            ~Client()
            {
                delete mImpl;
            }

            void read(size_t offset, size_t cnt, T* dst) const
            {
                if((offset + cnt) * sizeof(T) >= size())
                    throw mReadError;
                std::copy(mVirt + offset, mVirt + offset + cnt, dst);
            }

            void write(size_t offset, size_t cnt, const T* src) const
            {
                if((offset + cnt) * sizeof(T) >= size())
                    throw mWriteError;
                std::copy(src, src + cnt, mVirt + offset);
            }

            T read(size_t offset) const
            {
                T value;
                read(offset, 1, &value);
                return value;
            }

            void write(size_t offset, T value) const
            {
                write(offset, 1, &value);
            }

            impl::Client* client(Cache cache, uintmax_t offset, uintmax_t size) const
            {
                return mImpl->client(cache, offset, size);
            }
        };


        ///
        /// @brief Dummy memory resource
        ///
        /// We do not, for example, want to burden reads and writes to/from PCI BARs
        /// that do not exist with conditionals, so we provide a resource with size 0.
        ///

        namespace dummy
        {
            class Client : public impl::Client
            {
                volatile void* vaddr() const
                {
                    return NULL;
                }

                impl::Client* client(Cache cache, uintmax_t offset, uintmax_t size) const
                {
                    return const_cast<Client*>(this); // FIXME!!
                }
            };

            class Resource : public mem::Resource
            {
            public:
                Resource()
                    : mem::Resource(0, 0, 0)
                {
                }

                impl::Client* client(Cache cache, uintmax_t offset, uintmax_t size_) const
                {
                    return new Client;
                }
            };

            extern Resource gResource;
        };
    };
};


#endif  // _enzyme_mem_h_
