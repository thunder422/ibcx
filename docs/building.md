## Building
It is recommending that building be done in a separate build directory.  In source directory builds are permitted with CMake but this is dangerous because if clutters up the source directories and the generated builds may accidentally be committed to the repository.  Therefore the CMakeLists.txt file for with project prevents this.  However, it is permitted to build into a sub-directory.  Linux (bash) instructions are here (see the [Windows](windows.md) for Windows specific instructions):

Create a build directory:
   ```
   cd ibcx
   mkdir build
   cd build
   ```
Generate the make files with CMake:
   ```
   cmake ..
   ```
Build the project:
   ```
   make
   ```
See [Testing](testing.md) for details on running the tests.

---

[Home](../README.md)
