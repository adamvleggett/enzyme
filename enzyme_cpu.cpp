///
/// @file    enzyme_cpu.cpp
/// @brief   Enzyme Hardware Abstraction Layer: CPU Core
///
/// @author  Adam Leggett
///

// ---------------------------------------------------------------------------


#include <cstring>
#include "enzyme_cpu.h"

#ifdef _WIN32
#include <windows.h>
#endif
#ifdef _MSC_VER
#include <intrin.h>
#endif
#ifdef __linux__
#include <sched.h>
#endif


// ---------------------------------------------------------------------------


enzyme::cpu::Enumerator::Enumerator()
{
    std::string name;

#ifdef __linux__
//    cpu_set_t affinold = {{0}};
//    cpu_set_t affinnew = {{1 << id}};
//    sched_getaffinity( 0, sizeof(affinold), &affinold );
//    sched_setaffinity( 0, sizeof(affinnew), &affinnew );
#endif

    int tmp[4];
    union {
        uint32_t u[3];
        char c[12];
    } vendor;
#if defined(_MSC_VER)
    __cpuid(tmp, 0); vendor.u[0] = tmp[1]; vendor.u[1] = tmp[3]; vendor.u[2] = tmp[2];
#elif defined(__x86_64__)
    __asm __volatile("mov %%rbx, %%rsi; cpuid; xchg %%rbx, %%rsi" : "=S" (vendor.u[0]), "=d" (vendor.u[1]), "=c" (vendor.u[2]) : "a" (0));
#else
    __asm __volatile("mov %%ebx, %%esi; cpuid; xchg %%ebx, %%esi" : "=S" (vendor.u[0]), "=d" (vendor.u[1]), "=c" (vendor.u[2]) : "a" (0));
#endif

#if !defined(__x86_64__)
    uint32_t version;
#if defined(_MSC_VER)
    __cpuid(tmp, 1); version = tmp[2];
#else
    __asm __volatile("mov %%ebx, %%esi; cpuid; xchg %%ebx, %%esi" : "=c" (version) : "a" (1));
#endif
    if(!memcmp(vendor.c, "GenuineIntel", 12) && ((version & 0x000000FF) < 4))
    {
        switch(version & 0x000000FF)
        {
        case 1:
            name = "Celeron"; break;
        case 2:
            name = "Pentium III"; break;
        case 3:
            name = "Pentium III Xeon"; break;
        default:
            name = "Unknown"; break;
        }
    }
    else {
        uint32_t e;
#if defined(_MSC_VER)
        __cpuid(tmp, 0x80000000); e = tmp[0];
#else
        __asm __volatile("mov %%ebx, %%esi; cpuid; xchg %%ebx, %%esi" : "=a" (e) : "a" (0x80000000));
#endif
        if(e < 0x80000004)
        {
            if(!memcmp(vendor.c, "GenuineIntel", 12))
                name = "Pentium 4";
            else
                name = "Unknown";
        }
        else {
#endif  // !__x86_64__

            union {
                uint32_t reg[3][4];
                char desc[49];
            } ext;
            memset(&ext, 0, sizeof(ext));
            int i;
            for(i = 0; i < 3; i++)
            {
#if defined(_MSC_VER)
                __cpuid(tmp, 0x80000002 + i); ext.reg[i][0] = tmp[0]; ext.reg[i][1] = tmp[1]; ext.reg[i][2] = tmp[2]; ext.reg[i][3] = tmp[3];
#elif defined(__x86_64__)
                __asm __volatile("mov %%rbx, %%rsi; cpuid; xchg %%rbx, %%rsi" : "=a" (ext.reg[i][0]), "=S" (ext.reg[i][1]), "=c" (ext.reg[i][2]), "=d" (ext.reg[i][3]) : "a" (0x80000002 + i));
#else
                __asm __volatile("mov %%ebx, %%esi; cpuid; xchg %%ebx, %%esi" : "=a" (ext.reg[i][0]), "=S" (ext.reg[i][1]), "=c" (ext.reg[i][2]), "=d" (ext.reg[i][3]) : "a" (0x80000002 + i));
#endif
            }
            name = ext.desc;

#if !defined(__x86_64__)
        }
    }
#endif

#ifdef __linux__
//    sched_setaffinity( 0, sizeof(affinold), &affinold );
#endif

    std::string mfr;
    std::string cls(vendor.c, 12);
    if(!memcmp(vendor.c, "AuthenticAMD", 12))
        mfr = "AMD";
    else if(!memcmp(vendor.c, "GenuineIntel", 12))
        mfr = "Intel Corporation";
    else
        mfr = cls;

#if defined(__linux__) || defined(__APPLE__)
    unsigned short cnt = sysconf(_SC_NPROCESSORS_CONF);
#elif defined(_WIN32)
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    unsigned short cnt = static_cast<unsigned short>(si.dwNumberOfProcessors);
#else
    unsigned short cnt = 1;
#endif
    unsigned short ind = 0;
    while(ind < cnt)
    {
        child().push_back(new Core(cls, mfr, name));
        ind++;
    }
}


enzyme::cpu::Enumerator::~Enumerator()
{
}
