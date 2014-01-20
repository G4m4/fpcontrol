/// @filename fpcontrol.h
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

/// @brief Compiler detection
#if(defined(__GNUC__))
  #define _COMPILER_GCC 1
#elif(defined(_MSC_VER))
  #define _COMPILER_MSVC 1
#else
  #error "Compiler could not be detected"
#endif

#if(_COMPILER_MSVC)
  #include <float.h>
#elif(_COMPILER_GCC)
  #include <fenv.h>
  // required for fpclassify()
  #include <math.h>
  #pragma STDC FENV_ACCESS ON
#endif  // _COMPILER_ ?

#if (_COMPILER_MSVC)
// Disable warning "This function or variable may be unsafe. Consider using..."
#pragma warning(push)
#pragma warning(disable : 4996)
#endif  // (_COMPILER_MSVC)

// Wrap compiler-specific definitions into common one.
// Gcc (and Clang) will rely on C99 standard fenv.h stuff,
// where msvc will be using its own specific functions.

// Mostly we are sticking with C99 stuff here
/// Note that, where most C99 functions allow a per-exception flag access,
/// MSVC does not for clearing/reading flags.
/// Hence this wrapper does not provide per-exception flag clearing/reading.

/// @brief Floating points exception constants
#if(_COMPILER_MSVC)
  #define FPC_DIVBYZERO _EM_ZERODIVIDE
  #define FPC_INEXACT _EM_INEXACT
  #define FPC_INVALID _EM_INVALID
  #define FPC_OVERFLOW  _EM_OVERFLOW
  #define FPC_UNDERFLOW _EM_UNDERFLOW
#elif(_COMPILER_GCC)
  #define FPC_DIVBYZERO FE_DIVBYZERO
  #define FPC_INEXACT FE_INEXACT
  #define FPC_INVALID FE_INVALID
  #define FPC_OVERFLOW  FE_OVERFLOW
  #define FPC_UNDERFLOW FE_UNDERFLOW
#endif  // _COMPILER_ ?

/// @brief Shortcut for "all floating points exception"
#define FPC_ALL_EXCEPT FPC_DIVBYZERO | FPC_INEXACT | \
                       FPC_INVALID | FPC_OVERFLOW |  \
                       FPC_UNDERFLOW

/// @brief Floating points rounding mode constants
#if(_COMPILER_MSVC)
  #define FPC_DOWNWARD _RC_DOWN
  #define FPC_TONEAREST _RC_NEAR
  #define FPC_TOWARDZERO _RC_CHOP
  #define FPC_UPWARD  _RC_UP
#elif(_COMPILER_GCC)
  #define FPC_DOWNWARD FE_DOWNWARD
  #define FPC_TONEAREST FE_TONEAREST
  #define FPC_TOWARDZERO FE_TOWARDZERO
  #define FPC_UPWARD  FE_UPWARD
#endif  // _COMPILER_ ?

/// @brief Type for floating points exception flags
#if(_COMPILER_MSVC)
  typedef unsigned int FPCexcept_t;
#elif(_COMPILER_GCC)
  typedef fexcept_t FPCexcept_t;
#endif  // _COMPILER_ ?

/// @brief Clear any pending exception
int FPCfeclearexcept(void) {
#if(_COMPILER_MSVC)
  return _clearfp();
#elif(_COMPILER_GCC)
  return feclearexcept(FPC_ALL_EXCEPT));
#endif  // _COMPILER_ ?
}

/// @brief Retrieve all exception flags
///
/// @param[in,out]  flagp   Pointer to where the flags will be stored
int FPCfegetexceptflag(FPCexcept_t* flagp) {
#if(_COMPILER_MSVC)
  *flagp = _controlfp(0, 0) & _MCW_EM;
  return 0;
#elif(_COMPILER_GCC)
  return fegetexceptflag(flagp, FE_ALL_EXCEPT);
#endif  // _COMPILER_ ?
}

