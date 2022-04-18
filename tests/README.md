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
It has useful functions and classes for tests.
  
test.cpp  
Has the group of tests  
  
makefile  
Have the recipe to run the tests.


# Important
- The system was develop under linux and the instructions target a linux system.
- To run the tests use the command "make"
