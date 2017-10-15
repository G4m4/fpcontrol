/// @file tests/fpcontrol.cc
/// @brief FPControl tests
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

#include <limits>

#include "gtest/gtest.h"

#include "fpcontrol/inc/fpcontrol.h"

/// brief Helper for automated array size retrieval
template <typename TypeArray, int N>
inline int GetArraySize(TypeArray (&) [N]) {
  return N;
}

/// @brief Helper for nasty float-to-int tricks without using pointers stuff
union Number32b {
  Number32b(float number) : f(number) {}
  Number32b(int number) : i(number) {}

  int i;
  float f;
};

/// @brief Get a random denormal
float GetDenormal() {
  // math.h pulled by fpcontrol.h already
  static const Number32b base(rand());
  // Integer representation for the biggest normalised float
  const Number32b mod(base.i % 0x00800000);
  return mod.f;
}

/// @brief Check the IsDenormal function on all denormals:
/// e.g. those in ] 0.0f ; FLT_MIN [
/// Also check it on the first non-denormal (FLT_MIN)
TEST(Denormals, IsDenormal) {
  const Number32b zero(0);
  EXPECT_FALSE(FPCIsDenormal(zero.f));
  Number32b checked_num(1);
  while (checked_num.f < std::numeric_limits<float>::min()) {
    EXPECT_TRUE(FPCIsDenormal(checked_num.f));
    checked_num.i += 1;
  }
  EXPECT_FALSE(FPCIsDenormal(checked_num.f));
}

/// @brief Set/Get denormals flush to zero
TEST(Denormals, DenormalsFTZ) {
  FPCenv_t fp_env;
  FPCSaveEnv(&fp_env);
  Number32b denormal(GetDenormal());
  EXPECT_TRUE(FPCIsDenormal(denormal.f));
  EXPECT_NE(0.0f, denormal.f);
  FPCSetDenormalsFTZ_SSE();
  const float add_ftz = denormal.f + denormal.f;
  EXPECT_EQ(0.0f, add_ftz);
  EXPECT_FALSE(FPCIsDenormal(add_ftz));

  FPCLoadEnv(&fp_env);
  // No operation happened on this variable in between
  EXPECT_FALSE(FPCIsDenormal(add_ftz));
  const float add_noftz = denormal.f + denormal.f + denormal.f;
  EXPECT_TRUE(FPCIsDenormal(add_noftz));
  EXPECT_NE(0.0f, add_noftz);
}

#if _SSE_VERSION >= 3
/// @brief Set/Get denormals are zero
TEST(Denormals, DAZ) {
  FPCenv_t fp_env;
  FPCSaveEnv(&fp_env);
  Number32b denormal(GetDenormal());
  EXPECT_TRUE(FPCIsDenormal(denormal.f));
  EXPECT_NE(0.0f, denormal.f);
  FPCSetDenormalsDAZ_SSE();
  EXPECT_EQ(0.0f, denormal.f);
  // So the idea here is as follows:
  // the value is indeed denormal, but considered as zero when used as operand
  // As a result, it yields zeros when added to itself
  EXPECT_TRUE(FPCIsDenormal(denormal.f));
  EXPECT_EQ(0.0f, denormal.f + denormal.f);

  FPCLoadEnv(&fp_env);
  EXPECT_NE(0.0f, denormal.f);
  EXPECT_NE(0.0f, denormal.f + denormal.f + denormal.f);
  EXPECT_TRUE(FPCIsDenormal(denormal.f));
}
#endif  // _SSE_VERSION >= 3 ?

/// @brief Set/Get all exceptions flags, one by one
TEST(Exceptions, GetSetOneByOne) {
  unsigned int exception_flags[] = {FPC_INEXACT,
                                    FPC_UNDERFLOW,
                                    FPC_OVERFLOW,
                                    FPC_DIVBYZERO,
                                    FPC_INVALID};

  for (int current_flag_id(0);
       current_flag_id < GetArraySize(exception_flags);
       ++current_flag_id) {
    // All pending exceptions have to be cleared, just in case
    FPCClearExcept();

    const unsigned int excepts_before = FPCGetExcept();
    const unsigned int kCurrentException(exception_flags[current_flag_id]);

    FPCEnableExcept(kCurrentException);
    EXPECT_EQ(excepts_before ^ kCurrentException, FPCGetExcept());
    FPCDisableExcept(kCurrentException);
    EXPECT_EQ(excepts_before, FPCGetExcept());
  }
}
