TEST_MAIN_FOLDER_DIR = ./
TEST_PROGRAM_DIR = ../
TEST_SUPPORT_DIR = support/
TEST_MOCK_DIR = mock_interface/

TESTS_DRONE_CAN_SERVICE_DIR = tests_DroneCAN_service/

BUILD_DEPENDENCIES_DIR = ../dependencies/
BUILD_DSDLC_GENERATED = ../dependencies/dsdlc_generated/include
BUILD_UAVCAN_DIR = ../dependencies/dsdlc_generated/include/uavcan
BUILD_CAN_DRIVER_DIR = ../dependencies/arduino-CAN/src/

CPPUTEST_HOME = cpputest

ON_TEST_FILES = $(TEST_PROGRAM_DIR)DroneCAN_service.cpp
TEST_FILES = $(filter %.cpp, $(wildcard $(TESTS_DRONE_CAN_SERVICE_DIR)*))
MOCK_FILES = $(filter %.cpp, $(wildcard $(TEST_MOCK_DIR)*))

COMPILER_INCLUDE_FLAGS = -I$(TEST_MAIN_FOLDER_DIR) -I$(TEST_SUPPORT_DIR)
COMPILER_INCLUDE_FLAGS += -I$(TEST_MOCK_DIR) -I$(TEST_PROGRAM_DIR)
COMPILER_INCLUDE_FLAGS += -I$(BUILD_DEPENDENCIES_DIR) -I$(BUILD_DSDLC_GENERATED) 
COMPILER_INCLUDE_FLAGS += -I$(BUILD_CAN_DRIVER_DIR) -I$(BUILD_UAVCAN_DIR)
COMPILER_INCLUDE_FLAGS += -I$(CPPUTEST_HOME)/include

CXX = g++
CXXFLAGS  =  -Wall $(COMPILER_INCLUDE_FLAGS)

##UNCOMMENT TO TEST MEMORY LEAK
#CXXFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorNewMacros.h

LD_LIBRARIES  = -L$(CPPUTEST_HOME)/lib -lCppUTest -lCppUTestExt

run: tests
	./tests.out
	
tests: *.cpp $(ON_TEST_FILES) $(MOCK_FILES) $(TEST_FILES)
	$(CXX) $(CXXFLAGS) $^ $(LD_LIBRARIES) -o tests.out

#print:
#	echo $(CAN_DRIVER_FILES)