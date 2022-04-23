TEST_MAIN_FOLDER_DIR = ./
TEST_PROGRAM_DIR = ../
TEST_SUPPORT_DIR = support/
TEST_MOCK_DIR = mock_interface/
TEST_MOCK_DSDL_MESSAGES_DIR = $(TEST_MOCK_DIR)/folder_DSDL_messages/

TESTS_DRONE_CAN_SERVICE_DIR = tests_DroneCAN_service/
TESTS_PUBLISH_MESSAGE_DIR = $(TESTS_DRONE_CAN_SERVICE_DIR)/tests_publish_message/
TESTS_PUBLISH_MESSAGE_COMMON_DIR = $(TESTS_PUBLISH_MESSAGE_DIR)/common_to_publish_message/

BUILD_DEPENDENCIES_DIR = ../dependencies/
BUILD_DSDLC_GENERATED = ../dependencies/dsdlc_generated/include
BUILD_UAVCAN_DIR = ../dependencies/dsdlc_generated/include/uavcan
BUILD_CAN_DRIVER_DIR = ../dependencies/arduino-CAN/src/

CPPUTEST_HOME = cpputest

ON_TEST_FILES = $(TEST_PROGRAM_DIR)DroneCAN_service_API.cpp
TEST_FILES = $(filter %.cpp, $(wildcard $(TESTS_DRONE_CAN_SERVICE_DIR)*))
TEST_FILES += $(filter %.cpp, $(wildcard $(TESTS_PUBLISH_MESSAGE_DIR)*))
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
FLAG_FOR_DEFINE = -D IS_RUNNING_TESTS

CXX = g++
CXXFLAGS  =  -Wall $(COMPILER_INCLUDE_FLAGS) $(FLAG_FOR_DEFINE)

##UNCOMMENT TO TEST MEMORY LEAK
#CXXFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorNewMacros.h

LD_LIBRARIES  = -L$(CPPUTEST_HOME)/lib -lCppUTest -lCppUTestExt

BUILD_OUTPUT_DIR = build
COVERAGE_OUTPUT_DIR = coverage

all: create_build_folder compile_tests
	@echo "Running tests"
	@$(BUILD_OUTPUT_DIR)/tests.out

clean:
	@rm -f -r $(BUILD_OUTPUT_DIR)
	@rm -f -r $(COVERAGE_OUTPUT_DIR)
	@echo "Build folders were removed"

coverage: create_build_folder create_coverage_folder compile_tests_for_coverage
	@$(BUILD_OUTPUT_DIR)/tests.out
	@lcov --capture --directory $(BUILD_OUTPUT_DIR) --output-file $(COVERAGE_OUTPUT_DIR)/coverage.info
	@genhtml $(COVERAGE_OUTPUT_DIR)/coverage.info --output-directory $(COVERAGE_OUTPUT_DIR)/html_report
	@echo "Coverage report generated on: $(COVERAGE_OUTPUT_DIR)" 
	
create_build_folder:
	@mkdir -p $(BUILD_OUTPUT_DIR)

create_coverage_folder:
	@mkdir -p $(COVERAGE_OUTPUT_DIR)

compile_tests: *.cpp $(ON_TEST_FILES) $(MOCK_FILES) $(TEST_FILES) $(BUILD_FILES) 
	@echo "System is compiling files, please wait."
	@$(CXX) $(CXXFLAGS) $^ $(LD_LIBRARIES) -o $(BUILD_OUTPUT_DIR)/tests.out

compile_tests_for_coverage: *.cpp $(ON_TEST_FILES) $(MOCK_FILES) $(TEST_FILES) $(BUILD_FILES)
	@echo "System is compiling files for coverage report, please wait."
	@$(CXX) $(FLAG_FOR_COVERAGE) $(CXXFLAGS) $^ $(LD_LIBRARIES) -o $(BUILD_OUTPUT_DIR)/tests.out
