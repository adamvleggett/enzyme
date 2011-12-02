///
/// @file    enzyme_type.h
/// @brief   Enzyme Hardware Abstraction Layer: Base Types
///
/// @author  Adam Leggett
///

// ---------------------------------------------------------------------------


#pragma once
#ifndef _enzyme_type_h_
#define _enzyme_type_h_


#if !defined(_MSC_VER) || (_MSC_VER >= 1600)    // C99 compliant
#include <stdint.h>
#endif


namespace enzyme
{
#if !defined(_MSC_VER) || (_MSC_VER >= 1600)
    typedef ::int8_t            int8_t;
    typedef ::int16_t           int16_t;
    typedef ::int32_t           int32_t;
    typedef ::int64_t           int64_t;
    typedef ::uint8_t           uint8_t;
    typedef ::uint16_t          uint16_t;
    typedef ::uint32_t          uint32_t;
    typedef ::uint64_t          uint64_t;
    typedef ::intptr_t          intptr_t;
    typedef ::uintptr_t         uintptr_t;
    typedef ::intmax_t          intmax_t;
    typedef ::uintmax_t         uintmax_t;

#else
    typedef signed __int8       int8_t;
    typedef signed __int16      int16_t;
    typedef signed __int32      int32_t;
    typedef signed __int64      int64_t;
    typedef unsigned __int8     uint8_t;
    typedef unsigned __int16    uint16_t;
    typedef unsigned __int32    uint32_t;
    typedef unsigned __int64    uint64_t;
#ifdef _WIN64
    typedef signed __int64      intptr_t;
    typedef unsigned __int64    uintptr_t;
#else
    typedef signed __int32      intptr_t;
    typedef unsigned __int32    uintptr_t;
#endif
    typedef signed __int64      intmax_t;
    typedef unsigned __int64    uintmax_t;
#endif
};


#endif  // _enzyme_type_h_
