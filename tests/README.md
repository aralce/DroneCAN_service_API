Unit tests wiht Cpputest
Setting up CppUTest:
$ cd cpputest
$ autoreconf . -i
$ ./configure
$ make tdd
$ export CPPUTEST_HOME=$(pwd)

*Building with autoconf requires you to (this requires you to have installed GNU autotools, apt-get install automake autoconf libtool)

Then to run the tests just use: $ make

Structure of code
cpputest
Test Harness folder

mock_interface
Has the interface for files to mock on test

support
Has dummy files to fill dependencies

test.cpp
Has the group of tests

makefile
The production files to test have to be added on FILES_TO_TEST