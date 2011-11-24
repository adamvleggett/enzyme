///
/// @file    enzyme.h
/// @brief   Enzyme Hardware Abstraction Layer
///
/// @author  Adam Leggett
///

// ---------------------------------------------------------------------------


#pragma once
#ifndef _enzyme_h_
#define _enzyme_h_


#include "enzyme_type.h"

#include <list>
#include <sstream>


namespace enzyme
{
    class Device;


    ///
    /// @brief Any object that can be output to a stream
    ///

    class AutoLex
    {
    public:
        std::string str() const
        {
            std::ostringstream ss;
            lex(ss);
            return ss.str();
        }

        virtual std::ostream& lex(std::ostream& os) const = 0;
    };

    inline std::ostream& operator<<(std::ostream& os, const AutoLex& lex)
    {
        return lex.lex(os);
    }


    ///
    /// @brief Node in device tree: device or enumerator
    ///

    class Node
    {
    protected:
        std::list<Device*> mChild;

    public:
        std::list<Device*>& child() { return mChild; }
        std::list<Device*> child(const std::string& id);
    };


    ///
    /// @brief Abstract device instance
    ///

    class Device : public Node
    {
    protected:
        std::string mEnumerator;
        std::string mService;

        const AutoLex& mLocation;
        const AutoLex& mClass;
        const AutoLex& mVendor;
        const AutoLex& mDescription;

    public:
        Device(const AutoLex& location, const AutoLex& classid, const AutoLex& vendor, const AutoLex& description)
            : mLocation(location)
            , mClass(classid)
            , mVendor(vendor)
            , mDescription(description)
        {
        }

        const std::string& enumerator()     const { return mEnumerator; }
        const std::string& service()        const { return mService; }

        const AutoLex& location()           const { return mLocation; }
        const AutoLex& classid()            const { return mClass; }
        const AutoLex& vendor()             const { return mVendor; }
        const AutoLex& description()        const { return mDescription; }
    };


    ///
    /// @brief Enumerator forward declarations
    ///

    namespace cpu
    {
        class Enumerator;
    };

    namespace pci
    {
        class Enumerator;
    };


    ///
    /// @brief System enumerator
    ///

    class Enumerator : public Device
    {
    protected:
        cpu::Enumerator* mCPU;
        pci::Enumerator* mPCI;

    public:
        Enumerator();
        ~Enumerator();

        cpu::Enumerator& cpu() { return *mCPU; }
        pci::Enumerator& pci() { return *mPCI; }
    };
};


#endif  // _enzyme_h_
