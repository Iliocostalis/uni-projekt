#pragma once
#include <Config.h>

#if DEFINED(WINDOWS)
    #include <intrin.h>
#else
    #include <csignal>
#endif

#if DEFINED(DEBUG)
    #if DEFINED(WINDOWS)
        #define ASSERT(x) if(!(x)){__debugbreak();}
        #define ASSERT_ZERO(x) if(x){__debugbreak();}
    #else
        #define ASSERT(x) if(!(x)){raise(SIGTRAP);}
        #define ASSERT_ZERO(x) if(x){raise(SIGTRAP);}
    #endif
#else
    #define ASSERT(x) 
    #define ASSERT_ZERO(x)
#endif