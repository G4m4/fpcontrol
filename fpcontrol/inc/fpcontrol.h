/// @file fpcontrol.h
/// @brief Floating Point control main header
/// @author gm
/// @copyright gm 2017
///
/// This file is part of FPControl
///
///            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
///                    Version 2, December 2004
///
/// Copyright (C) 2004 Sam Hocevar <sam@hocevar.net>
///
/// Everyone is permitted to copy and distribute verbatim or modified
/// copies of this license document, and changing it is allowed as long
/// as the name is changed.
///
///            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
///   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
///
///  0. You just DO WHAT THE FUCK YOU WANT TO.

#ifndef FPCONTROL_INC_FPCONTROL_H_
#define FPCONTROL_INC_FPCONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// @brief System detection
#if(defined(__linux))
  #define _SYSTEM_LINUX 1
#elif(defined(_WIN32) || defined(_WIN64))
  #define _SYSTEM_WIN 1
#elif(defined(__APPLE__))
  #define _SYSTEM_APPLE 1
#else
  #error "System could not be detected"
#endif

#include <stdint.h>
#if(INTPTR_MAX == INT64_MAX)
#define _SYSTEM_32B 0
#define _SYSTEM_64B 1
#else
#define _SYSTEM_32B 1
#define _SYSTEM_64B 0
#endif

/// @brief Compiler detection
#if(defined(__GNUC__))
#define _COMPILER_GCC 1
#elif(defined(__clang__))
#define _COMPILER_CLANG 1
#elif(defined(_MSC_VER))
#define _COMPILER_MSVC 1
#else
#error "Compiler could not be detected"
#endif

/// @brief SSE detection
#if(_COMPILER_GCC) || (_COMPILER_CLANG)
  #if defined(__SSE__)
  #define _SSE_VERSION 1
  #elif (defined(__SSE3__))
  #defined _SSE_VERSION 3
  #endif
#elif(_COMPILER_MSVC)
  #if _M_IX86_FP >= 1
  #define _SSE_VERSION 1
  // Microsoft's cl does not define anything between SSE2 and AVX,
  // sadly this is exactly what we would need, hence this weird definition
  #elif defined __AVX__
  #define _SSE_VERSION 3
  #endif
#else
  #define _SSE_VERSION 0
#endif  // _COMPILER?

/// @brief SSE detection
// On gcc and Clang SSE is opt-in, so notify the user about any mismatch
#if defined(SET_SSE_VERSION)
#if(_COMPILER_GCC) || (_COMPILER_CLANG)
  #if SET_SSE_VERSION < _SSE_VERSION
    #error "SSE set version and the one used for compilation do not match, make sure you build with -msse or -msse3 arguments"
  #endif  // SET_SSE_VERSION < _SSE_VERSION?
#endif  // _COMPILER?
// From now on always use _SSE_VERSION
#undef _SSE_VERSION
#define _SSE_VERSION SET_SSE_VERSION
#endif  // defined(SET_SSE_VERSION)?

// If relying on x87 when targeting X64, something is unusual to say the least.
// Most of fp environment tweaks might be wrong, especially regarding denormals
#if (_SSE_VERSION < 1) && _SYSTEM_64B
#error "X64 requires SSE, check your compiler settings (typically -mno-sse)"
#endif

// Standard(ish) SSE includes
// On Windows explicitly include the right header,
// otherwise let the more or less standard header do it
#if _SYSTEM_WIN
  #if _SSE_VERSION >= 1
  #include <xmmintrin.h>
  #endif
  #if _SSE_VERSION >= 3
  #include <pmmintrin.h>
  #endif
#else  // _SYSTEM?
  #include <x86intrin.h>
#endif  // _SYSTEM?

#if(_SYSTEM_WIN)
  #include <float.h>
#elif(_SYSTEM_LINUX)
  // In order to use feenableexcept(), etc.
  #include <fenv.h>
  // required for fpclassify()
  #include <math.h>
#elif(_SYSTEM_APPLE)
  #include <fenv.h>
  // required for fpclassify()
  #include <math.h>
#endif  // _SYSTEM_ ?

// Notice we don't notify the compiler about messing with floating point settings with the following
// #pragma STDC FENV_ACCESS ON
// The reasons are:
// - We expect the main usage of this header would be getting rid of denormals
// - We do not want to wreak havok in users performance by enabling this
// - This is not widely supported among compilers
// In case the user does use say a custom precision mode, it might be required though

