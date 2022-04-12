PROG_DIR = ../
SW_LIB = ../../sw_lib
SUPPORT_DIR = support/
MOCK_DIR = mock_interface/
DSDL_DIR = ../dsdlc_generated/
CPPUTEST_HOME = cpputest

FILES_TO_TEST = DroneCan_service_singleton.cpp

CXX = g++
CXXFLAGS  = -Wall -I$(SUPPORT_DIR) -I$(MOCK_DIR) 
CXXFLAGS += -I$(SW_LIB) -I$(PROG_DIR) -I$(DSDL_DIR) #The order is important MOCK_DIR replace the .h with the same name on PROG_DIR and SW_LIB
CXXFLAGS += -I$(CPPUTEST_HOME)/include


##UNCOMMENT TO TEST MEMORY LEAK
#CXXFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorNewMacros.h

LD_LIBRARIES  = -L$(CPPUTEST_HOME)/lib -lCppUTest -lCppUTestExt

run: tests
	./tests.out
	
tests: *.cpp $(PROG_DIR)$(FILES_TO_TEST)
	$(CXX) $(CXXFLAGS) $^ $(LD_LIBRARIES) -o tests.out	
