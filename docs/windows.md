## Windows
Full details coming soon, this page will list details for building and testing under Windows.

#### Building on Windows XP:
The following procedures were testing using CMake 2.8.9, git 1.7.11 and MinGW with GCC 4.8.1.  First, these programs need to be found by the command line shells, therefore, the following should be **added** to the PATH environment variable (My Computer, Properties, Advanced, Environment Variables under System Variables):

```
;C:\Program Files\CMake 2.8\bin;C:\Program Files\Git\bin;C:\MinGW\bin
```
Do not disturb what is already there (a different version of CMake will have a different path).

###### From MinGW Shell:
Proceed the same as for Linux except the specific make file generator needs to be selected when running CMake:
```
cmake -G "MSYS Makefiles" ..
```
Continue building the same as for Linux.

###### From DOS Prompt:
Proceed the same as for Linux expect the specific make file generator needs to be selected when running CMake:
```
cmake -G "MSYS Makefiles" ..
```
However, for some reason this generates a bunch of errors the first time, but running it a second time completes without further problems.  The MinGW specific make program needs to be run instead of plain **make** to build:
```
mingw32-make
```

---

[Home](../README.md)
