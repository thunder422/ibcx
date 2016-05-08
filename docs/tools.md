## Tools
The primary development platform is Linux.  The list of tools being used for IBCX will be listed here:
* GCC 4.9
  * Compiler suite
  * Version 4.8 will be acceptable
* CMake 2.8.12.2
  * Cross platform build generator
  * Will also be tested on version 3.5.2
* git 2.8.2
  * Software revision control
  * Repository hosted on [GitHub](https://github.com/thunder422/ibcx)
  * Older versions should work (1.7+)
* Linux Mint 13 LTS
  * Equivalent to Ubunutu 12.04 LTS
  * With [toolchains repostory](http://ppa.launchpad.net/ubuntu-toolchain-r/test/ubuntu/) for newer versions of GCC
  * Will also be tested on Linux Mint 17.3 LTS (equivalent to Ubuntu 14.04.4 LTS)
  * Windows will be added soon, version(s) to be determined
* VIM 7.4.1816 (for editing)
  * The best source code and general text editor ;-)
  * Any text editor can be used
* Qt Cross Platform GUI Toolkit
  * GUI development not currently planned (distance future)
* Qt Creator 4.0.0-RC1 IDE
  * Though not for Qt, still a good general Integrated Development Environment
  * For editing (using the included FakeVIM mode)
  * Good support for CMake
  * GUI frontend to the GDB command line debugger
* GCOV 4.9
  * Code coverage tool, part of the GCC compiler suite
  * Separate LCOV (1.12) for report generation (with GenHTML)
