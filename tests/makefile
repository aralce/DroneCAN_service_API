TEST_MAIN_FOLDER_DIR = ./
TEST_PROGRAM_DIR = ../
TEST_SUPPORT_DIR = support/
TEST_MOCK_DIR = mock_interface/
TEST_MOCK_DSDL_MESSAGES_DIR = $(TEST_MOCK_DIR)/folder_DSDL_messages/

TESTS_DRONE_CAN_SERVICE_DIR = tests_DroneCAN_service/
TESTS_PUBLISH_MESSAGE_DIR = $(TESTS_DRONE_CAN_SERVICE_DIR)/tests_publish_message/
TESTS_PUBLISH_DIFFERENT_MESSAGES_DIR = $(TESTS_PUBLISH_MESSAGE_DIR)/tests_different_messages/
TESTS_PUBLISH_MESSAGE_COMMON_DIR = $(TESTS_PUBLISH_MESSAGE_DIR)/common_to_publish_message/

BUILD_DEPENDENCIES_DIR = ../dependencies/
BUILD_DSDLC_GENERATED = ../dependencies/dsdlc_generated/include
BUILD_UAVCAN_DIR = ../dependencies/dsdlc_generated/include/uavcan
BUILD_CAN_DRIVER_DIR = ../dependencies/arduino-CAN/src/

CPPUTEST_HOME = cpputest

ON_TEST_FILES = $(TEST_PROGRAM_DIR)DroneCAN_service_API.cpp
TEST_FILES = $(filter %.cpp, $(wildcard $(TESTS_DRONE_CAN_SERVICE_DIR)*))
TEST_FILES += $(filter %.cpp, $(wildcard $(TESTS_PUBLISH_MESSAGE_DIR)*))
TEST_FILES += $(filter %.cpp, $(wildcard $(TESTS_PUBLISH_DIFFERENT_MESSAGES_DIR)*))
TEST_FILES += $(filter %.cpp, $(wildcard $(TESTS_PUBLISH_MESSAGE_COMMON_DIR)*))
MOCK_FILES = $(filter %.cpp, $(wildcard $(TEST_MOCK_DIR)*))
MOCK_FILES += $(filter %.cpp, $(wildcard $(TEST_MOCK_DSDL_MESSAGES_DIR)*))

BUILD_FILES = $(filter %.cpp, $(wildcard $(BUILD_DEPENDENCIES_DIR)*))

COMPILER_INCLUDE_FLAGS = -I$(TEST_MAIN_FOLDER_DIR) -I$(TEST_SUPPORT_DIR)
COMPILER_INCLUDE_FLAGS += -I$(TEST_MOCK_DIR) -I$(TEST_MOCK_DSDL_MESSAGES_DIR) 
COMPILER_INCLUDE_FLAGS += -I$(TEST_PROGRAM_DIR)
COMPILER_INCLUDE_FLAGS += -I$(BUILD_DEPENDENCIES_DIR) -I$(BUILD_DSDLC_GENERATED) 
COMPILER_INCLUDE_FLAGS += -I$(BUILD_CAN_DRIVER_DIR) -I$(BUILD_UAVCAN_DIR)
COMPILER_INCLUDE_FLAGS += -I$(CPPUTEST_HOME)/include

FLAG_FOR_COVERAGE = --coverage

CXX = g++
CXXFLAGS  =  -Wall $(COMPILER_INCLUDE_FLAGS)

##UNCOMMENT TO TEST MEMORY LEAK
#CXXFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorNewMacros.h

LD_LIBRARIES  = -L$(CPPUTEST_HOME)/lib -lCppUTest -lCppUTestExt

BUILD_OUTPUT_DIR = build

all: create_build_folder compile_tests
	build/tests.out

coverage: create_build_folder create_coverage_folder compile_tests_for_coverage
	$(BUILD_OUTPUT_DIR)/tests.out
	lcov --capture --directory build --output-file coverage/coverage.info
	genhtml coverage/coverage.info --output-directory coverage/html_report
	
create_build_folder:
	mkdir -p build

create_coverage_folder:
	mkdir -p coverage

compile_tests: *.cpp $(ON_TEST_FILES) $(MOCK_FILES) $(TEST_FILES) $(BUILD_FILES) 
	$(CXX) $(CXXFLAGS) $^ $(LD_LIBRARIES) -o $(BUILD_OUTPUT_DIR)/tests.out

compile_tests_for_coverage: *.cpp $(ON_TEST_FILES) $(MOCK_FILES) $(TEST_FILES) $(BUILD_FILES)
	$(CXX) $(FLAG_FOR_COVERAGE) $(CXXFLAGS) $^ $(LD_LIBRARIES) -o $(BUILD_OUTPUT_DIR)/tests.out
