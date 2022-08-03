TEST_MAIN_FOLDER_DIR = ../../
TEST_PROGRAM_DIR = $(TEST_MAIN_FOLDER_DIR)../
TEST_SUPPORT_DIR = $(TEST_MAIN_FOLDER_DIR)support/
TEST_MOCK_DIR = $(TEST_MAIN_FOLDER_DIR)mock_interface/
TEST_MOCK_DSDL_MESSAGES_DIR = $(TEST_MOCK_DIR)/folder_DSDL_messages/

CPPUTEST_HOME = $(TEST_MAIN_FOLDER_DIR)cpputest

ON_TEST_FILES = $(TEST_PROGRAM_DIR)DroneCAN_service_API.cpp
TEST_FILES = $(filter %.cpp, $(wildcard ./*))
TEST_FILES += $(filter %.cpp, $(wildcard $(TEST_MAIN_FOLDER_DIR)*))

MOCK_FILES = $(filter %.cpp, $(wildcard $(TEST_MOCK_DIR)*))
MOCK_FILES += $(filter %.cpp, $(wildcard $(TEST_MOCK_DSDL_MESSAGES_DIR)*))

BUILD_FILES = $(filter %.cpp, $(wildcard $(TEST_PROGRAM_DIR)*))

COMPILER_INCLUDE_FLAGS = -I$(TEST_MAIN_FOLDER_DIR) -I$(TEST_SUPPORT_DIR)
COMPILER_INCLUDE_FLAGS += -I$(TEST_MOCK_DIR) -I$(TEST_MOCK_DSDL_MESSAGES_DIR) 
COMPILER_INCLUDE_FLAGS += -I$(TEST_PROGRAM_DIR)
COMPILER_INCLUDE_FLAGS += -I$(CPPUTEST_HOME)/include

FLAG_FOR_DEFINE = -D IS_RUNNING_TESTS

CXX = g++
CXXFLAGS  =  -Wall $(COMPILER_INCLUDE_FLAGS) $(FLAG_FOR_DEFINE)

##UNCOMMENT TO TEST MEMORY LEAK
#CXXFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorNewMacros.h

LD_LIBRARIES  = -L$(CPPUTEST_HOME)/lib -lCppUTest -lCppUTestExt

BUILD_OUTPUT_DIR = build

all: create_build_folder compile_tests
	@echo "Running tests"
	@$(BUILD_OUTPUT_DIR)/tests.out

clean:
	@rm -f -r $(BUILD_OUTPUT_DIR)
	@rm -f -r $(COVERAGE_OUTPUT_DIR)
	@echo "Build folders were removed"

create_build_folder:
	@mkdir -p $(BUILD_OUTPUT_DIR)

create_coverage_folder:
	@mkdir -p $(COVERAGE_OUTPUT_DIR)

compile_tests: *.cpp $(ON_TEST_FILES) $(MOCK_FILES) $(TEST_FILES) $(BUILD_FILES) 
	@echo "System is compiling files, please wait."
	@$(CXX) $(CXXFLAGS) $^ $(LD_LIBRARIES) -o $(BUILD_OUTPUT_DIR)/tests.out

print:
	@echo $(TEST_FILES)
#	@ls $(TEST_PROGRAM_DIR)