#if (_SYSTEM_WIN)
// Disable warning "This function or variable may be unsafe. Consider using..."
#pragma warning(push)
#pragma warning(disable : 4996)
#endif  // (_SYSTEM_WIN)

// Wrap system-specific definitions into common one.
// Apple and Linux will usually rely on C99 standard fenv.h stuff,
// where Windows will be using its own specific functions.

/// @brief Floating points exception constants
#if(_SYSTEM_WIN)
  #define FPC_INEXACT _EM_INEXACT
  #define FPC_UNDERFLOW _EM_UNDERFLOW
  #define FPC_OVERFLOW  _EM_OVERFLOW
  #define FPC_DIVBYZERO _EM_ZERODIVIDE
  #define FPC_INVALID _EM_INVALID
#elif(_SYSTEM_LINUX) | (_SYSTEM_APPLE)
  #define FPC_INEXACT FE_INEXACT
  #define FPC_UNDERFLOW FE_UNDERFLOW
  #define FPC_OVERFLOW  FE_OVERFLOW
  #define FPC_DIVBYZERO FE_DIVBYZERO
  #define FPC_INVALID FE_INVALID
#endif  // _SYSTEM_ ?

/// @brief Shortcut for "all floating points exception"
#define FPC_ALL_EXCEPT ( FPC_DIVBYZERO | FPC_INEXACT | \
                       FPC_INVALID | FPC_OVERFLOW |  \
                       FPC_UNDERFLOW )

/// @brief Floating points rounding mode constants
#if(_SYSTEM_WIN)
  #define FPC_DOWNWARD _RC_DOWN
  #define FPC_TONEAREST _RC_NEAR
  #define FPC_TOWARDZERO _RC_CHOP
  #define FPC_UPWARD  _RC_UP
#elif((_SYSTEM_LINUX) || (_SYSTEM_APPLE))
  #define FPC_DOWNWARD FE_DOWNWARD
  #define FPC_TONEAREST FE_TONEAREST
  #define FPC_TOWARDZERO FE_TOWARDZERO
  #define FPC_UPWARD  FE_UPWARD
#endif  // _SYSTEM_ ?

/// @brief Type for floating points exception flags
#if(_SYSTEM_WIN)
  typedef unsigned int FPCexcept_t;
#elif((_SYSTEM_LINUX) || (_SYSTEM_APPLE))
  typedef fexcept_t FPCexcept_t;
#endif  // _SYSTEM_ ?

/// @brief Clear any pending exception
FPCexcept_t FPCClearExcept(void) {
#if(_SYSTEM_WIN)
  return _controlfp(0, FPC_ALL_EXCEPT);
#elif((_SYSTEM_LINUX) || (_SYSTEM_APPLE))
  return feclearexcept(FPC_ALL_EXCEPT);
#endif  // _SYSTEM_ ?
}

/// @brief Return the currently enabled exceptions
///
/// Unspecified exception masks stays unchanged
///
/// @return Currently enabled exceptions
FPCexcept_t FPCGetExcept(void) {
  FPCexcept_t out = FPC_ALL_EXCEPT;
#if(_SYSTEM_WIN)
  out &= _controlfp(0, 0);
#elif((_SYSTEM_LINUX) || (_SYSTEM_APPLE))
  fegetexceptflag(&out, FE_ALL_EXCEPT);
#endif  // _SYSTEM_ ?
  return out;
}

/// @brief Enable the specified exceptions
///
/// Unspecified exception masks stays unchanged
///
/// @param[in]  excepts   Bitwise OR of the exceptions to be enabled
FPCexcept_t FPCEnableExcept(FPCexcept_t excepts) {
  const FPCexcept_t value = FPC_ALL_EXCEPT & excepts;
#if(_SYSTEM_WIN)
  return _controlfp(value, FPC_ALL_EXCEPT);
#elif((_SYSTEM_LINUX) || (_SYSTEM_APPLE))
  return fesetexceptflag(&value, FE_ALL_EXCEPT);
#endif  // _SYSTEM_ ?
}

