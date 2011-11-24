///
/// @file    enzyme_winkernel.h
/// @brief   Enzyme Hardware Abstraction Layer: Windows Platform
///
/// @author  Adam Leggett
///

// ---------------------------------------------------------------------------


#pragma once
#ifndef _enzyme_winkernel_h_
#define _enzyme_winkernel_h_


#include <string>
#include <windows.h>


namespace enzyme
{
    namespace kernel
    {

        ///
        /// @brief Installed Kernel Service (enzyme32.sys/enzyme64.sys)
        ///

        class Service
        {
        private:
            SC_HANDLE mSCHandle;
            HANDLE mHandle;

            static void exit();

        public:
            Service();
            ~Service();

            BOOL start(SC_HANDLE scm, const TCHAR* path);
            BOOL stop();

            HANDLE handle() const { return mHandle; }
        };


        ///
        /// @brief Client of Installed Kernel Service
        ///

        class Client
        {
        protected:
            HANDLE mHandle;

        public:
            Client();
            ~Client();

            HANDLE handle() const { return mHandle; }
        };


        std::string lasterror();

        extern Service* gService;
    };
};


#endif  // _enzyme_winkernel_h_
