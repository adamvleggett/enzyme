///
/// @file    enzyme_port.h
/// @brief   Enzyme Hardware Abstraction Layer: I/O Port Resource
///
/// @author  Adam Leggett
///

// ---------------------------------------------------------------------------


#pragma once
#ifndef _enzyme_port_h_
#define _enzyme_port_h_


#include "enzyme.h"


namespace enzyme
{
    namespace port
    {

        ///
        /// @brief Abstract port resource accessor
        ///

        namespace impl
        {
            class Client
            {
            public:
                virtual ~Client() { };
                virtual Client* client() = 0;
            };
        };



        ///
        /// @brief I/O Port Resource
        ///

        class Resource
        {
        protected:
            uint16_t mBase;
            uint16_t mSize;

        public:
            Resource(uint16_t base, uint16_t size)
                : mBase(base)
                , mSize(size)
            {
            }

            virtual ~Resource()
            {
            }

            uint16_t base() const { return mBase; }
            uint16_t size() const { return mSize; }

            bool compare(uint16_t base, uint16_t size) const
            {
                return (mBase > base) || (mBase + mSize < base + size);
            }

            bool operator<(const Resource& other) const
            {
                return (mBase < other.mBase);
            }

            virtual impl::Client* client() const = 0;
        };


        ///
        /// @brief I/O Port resource accessor
        ///

        template<typename T> class Client : public Resource
        {
        protected:
            impl::Client* mImpl;

        public:
            Client(Resource& resource)
                : mImpl(resource.client())
            {
            }

            ~Client()
            {
                delete mImpl;
            }

            T read(unsigned short offset) const
            {
                return 0;
            }

            void write(unsigned short offset, T value) const
            {
            }

            impl::Client* client() const
            {
                return mImpl->client();
            }
        };
    };
};


#endif  // _enzyme_port_h_
