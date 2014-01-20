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
)

# Target
add_executable(fpcontrol_tests
  ${FPCONTROL_TESTS_SRC}
)

set_target_mt(fpcontrol_tests)

target_link_libraries(fpcontrol_tests
  gtest_main
)
