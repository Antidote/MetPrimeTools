#ifndef ZEUS_GLOBAL_HPP
#define ZEUS_GLOBAL_HPP

#include <immintrin.h>

#if _M_IX86_FP >= 1 || _M_X64
#   define __SSE__ 1
#endif

#if __SSE__
#   ifdef _MSC_VER
#       define ZE_ALIGN(x) __declspec(align(x))
#   else
#       define ZE_ALIGN(x) __attribute__((aligned(x)))
#   endif
#else
#   define ZE_ALIGN(x)
#endif

#endif //ZEUS_GLOBAL_HPP

