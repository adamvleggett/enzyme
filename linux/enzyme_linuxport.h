///
/// @file    enzyme_linuxport.h
/// @brief   Enzyme Hardware Abstraction Layer: Linux Platform / I/O Port Resource
///
/// @author  Adam Leggett
///

// ---------------------------------------------------------------------------


#pragma once
#ifndef _enzyme_linuxport_h_
#define _enzyme_linuxport_h_


#include "../enzyme_port.h"


namespace enzyme
{
    namespace port
    {
        namespace os
        {
            class Resource : public port::Resource
            {
            protected:
                unsigned int mIndex;

            public:
                Resource(unsigned int index)
                    : port::Resource(static_cast<uint16_t>(0),
                                     static_cast<uint16_t>(0))
                    , mIndex(index)
                {
                }
            };

            class Client
            {
            public:
            };
        };
    };
};


#endif  // _enzyme_linuxport_h_
