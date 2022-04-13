PROGRAM_DIR = ../
DEPENDENCIES_DIR = ../dependencies/
DSDLC_GENERATED = ../dependencies/dsdlc_generated/include
UAVCAN_DIR = ../dependencies/dsdlc_generated/include/uavcan
SUPPORT_DIR = support/

MOCK_DIR = mock_interface/
MOCK_FILES = $(filter %.cpp, $(notdir $(wildcard $(MOCK_DIR)*)))
CPPUTEST_HOME = cpputest
FILES_TO_TEST = DroneCan_service.cpp

CXX = g++
CXXFLAGS  = -I$(SUPPORT_DIR) -I$(MOCK_DIR) -Wall
CXXFLAGS += -I$(DEPENDENCIES_DIR) -I$(PROGRAM_DIR) #The order is important MOCK_DIR replace the .h with the same name on PROG_DIR and SW_LIB
CXXFLAGS += -I$(DSDLC_GENERATED) -I$(UAVCAN_DIR)
CXXFLAGS += -I$(CPPUTEST_HOME)/include


##UNCOMMENT TO TEST MEMORY LEAK
#CXXFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorNewMacros.h

LD_LIBRARIES  = -L$(CPPUTEST_HOME)/lib -lCppUTest -lCppUTestExt

run: tests
	./tests.out
	
tests: *.cpp $(PROGRAM_DIR)$(FILES_TO_TEST) $(MOCK_DIR)$(MOCK_FILES)
	$(CXX) $(CXXFLAGS) $^ $(LD_LIBRARIES) -o tests.out	