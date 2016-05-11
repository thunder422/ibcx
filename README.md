# IBCX
Interactive BASIC Complier - Experimental


## Purpose
The Interative BASIC Compiler Project (IBCP) code has gotten rather complicated.  The code probably should be cleaned up (refactored).  Also, while there are tests for the IBCP code, the test code is built into the program and activated with special command line options, test input files and expected test results files.  This is not ideal and is problematic on Windows, and this is not a good substitute for proper unit tests.

This repository is a test bed to research for a new ideas for the translation of BASIC code into the internal code.  The Test Driven Development (TDD) methodology will be used, which consists of these three simple steps:

1. Develop a test for a new feature (which will initially fail as the feature is not yet implement).
2. Implement enough code to make the test pass.
3. Clean up (refactor) the code (using the passing test to validate the changes).

These steps are repeated as development progresses.  This project will still utilize C++11 and the C++ Standard Library (STL).

## Testing
The [Catch](https://github.com/philsquared/Catch) unit test platform will be used for developing the tests.  Catch is easy to use and only consists of a single header file.  The programs will be created under CMake and run with CTest.

## Notes
There will be no blog associated with this project.  Sufficient details of changes will be put into the commit messages of the repository.  Nor will the code be posted (as 'official' release as IBCP was to sourceforge) - the code will only be available in this repository, and releases will only be made by tags.  The git master/develop/topic branching model used for the IBCP will also be used here.

Additional details about tool setup, building, testing etc. will be included in the repository in the [docs](docs) folder.

## Status
This section will contain the current status of the project.  Currently the project is being setup.

## Documentation
* [Tools](docs/tools.md) - list of tools being used
* [Building](docs/building.md) - details for building
* [Testing](docs/testing.md) - details for testing
* [Windows](docs/windows.md) - details for development on Windows
