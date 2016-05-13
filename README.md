# IBCX
Interactive BASIC Compiler - Experimental


## Purpose
The Interactive BASIC Compiler Project ([IBCP](../ibcp)) code has gotten rather complicated.  The code probably should be cleaned up (refactored).  Also, while there are tests for the IBCP code, the test code is built into the program and activated with special command line options, test input files and expected test results files.  This is not ideal and is problematic on Windows, and this is not a good substitute for proper unit tests.

This repository is a test bed for research of some new ideas for the conversion of BASIC code into the internal code.  The Test Driven Development (TDD) methodology will be used, which consists of these three simple steps:

1. Develop a test for a new feature (which will initially fail as the feature is not yet implement).
2. Implement enough code to make the test pass.
3. Clean up (refactor) the code (using the passing test to validate the changes).

These steps are repeated as development progresses.  This repository will still utilize C++11 and the C++ Standard Library (STL).

The initial intention of this repository is to implement a series of classes that will do the underlying work of the interactive BASIC compiler.  There will be no executable for a while beyond the unit tests to exercise these classes.

## Testing
The [Catch](https://github.com/philsquared/Catch) unit test platform is being used for developing unit tests.  Catch is easy to use and only consists of a single header file.  The unit test programs are created under CMake and are run with CTest.  Coverage tools are being utilized to insure that 100% of the code is being tested.  This will insure that what has been implemented stays working correctly.  The unit tests will also provide _active_ documentation.

## Notes
There is no blog associated with this project.  Details of changes will be put into the commit messages of the repository.  Releases of the code will not be posted as 'official' releases as IBCP was to sourceforge - the code will only be available in this repository, and releases will be made by tags only.

The git master/develop/topic branching model used for IBCP will also be used here.  Additional status and details about tool setup, building, testing etc. can be found in the [wiki](wiki) for this repository.
