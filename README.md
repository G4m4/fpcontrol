FPControl - Floating point control
==================================

FPControl is a tiny piece of code for floating point environment control and especially denormals management.

C99 standardizes the fenv.h header, with a few utilities related to floating point environment, but this file is not available with Microsoft VC++.

This is a (very) short attempt to offer platform-independent for the following:
- floating-point exceptions control
- rounding mode selection
- denormals flush to zero (SSE only for both gcc and Clang, SSE + x87 with MSVC)
- Denormal detection

The entire code consists of one header file which is self-sufficient.
However this repository also contains a few tests reyling on [Google Test Framework](http://code.google.com/p/googletest/).

Building the tests
------------------

Building is done using Cmake.
All required Cmake variables are set up, the only thing to do is make the environment variable GTEST_ROOT point to GTest sources.
On Linux, this can be done as follows:

    sudo apt-get install libgtest-dev
    export GTEST_ROOT=/usr/src/gtest
    mkdir build && cd build
    cmake ..
    cmake --build .

License
==================================
FPControl is under GPLv3.

See the COPYING file for the full license text.