/// @brief Set specified exception flags
///
/// @param[in]  flagp   Pointer to where the flags to set will be read from
/// @param[in]  excepts   Bitmask of the flags to be set
int FPCfesetexceptflag(const FPCexcept_t* flagp, int excepts) {
#if(_COMPILER_MSVC)
  return _controlfp(excepts, _MCW_EM);
#elif(_COMPILER_GCC)
  fesetexceptflag(flagp, excepts));
#endif  // _COMPILER_ ?
}

/// @brief Retrieve rounding mode
///
/// @return the rounding mode.
/// @see FE_DOWNWARD, FE_TONEAREST, FE_TOWARDZERO, FE_UPWARD
int FPCfegetround(void) {
#if(_COMPILER_MSVC)
  return _controlfp(0, 0) & _MCW_RC;
#elif(_COMPILER_GCC)
  return fegetround();
#endif  // _COMPILER_ ?
}

/// @brief Set rounding mode
///
/// @param[in]  mode    Mode to set the FP environment to
/// @see FE_DOWNWARD, FE_TONEAREST, FE_TOWARDZERO, FE_UPWARD
int FPCfesetround(int round) {
#if(_COMPILER_MSVC)
  return _controlfp(round, _MCW_RC);
#elif(_COMPILER_GCC)
  fesetround(round));
#endif  // _COMPILER_ ?
}

/// @brief Type for previous floating points environnement
#if(_COMPILER_MSVC)
  typedef unsigned int FPCenv_t;
#elif(_COMPILER_GCC)
  typedef fenv_t FPCenv_t;
#endif  // _COMPILER_ ?

/// @brief Static variable holding the previous floating-point environment
static FPCenv_t FPCprevious_env;

/// @brief Flush all denormals to zero
///
/// On Msvc this will be done for both x87 and SSE; SSE only for gcc/Clang
///
/// On gcc/Clang this can only be done by setting the whole FP environment
/// to default! That's why this environment has to be saved first.
///
/// For the same reason, do all other changes AFTER calling this function.
void FPCSetDenormalsFTZ(void) {
#if(_COMPILER_MSVC)
  // Save previous environment
  FPCprevious_env = _controlfp(0, 0);
  // Activate FTZ
  _controlfp_s(0, _DN_FLUSH, _MCW_DN);
#elif(_COMPILER_GCC)
  // Save previous environment
  fegetenv(&FPCprevious_env);
  // Set FP environment to default and activate FTZ and DAZ
  fesetenv(FE_DFL_DISABLE_SSE_DENORMS_ENV);
#endif  // _COMPILER_ ?
}

/// @brief Set back configuration regarding denormals to its previous state
///
/// On gcc/Clang this rely on the previously saved environment.
/// Note also that on these compiler the whole environment (not only denormals)
/// will be reset.
void FPCResetDenormals(void) {
#if(_COMPILER_MSVC)
  _controlfp(FPCprevious_env, _MCW_DN);
#elif(_COMPILER_GCC)
  fesetenv(&FPCprevious_env);
#endif  // _COMPILER_ ?
}

/// @brief Function testing whether or no a value is a denormal
/// Single-precision floating point values only.
///
/// @param[in]  value   Value to test
///
/// @return true if the value is a denormal
bool FPCIsDenormal(const float value) {
#if(_COMPILER_MSVC)
  // _fpclass() takes a double so we have to do this by hand
  union Number32b {
    int i;
    float f;
  } value_;
  value_.f = value;
  // 0x00800000: integer representation of the smallest normalized float
  return value_.i < 0x00800000;
#elif(_COMPILER_GCC)
  return FP_SUBNORMAL == fpclassify(value);
#endif  // _COMPILER_ ?
}

#if (_COMPILER_MSVC)
#pragma warning(pop)
#endif  // (_COMPILER_MSVC)

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // FPCONTROL_INC_FPCONTROL_H_
