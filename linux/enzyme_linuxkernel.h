///
/// @file    enzyme_linuxkernel.h
/// @brief   Enzyme Hardware Abstraction Layer: Linux Platform
///
/// @author  Adam Leggett
///

// ---------------------------------------------------------------------------


#pragma once
#ifndef _enzyme_linuxkernel_h_
#define _enzyme_linuxkernel_h_


#include <string>


namespace enzyme
{
    namespace kernel
    {
        bool have_sysfs();
    };
};


#endif  // _enzyme_linuxkernel_h_
