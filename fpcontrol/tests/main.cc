/// @filename tests/main.cc
/// @brief Tests main entry point
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

#include "gtest/gtest.h"

/// @brief Main function, of course.
int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