/// @brief Disable the specified exceptions
///
/// Unspecified exception masks stays unchanged
///
/// @param[in]  excepts   Bitwise OR of the exceptions to be disabled
FPCexcept_t FPCDisableExcept(FPCexcept_t excepts) {
  const FPCexcept_t current = FPCGetExcept();
  const FPCexcept_t value = FPC_ALL_EXCEPT & excepts;
  const FPCexcept_t not_excepts = current & ~value;
#if(_SYSTEM_WIN)
  return _controlfp(not_excepts, FPC_ALL_EXCEPT);
#elif((_SYSTEM_LINUX) || (_SYSTEM_APPLE))
  return fesetexceptflag(&not_excepts, FE_ALL_EXCEPT);
#endif  // _SYSTEM_ ?
}

/// @brief Retrieve rounding mode
///
/// @return the rounding mode.
/// @see FE_DOWNWARD, FE_TONEAREST, FE_TOWARDZERO, FE_UPWARD
int FPCfegetround(void) {
#if(_SYSTEM_WIN)
  return _controlfp(0, 0) & _MCW_RC;
#elif((_SYSTEM_LINUX) || (_SYSTEM_APPLE))
  return fegetround();
#endif  // _SYSTEM_ ?
}

/// @brief Set rounding mode
///
/// @param[in]  mode    Mode to set the FP environment to
/// @see FE_DOWNWARD, FE_TONEAREST, FE_TOWARDZERO, FE_UPWARD
int FPCfesetround(int round) {
#if(_SYSTEM_WIN)
  return _controlfp(round, _MCW_RC);
#elif((_SYSTEM_LINUX) || (_SYSTEM_APPLE))
  return fesetround(round);
#endif  // _SYSTEM_ ?
}

/// @brief Type for floating point environnement
#if(_SYSTEM_WIN)
  typedef unsigned int FPCenv_t;
#elif((_SYSTEM_LINUX) || (_SYSTEM_APPLE))
  typedef fenv_t FPCenv_t;
#endif  // _SYSTEM_ ?

/// @brief Save the entire floating-point environment
/// If relevant (e.g. 32b) this means for both x87 and SSE
void FPCSaveEnv(FPCenv_t* out) {
#if(_SYSTEM_WIN)
  *out = _controlfp(0, 0);
#elif((_SYSTEM_LINUX) || (_SYSTEM_APPLE))
  fegetenv(out);
#endif  // _SYSTEM_ ?
}

/// @brief Load the entire floating-point environment
/// If relevant (e.g. 32b) this means for both x87 and SSE
void FPCLoadEnv(const FPCenv_t* in) {
#if(_SYSTEM_WIN)
  _controlfp(*in, _MCW_EM | _MCW_RC | _MCW_DN);
#elif((_SYSTEM_LINUX) || (_SYSTEM_APPLE))
  fesetenv(in);
#endif  // _SYSTEM_ ?
}

/// @brief Function testing whether or no a value is a denormal
/// Single-precision floating point values only.
/// Beware, it tests for what range the value is in,
/// regardless of the FPU configuration
///
/// @param[in]  value   Value to test
///
/// @return true if the value is a denormal
bool FPCIsDenormal(const float value) {
  // On Windows _fpclass() takes a double so we have to do this by hand
  // On other systems the C version of fpclassify is not handling single floats
  union Number32b {
    int i;
    float f;
  } value_;
  value_.f = value;
  // 0x00800000: integer representation of the smallest normalized float
  return value_.i > 0 && value_.i < 0x00800000;
}

/// Below are sse-specific routines, typically to be used on x64
#if _SSE_VERSION >= 1
void FPCSetDenormalsFTZ_SSE(void) {
  _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
}

void FPCSetDenormalsDAZ_SSE(void) {
#if _SSE_VERSION >= 3
  _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);
#endif // _SSE_VERSION >= 3?
}

/// @brief Get rid of denormals
///
/// Make so the SSE control unit:
/// - flush denormals to zero when having computed one (requires SSE1)
/// - consider denormals operands as zeros (requires SSE3)
/// Both will be set if the SSE configuration is suitable
void FPCNoDenormals(void) {
  FPCSetDenormalsFTZ_SSE();
#if _SSE_VERSION >= 3
  FPCSetDenormalsDAZ_SSE();
#endif // _SSE_VERSION >= 3?
}
#endif // _SSE_VERSION >= 1?

#if (_SYSTEM_WIN)
#pragma warning(pop)
#endif  // (_SYSTEM_WIN)

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // FPCONTROL_INC_FPCONTROL_H_
