﻿/// @file fpcontrol.h
/// @brief Floating Point control main header
/// @author gm
/// @copyright gm 2013
///
/// This file is part of FPControl
///
/// FPControl is free software: you can redistribute it and/or modify
/// it under the terms of the GNU General Public License as published by
/// the Free Software Foundation, either version 3 of the License, or
/// (at your option) any later version.
///
/// FPControl is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU General Public License for more details.
///
/// You should have received a copy of the GNU General Public License
/// along with FPControl.  If not, see <http://www.gnu.org/licenses/>.

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

/// @brief SSE configuration
#if defined(SET_SSE_VERSION)
#define _SSE_VERSION SET_SSE_VERSION
#else
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
  #endif
#endif  // defined(SET_SSE_VERSION)?

// If relying on x87 when targeting X64, something is unusual to say the least.
// Most of fp environment tweaks might be wrong, especially regarding denormals
#if (_SSE_VERSION < 1) && _SYSTEM_64B
#error "X64 requires SSE, check your compiler settings (typically -mno-sse)"
#endif

// Standard(ish) SSE includes
#if _SSE_VERSION >= 1
#include <xmmintrin.h>
#endif
#if _SSE_VERSION >= 3
#include <pmmintrin.h>
#endif

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
#elif(_SYSTEM_LINUX)
  #define FPC_INEXACT FE_INEXACT
  #define FPC_UNDERFLOW FE_UNDERFLOW
  #define FPC_OVERFLOW  FE_OVERFLOW
  #define FPC_DIVBYZERO FE_DIVBYZERO
  #define FPC_INVALID FE_INVALID
#endif  // _SYSTEM_ ?

/// @brief Shortcut for "all floating points exception"
#define FPC_ALL_EXCEPT FPC_DIVBYZERO | FPC_INEXACT | \
                       FPC_INVALID | FPC_OVERFLOW |  \
                       FPC_UNDERFLOW

/// @brief Floating points rounding mode constants
#if(_SYSTEM_WIN)
  #define FPC_DOWNWARD _RC_DOWN
  #define FPC_TONEAREST _RC_NEAR
  #define FPC_TOWARDZERO _RC_CHOP
  #define FPC_UPWARD  _RC_UP
#elif(_SYSTEM_LINUX)
  #define FPC_DOWNWARD FE_DOWNWARD
  #define FPC_TONEAREST FE_TONEAREST
  #define FPC_TOWARDZERO FE_TOWARDZERO
  #define FPC_UPWARD  FE_UPWARD
#endif  // _SYSTEM_ ?

/// @brief Type for floating points exception flags
#if(_SYSTEM_WIN)
  typedef unsigned int FPCexcept_t;
#elif(_SYSTEM_LINUX)
  typedef fexcept_t FPCexcept_t;
#endif  // _SYSTEM_ ?

/// @brief Clear any pending exception
int FPCClearExcept(void) {
#if(_SYSTEM_WIN)
  return _clearfp();
#elif(_SYSTEM_LINUX)
  return feclearexcept(FPC_ALL_EXCEPT);
#endif  // _SYSTEM_ ?
}

/// @brief Return the currently enabled exceptions
///
/// Unspecified exception masks stays unchanged
///
/// @return Currently enabled exceptions
unsigned int FPCGetExcept(void) {
#if(_SYSTEM_WIN)
  return _controlfp(0, 0) & _MCW_EM;
#elif(_SYSTEM_LINUX)
  return fegetexcept();
#endif  // _SYSTEM_ ?
}

/// @brief Enable the specified exceptions
///
/// Unspecified exception masks stays unchanged
///
/// @param[in]  excepts   Bitwise OR of the exceptions to be enabled
int FPCEnableExcept(int excepts) {
#if(_SYSTEM_WIN)
  return _controlfp(~excepts, _MCW_EM);
#elif(_SYSTEM_LINUX)
  return feenableexcept(excepts);
#endif  // _SYSTEM_ ?
}

