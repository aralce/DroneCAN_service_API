# HOW DO I CONFIGURE THE SYSTEM THE FIRST TIME?

-Please, check the folder cpputest has the cpputest repository. If the cpputest is empty, then clone this [repository](https://github.com/aralce/cpputest) on this folder.  
  
-If It is the first time you set up the test enviroment run the followings commands opening a terminal on this folder (LINUX ONLY)  
```
$ cd cpputest  
$ autoreconf . -i  
$ ./configure  
$ make tdd  
$ export CPPUTEST_HOME=$(pwd)  
```
***NOTE:*** The commands could you to have installed GNU autotools. In this case run `$ apt-get install automake autoconf libtool`

# HOW DO I RUN THE TESTS?
- If the system is configured, then run the following command on your terminal `$ make`. You have to run the command from this folder.

# HOW IS THIS FOLDER STRUCTURED?

## cpputest  
Repository of the test tool.
  
## mock_interface  
Has the mock files.
  
## support  
It has useful functions and classes for tests.
   
## makefile  
Have the recipe to run the tests.

# Important
- The system was develop under linux and the instructions target a linux system.

-----------------------------------------------------------------------------------------
# [BACK](https://github.com/aralce/dronecan_with_libcanard/blob/develop/README.md)
