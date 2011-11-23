///
/// @file    enzyme_platform.h
/// @brief   Enzyme Hardware Abstraction Layer: Platform Dependency
///
/// @author  Adam Leggett
///

//-----------------------------------------------------------------------------


#pragma once
#ifndef _enzyme_platform_h_
#define _enzyme_platform_h_


#if defined(_WIN32)
#include "win/enzyme_winkernel.h"
#include "win/enzyme_winmem.h"
#include "win/enzyme_winport.h"
#include "win/enzyme_winpci.h"

#elif defined(__linux__)
#include "linux/enzyme_linuxkernel.h"
#include "linux/enzyme_linuxmem.h"
#include "linux/enzyme_linuxport.h"
#include "linux/enzyme_linuxpci.h"

#else
#error Unknown platform
#endif


#endif  // _enzyme_platform_h_
