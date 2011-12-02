///
/// @file    enzyme_system.cpp
/// @brief   Enzyme Hardware Abstraction Layer: System Enumerator
///
/// @author  Adam Leggett
///

// ---------------------------------------------------------------------------


#include "enzyme.h"
#include "enzyme_cpu.h"
#include "enzyme_pci.h"


// ---------------------------------------------------------------------------


namespace enzyme
{
    class EmptyLex : public AutoLex
    {
        std::ostream& lex(std::ostream& os) const { return os; }
    };

    class SystemLex : public AutoLex
    {
        std::ostream& lex(std::ostream& os) const { return (os << "System"); }
    };

    static EmptyLex gEmpty;
    static SystemLex gSystem;
};


// ---------------------------------------------------------------------------


///
/// @brief System Enumerator constructor
///

enzyme::Enumerator::Enumerator()
    : enzyme::Device(gEmpty, gEmpty, gEmpty, gSystem)
{
    mCPU = new cpu::Enumerator;
    mChild.insert(mChild.end(), cpu().child().begin(), cpu().child().end());
    mPCI = new pci::Enumerator;
    mChild.insert(mChild.end(), pci().child().begin(), pci().child().end());
}


///
/// @brief System Enumerator destructor
///

enzyme::Enumerator::~Enumerator()
{
    delete mPCI;
    delete mCPU;
}
