///
/// @file    enzyme_linuxkernel.cpp
/// @brief   Enzyme Hardware Abstraction Layer: Kernel Service
///
/// @author  Adam Leggett
///

// ---------------------------------------------------------------------------


#include "enzyme_linuxkernel.h"

#include <cerrno>
#include <sys/stat.h>


// ---------------------------------------------------------------------------


///
/// @brief Return true if Sysfs is supported; false otherwise
///

bool enzyme::kernel::have_sysfs()
{
    static bool already = false;
    static bool result;

    if(already)
        return result;

    struct stat st;
    if(stat("/sys", &st) < 0)
        result = false;
    else if((st.st_mode & S_IFMT) != S_IFDIR)
        result = false;
    else
        result = true;

    already = true;
    return result;
}
