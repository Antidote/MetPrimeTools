#ifndef ZEUS_GLOBAL_HPP
#define ZEUS_GLOBAL_HPP

#include <immintrin.h>

#if _M_IX86_FP >= 1 || _M_X64
#define __SSE__ 1
#endif

#endif //ZEUS_GLOBAL_HPP

