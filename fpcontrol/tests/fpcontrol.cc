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
TEST(FPControl, IsDenormal) {
  Number32b checked_num(1);
  while (checked_num.f < std::numeric_limits<float>::min()) {
    EXPECT_TRUE(FPCIsDenormal(checked_num.f));
    checked_num.i += 1;
  }
  EXPECT_FALSE(FPCIsDenormal(checked_num.f));
}

/// @brief Set/Get exceptions flags
TEST(FPControl, ExceptionsFlags) {
  FPCexcept_t initial_flags;
  FPCexcept_t new_flags;
  FPCfegetexceptflag(&initial_flags);

  // All pending exceptions have to be cleared, just in case
  FPCfeclearexcept();

  new_flags = ~initial_flags;
  FPCfesetexceptflag(&new_flags, new_flags);
  FPCfegetexceptflag(&new_flags);
  ASSERT_NE(initial_flags, new_flags);

  FPCfesetexceptflag(&initial_flags, FPC_ALL_EXCEPT);
  FPCfegetexceptflag(&new_flags);
  ASSERT_EQ(initial_flags, new_flags);
}

/// @brief Set/Get denormals flush to zero
TEST(FPControl, DenormalsFTZ) {
  Number32b denormal(1);
  EXPECT_TRUE(FPCIsDenormal(denormal.f + denormal.f));
  EXPECT_NE(0.0f, denormal.f + denormal.f);
  FPCSetDenormalsFTZ();

  EXPECT_EQ(0.0f, denormal.f + denormal.f);

  FPCResetDenormals();
  EXPECT_TRUE(FPCIsDenormal(denormal.f + denormal.f));
  EXPECT_NE(0.0f, denormal.f + denormal.f);
}
