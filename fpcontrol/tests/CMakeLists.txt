# @brief Build FPControl tests executable

# preventing warnings from external source files
include_directories(
  SYSTEM
  ${GTEST_INCLUDE_DIRS}
)

include_directories(
  ${FPCONTROL_INCLUDE_DIR}
)

# Source files
set(FPCONTROL_TESTS_SRC
    main.cc
    fpcontrol.cc
    ${FPCONTROL_INCLUDE_DIR}/fpcontrol/inc/fpcontrol.h # So it does appear in generated files
)

# Target
add_executable(fpcontrol_tests
  ${FPCONTROL_TESTS_SRC}
)

set_target_mt(fpcontrol_tests)

# Make sure the SSE unit gets used for fp maths - SSE3 allows DAZ so try using it
if(COMPILER_IS_GCC OR COMPILER_IS_CLANG)
  add_compiler_flags(fpcontrol_tests "-mfpmath=msse3")
else()
  add_compiler_flags(fpcontrol_tests "/ARCH:SSE")
  # On Windows SSE3 will simply be a project flag as there is no way to force the compiler
  add_definitions(-D SET_SSE_VERSION=3)
endif()

target_link_libraries(fpcontrol_tests
  gtest_main
)
