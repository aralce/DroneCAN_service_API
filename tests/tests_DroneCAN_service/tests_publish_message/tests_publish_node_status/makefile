#This file runs the tests on this folder
TEST_ROOT_FOLDER = ../../../
TEST_PROGRAM_DIR = $(TEST_ROOT_FOLDER)../
TEST_MAIN_FOLDER_DIR = ./
TEST_SUPPORT_DIR = $(TEST_ROOT_FOLDER)support/
TEST_MOCK_DIR = $(TEST_ROOT_FOLDER)mock_interface/
TEST_MOCK_DSDL_MESSAGES_DIR = $(TEST_MOCK_DIR)folder_DSDL_messages/

CPPUTEST_HOME = $(TEST_ROOT_FOLDER)cpputest

ON_TEST_FILES = $(TEST_PROGRAM_DIR)DroneCAN_service_API.cpp
TEST_FILES = $(filter %.cpp, $(wildcard $(TEST_MAIN_FOLDER_DIR)*))

MOCK_FILES = $(filter %.cpp, $(wildcard $(TEST_MOCK_DIR)*))
MOCK_FILES += $(filter %.cpp, $(wildcard $(TEST_MOCK_DSDL_MESSAGES_DIR)*))

BUILD_FILES = $(filter %.cpp, $(wildcard $(TEST_PROGRAM_DIR)*))

INCLUDE_DIR = $(TEST_ROOT_FOLDER)
INCLUDE_DIR += $(TEST_SUPPORT_DIR)
INCLUDE_DIR += $(TEST_MOCK_DIR)
INCLUDE_DIR += $(TEST_MOCK_DSDL_MESSAGES_DIR) 
INCLUDE_DIR += $(TEST_PROGRAM_DIR)
INCLUDE_DIR += $(CPPUTEST_HOME)/include
COMPILER_INCLUDE_FLAGS = $(addprefix -I, $(INCLUDE_DIR))

FLAG_FOR_COVERAGE = --coverage
FLAG_FOR_DEFINE = -D IS_RUNNING_TESTS

CXX = g++
CXXFLAGS  =  -Wall $(COMPILER_INCLUDE_FLAGS) $(FLAG_FOR_DEFINE)

##UNCOMMENT TO TEST MEMORY LEAK
#CXXFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorNewMacros.h

LD_LIBRARIES  = -L$(CPPUTEST_HOME)/lib -lCppUTest -lCppUTestExt

BUILD_OUTPUT_DIR = build/
COVERAGE_OUTPUT_DIR = coverage/

all: create_build_folder compile_tests
	@echo "-- Publish_node_status"
	@$(BUILD_OUTPUT_DIR)tests.out

clean:
	@rm -f -r $(BUILD_OUTPUT_DIR)

create_build_folder:
	@mkdir -p $(BUILD_OUTPUT_DIR)

TEST_MOCK_DSDL_OBJECTS = $(addsuffix .o, $(basename $(notdir $(filter %.cpp, $(wildcard $(TEST_MOCK_DSDL_MESSAGES_DIR)*)))))

OBJECTS_TO_LINK = $(addprefix $(BUILD_OUTPUT_DIR), $(TEST_MOCK_DSDL_OBJECTS))

compile_tests: $(ON_TEST_FILES) $(MOCK_FILES) $(TEST_FILES) $(BUILD_FILES)
	@echo "System is compiling files, please wait."
	$(CXX) $(CXXFLAGS) $^ $(LD_LIBRARIES) -o $(BUILD_OUTPUT_DIR)tests.out

INCLUDE_FILES = $(sort $(filter %.h, $(shell find $(INCLUDE_DIR))))
LIST_OF_MODIFIED_TIMES = $(foreach files, $(INCLUDE_FILES), $(shell date +%s -r $(files)))
LAST_LIBRARY_MODIFIED_TIME = $(lastword $(sort $(LIST_OF_MODIFIED_TIMES)))

#COMPILES FILES_ON_DSDL_MESSAGE_DIR
$(BUILD_OUTPUT_DIR)%.o : $(TEST_MOCK_DSDL_MESSAGES_DIR)%.cpp
#compile everything if some file on the include directory was modified
	@if [ $(LAST_LIBRARY_MODIFIED_TIME) -gt $(shell date +%s -r $(ON_TEST_FILES)) ]; then  \
		$(CXX) $(CXXFLAGS) $(LD_LIBRARIES) $^ -c -o $@; \
	else \
		$(CXX) $(CXXFLAGS) $(LD_LIBRARIES) $? -c -o $@; \
	fi;

print:
#	@ls $(CPPUTEST_HOME)
	echo $(INCLUDE_FILES)
	@echo LIBRARY $(LAST_LIBRARY_MODIFIED_TIME)
	@echo FILE $(shell date +%s -r $(ON_TEST_FILES))
	@if [ $(LAST_LIBRARY_MODIFIED_TIME) -gt $(shell date +%s -r $(ON_TEST_FILES)) ]; then  \
		echo 1; \
	else \
		echo 2; \
	fi;
#	@echo $(shell test $(LAST_LIBRARY_MODIFIED_TIME) -gt $(shell date +%s -r $(ON_TEST_FILES)))
#ifeq ($(shell test $(LAST_LIBRARY_MODIFIED_TIME) -gt $(shell date +%s -r $(ON_TEST_FILES)); echo $?), 1)
#	echo 4
#else
#	echo 2
#	@$(CXX) $(CXXFLAGS) $(LD_LIBRARIES) $? -c -o $@
#endif