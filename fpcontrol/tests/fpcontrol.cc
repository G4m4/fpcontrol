/// @file tests/fpcontrol.cc
/// @brief FPControl tests
/// @author gm
/// @copyright gm 2014
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
  Number32b denormal(1);
  EXPECT_TRUE(FPCIsDenormal(denormal.f + denormal.f));
  EXPECT_NE(0.0f, denormal.f + denormal.f);
  FPCSetDenormalsFTZ();
  const float add_ftz = denormal.f + denormal.f;
  EXPECT_EQ(0.0f, add_ftz);
  EXPECT_FALSE(FPCIsDenormal(add_ftz));

  FPCResetDenormals();
  // No operation happened on this variable in between
  EXPECT_FALSE(FPCIsDenormal(add_ftz));
  const float add_noftz = denormal.f + denormal.f;
  EXPECT_TRUE(FPCIsDenormal(add_noftz));
  EXPECT_NE(0.0f, add_noftz);
}

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

    unsigned int excepts_before = FPCGetExcept();
    const unsigned int kCurrentException(exception_flags[current_flag_id]);

    FPCEnableExcept(kCurrentException);
    EXPECT_EQ(excepts_before ^ kCurrentException, FPCGetExcept());
    FPCDisableExcept(kCurrentException);
    EXPECT_EQ(excepts_before, FPCGetExcept());
  }
}
