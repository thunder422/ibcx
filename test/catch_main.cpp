// tell CATCH to generate the main() function for a unit test program
//
// this is a separate file because it takes a while to compile, so to speed up
// development of the test program, it is a separate source file so that it
// only needs to be compiled once

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
