///
/// @file    enzyme_cpu.h
/// @brief   Enzyme Hardware Abstraction Layer: CPU Core
///
/// @author  Adam Leggett
///

// ---------------------------------------------------------------------------


#pragma once
#ifndef _enzyme_cpu_h_
#define _enzyme_cpu_h_


#include "enzyme.h"


namespace enzyme
{
    namespace cpu
    {

        ///
        /// @brief CPU core
        ///

        class Core : public enzyme::Device
        {
        public:
            Core(const std::string& classid, const std::string& manufacturer, const std::string& name);
            ~Core();
        };


        ///
        /// @brief CPU core enumerator
        ///

        class Enumerator : public enzyme::Node
        {
        protected:
            std::list<Core> mCore;

        public:
            Enumerator();
            ~Enumerator();

            Core* activecore();
//            Core* core(const Location& location);

            std::list<Core>& core() { return mCore; }
        };
    };
};


#endif  // _enzyme_cpu_h_
