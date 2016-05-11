## Testing
Running the Catch unit tests is built into CMake and they can be run using the **test** make target after building the project in the build directory:
```
make test
```
The CTest program can also be used to run the tests:
```
ctest
```
Using CTest gives the ability for additional options.  The tests can be run multithreaded using the **-j** with the number of desired cores (example for 4 threads):
```
ctest -j4
```
This will run the tests faster, which is more important as the number of tests increase.  A subset of the tests can be run by using the **-R** option with a regular expression (quotes are necessary so that the shell does not try to do expansion of the expression):
```
ctest -R "parser*"
```
Will run all the tests that start with **parser**.  Both of these options can be combined.  CTest has many more options (see **ctest --help**).  This also works on Windows in either the MinGW Shell or the DOS Prompt.

#### Catch
If CTest reports a failing test, the Catch unit test program can be run to identify the failing test(s), which will give the line number(s) of the test(s) that failed.  The Catch unit test program also has several options.  Use the **-l** option to see the list of tests:
```
unittest -l
```
Using the **-t** option will give an appreviated list of just the test tag names.  A specific test can by tag name by specifying the tag name on the command line:
```
unittest "[tagname]"
```
Note that the quotes are not strictly required, but for some reason some tag names don't work without the quotes (possibly interference with brackets on bash).  Use the **-s** option to output all of the tests including the passing and failing tests (useful for seeing that all of the tests are running):
```
unittest -s
```
There are more options, use the **-h** option for a complete list of options.

---

[Home](../README.md)
