///
/// @file    enzyme_pci.h
/// @brief   Enzyme Hardware Abstraction Layer: PCI Device
///
/// @author  Adam Leggett
///

// ---------------------------------------------------------------------------


#pragma once
#ifndef _enzyme_pci_h_
#define _enzyme_pci_h_


#include "enzyme.h"
#include "enzyme_mem.h"
#include "enzyme_port.h"

#include <iomanip>
#include <list>
#include <set>

#if _MSC_VER >= 1600
#include <unordered_map>
#else
#include <tr1/unordered_map>
#endif


namespace enzyme
{
    namespace pci
    {
        namespace os
        {
            class Enumerator;
            class Device;
            class Client;
        };


        ///
        /// @brief PCI bus location
        ///

        class Location : public AutoLex
        {
        private:
            union {
                struct {
                    unsigned int mDomain    : 16;
                    unsigned int mBus       : 8;
                    unsigned int mDevice    : 5;
                    unsigned int mFunction  : 3;
                };
                unsigned int mAll;
            };

        public:
            Location()
                : mDomain(0)
                , mBus(0)
                , mDevice(0)
                , mFunction(0)
            {
            }

            Location(unsigned int domain, unsigned int bus, unsigned int device, unsigned int function)
                : mDomain(domain)
                , mBus(bus)
                , mDevice(device)
                , mFunction(function)
            {
            }

            bool operator<(const Location& other) const
            {
                return (to_i() < other.to_i());
            }

            bool operator==(const Location& other) const
            {
                return (to_i() == other.to_i());
            }

            unsigned int domain()     const { return mDomain; }
            unsigned int bus()        const { return mBus; }
            unsigned int device()     const { return mDevice; }
            unsigned int function()   const { return mFunction; }

            int to_i() const
            {
                return mAll;
            }

            std::ostream& lex(std::ostream& os) const
            {
                return os << "PCI:" << std::hex << std::nouppercase << std::noshowbase << std::setfill('0') << std::setw(4) << domain() << ':' << std::setw(2) << bus() << ':' << std::setw(2) << device() << '.' << std::setw(1) << function();
            }
        };


        ///
        /// @brief PCI configuration space data
        ///

        class Config : public AutoLex
        {
        private:
            unsigned short mVendor;
            unsigned short mDevice;
            unsigned short mSubVendor;
            unsigned short mSubDevice;
            unsigned int mClass;

        public:
            Config()
                : mVendor(~0)
                , mDevice(~0)
                , mSubVendor(~0)
                , mSubDevice(~0)
                , mClass(0)
            {
            }

            Config(unsigned short vendor, unsigned short device, unsigned short subvendor = ~0, unsigned short subdevice = ~0, unsigned int classid = 0)
                : mVendor(vendor)
                , mDevice(device)
                , mSubVendor(subvendor)
                , mSubDevice(subdevice)
                , mClass(classid)
            {
            }

            unsigned short vendor()     const { return mVendor; }
            unsigned short device()     const { return mDevice; }
            unsigned short subvendor()  const { return mSubVendor; }
            unsigned short subdevice()  const { return mSubDevice; }
            unsigned int   classid()    const { return mClass; }

            std::ostream& lex(std::ostream& os) const
            {
                os << std::hex << std::uppercase << std::noshowbase << std::setfill('0') << "pci:" << std::setw(4) << vendor();
                if(device() != 0xFFFF)
                    os << ',' << std::setw(4) << device();
                if(subvendor() != 0xFFFF)
                    os << ',' << std::setw(4) << subvendor();
                if(subdevice() != 0xFFFF)
                    os << ',' << std::setw(4) << subdevice();
                return os;
            }
        };


        ///
        /// @brief PCI vendor name
        ///

        class Vendor : public AutoLex
        {
        private:
            static std::tr1::unordered_map<unsigned short, const char*> mVendorID;
            unsigned short mVendor;

        public:
            Vendor(unsigned short vendor);
            std::ostream& lex(std::ostream& os) const;
        };


        ///
        /// @brief PCI device
        ///

        class Enumerator;

        class Device : public enzyme::Device
        {
        protected:
            const Enumerator& mEnumerator;
            const Location mLocation;
            const Config mConfig;
            const Vendor mVendor;

            std::set<const mem::Resource*> mMemResource;
            std::set<const port::Resource*> mPortResource;

        public:
            Device(const Enumerator& enumerator, const Location& location, const Config& config)
                : enzyme::Device(location, config, mVendor, mVendor)
                , mEnumerator(enumerator)
                , mLocation(location)
                , mConfig(config)
                , mVendor(config.vendor())
            {
            }

            const Enumerator& enumerator()  const { return mEnumerator; }
            const Location& location()      const { return mLocation; }
            const Config& config()          const { return mConfig; }

            const std::set<const mem::Resource*>& mem()   const  { return mMemResource; }
            const std::set<const port::Resource*>& port() const  { return mPortResource; }

            bool operator<(const Device& other) const
            {
                return (location() < other.location());
            }
        };


        ///
        /// @brief Interface to abstract (real or emulated) PCI device
        ///

        class Client
        {
        private:
            os::Client* mImpl;
            const pci::Device& mDevice;

            mem::Client<uint32_t>*  mMMReg32;
            mem::Client<uint16_t>*  mMMReg16;
            mem::Client<uint8_t>*   mMMReg8;

            port::Client<uint32_t>* mPMReg32;
            port::Client<uint16_t>* mPMReg16;
            port::Client<uint8_t>*  mPMReg8;

        public:
            Client(const Device& dev, bool writable = true, bool exclusive = false);
            ~Client();

            const Device& device() const { return mDevice; }

            /// @brief Memory Mapped Register I/O
            const mem::Client<uint32_t>&  mmreg32() const { return *mMMReg32; }
            const mem::Client<uint16_t>&  mmreg16() const { return *mMMReg16; }
            const mem::Client<uint8_t>&   mmreg8()  const { return *mMMReg8; }

            /// @brief Port Mapped Register I/O
            const port::Client<uint32_t>& pmreg32() const { return *mPMReg32; }
            const port::Client<uint16_t>& pmreg16() const { return *mPMReg16; }
            const port::Client<uint8_t>&  pmreg8()  const { return *mPMReg8; }

            /// @brief PCI Config I/O
            void cfgr(size_t offset, size_t len, void* dst);
            void cfgw(size_t offset, size_t len, const void* src);

            inline uint8_t   cfgr8 (size_t offset) { uint8_t  r; cfgr(offset, 1, &r); return r; }
            inline uint16_t  cfgr16(size_t offset) { uint16_t r; cfgr(offset, 2, &r); return r; }
            inline uint32_t  cfgr32(size_t offset) { uint32_t r; cfgr(offset, 4, &r); return r; }

            inline void      cfgw8 (size_t offset, uint8_t  value) { cfgw(offset, 1, &value); }
            inline void      cfgw16(size_t offset, uint16_t value) { cfgw(offset, 2, &value); }
            inline void      cfgw32(size_t offset, uint32_t value) { cfgw(offset, 4, &value); }
        };


        ///
        /// @brief PCI bus enumerator
        ///

        class Enumerator : public enzyme::Node
        {
        protected:
            std::list<Device> mDevice;

        public:
            Device* device(const Location& location);

            std::list<Device>& device() { return mDevice; }
//            std::list<Device*> device(const Location& location);
            std::list<Device*> device(const Config& config);
        };
    };
};


#endif  // _enzyme_pci_h_
