FPControl - Floating point control
==================================

[![Build Status](https://travis-ci.org/G4m4/fpcontrol.svg?branch=master)](https://travis-ci.org/G4m4/fpcontrol)

FPControl is a C single-header library for floating point environment control and especially denormals management.

It aims at providing platform-independent for the following, on both x87 and SSE:
- floating-point exceptions control
- rounding mode selection
- denormals flush to zero
- Denormal detection

The entire code consists of one header file which is self-sufficient.
However this repository also contains a few tests relying on [Google Test Framework](http://code.google.com/p/googletest/).

Context
-------

C99 standardizes the fenv.h header, with a few utilities related to floating point environment, but this file is not available with Microsoft VC++.
Moreover, it provides different functionality on Linux or Apple machines (the latter offering a simple way to do denormals Flush-to-Zero).
Eventually, x87 and SSE units are differently handled on these 3 systems.

This library wraps them all into common functions and typedefs, working on all 3 platforms in both 32 and 64 bits.

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
FPControl is under a very permissive license.

See the COPYING file for the full license text.