/// @brief Disable the specified exceptions
///
/// Unspecified exception masks stays unchanged
///
/// @param[in]  excepts   Bitwise OR of the exceptions to be disabled
int FPCDisableExcept(int excepts) {
#if(_SYSTEM_WIN)
  return _controlfp((FPCGetExcept() & _MCW_EM) ^ excepts, _MCW_EM);
#elif(_SYSTEM_LINUX)
  return fedisableexcept(excepts);
#endif  // _SYSTEM_ ?
}

/// @brief Retrieve rounding mode
///
/// @return the rounding mode.
/// @see FE_DOWNWARD, FE_TONEAREST, FE_TOWARDZERO, FE_UPWARD
int FPCfegetround(void) {
#if(_SYSTEM_WIN)
  return _controlfp(0, 0) & _MCW_RC;
#elif(_SYSTEM_LINUX)
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
#elif(_SYSTEM_LINUX)
  return fesetround(round);
#endif  // _SYSTEM_ ?
}

/// @brief Type for previous floating points environnement
#if(_SYSTEM_WIN)
  typedef unsigned int FPCenv_t;
#elif(_SYSTEM_LINUX)
  typedef unsigned int FPCenv_t;
  /// @brief Bitmask for denormals FTZ (SSE control word)
  const unsigned int kDenormalsFTZ = 0x8000;
  /// @brief Bitmask for denormals DAZ (SSE control word)
  const unsigned int kDenormalsDAZ = 0x0040;
#elif(_SYSTEM_APPLE)
  typedef fenv_t FPCenv_t;
#endif  // _SYSTEM_ ?

/// @brief Static variable holding the previous floating-point environment
static FPCenv_t FPCprevious_env;

/// @brief Flush all denormals to zero
///
/// On Windows this will be done for both x87 and SSE,
/// SSE only for Apple and Linux
///
/// On Apple this can only be done by setting the whole FP environment
/// to default! That's why this environment has to be saved first.
///
/// For the same reason, do all other changes AFTER calling this function.
void FPCSetDenormalsFTZ(void) {
#if(_SYSTEM_WIN)
  // Save previous environment
  FPCprevious_env = _controlfp(0, 0);
  // Activate FTZ
  _controlfp_s(0, _DN_FLUSH, _MCW_DN);
#elif(_SYSTEM_LINUX)
  // Save previous value
  FPCprevious_env = _mm_getcsr();
  // Activate FTZ
  _mm_setcsr(_mm_getcsr() | (kDenormalsFTZ | kDenormalsDAZ));
#elif(_SYSTEM_APPLE)
  // Save previous environment
  fgetenv(&FPCprevious_env);
  // Set FP environment to default and activate FTZ
  fesetenv(FE_DFL_DISABLE_SSE_DENORMS_ENV);
#endif  // _SYSTEM_ ?
}

/// @brief Set back configuration regarding denormals to its previous state
///
/// On gcc/Clang this rely on the previously saved environment.
/// Note also that on these compiler the whole environment (not only denormals)
/// will be reset.
void FPCResetDenormals(void) {
#if(_SYSTEM_WIN)
  _controlfp(FPCprevious_env, _MCW_DN);
#elif(_SYSTEM_LINUX)
  _mm_setcsr(FPCprevious_env);
#elif(_SYSTEM_APPLE)
  fesetenv(FPCprevious_env);
#endif  // _SYSTEM_ ?
}

/// @brief Function testing whether or no a value is a denormal
/// Single-precision floating point values only.
///
/// @param[in]  value   Value to test
///
/// @return true if the value is a denormal
bool FPCIsDenormal(const float value) {
#if(_SYSTEM_WIN)
  // _fpclass() takes a double so we have to do this by hand
  union Number32b {
    int i;
    float f;
  } value_;
  value_.f = value;
  // 0x00800000: integer representation of the smallest normalized float
  return value_.i > 0 && value_.i < 0x00800000;
#elif(_SYSTEM_LINUX)
  return FP_SUBNORMAL == fpclassify(value);
#endif  // _SYSTEM_ ?
}

#if (_SYSTEM_WIN)
#pragma warning(pop)
#endif  // (_SYSTEM_WIN)

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // FPCONTROL_INC_FPCONTROL_H_
