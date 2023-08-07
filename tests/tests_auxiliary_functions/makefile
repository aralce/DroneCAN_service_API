TEST_MAIN_FOLDER_DIR = ../
TEST_PROGRAM_DIR = $(TEST_MAIN_FOLDER_DIR)../
IMPLEMENTATION_DIR = $(TEST_PROGRAM_DIR)implementation/
UAVCAN_MESSAGES_DIR = $(IMPLEMENTATION_DIR)uavcan_driver/uavcan_messages/
TEST_MOCK_DIR = $(TEST_MAIN_FOLDER_DIR)mocks/
TEST_MOCK_CANARD_WRAPER_DIR = $(TEST_MOCK_DIR)Canard_wrapper/
TEST_MOCK_CANARD_DIR = $(TEST_MOCK_CANARD_WRAPER_DIR)Canard/
TEST_MOCK_DSDL_MESSAGES_DIR = $(TEST_MOCK_DIR)/folder_DSDL_messages/

CPPUTEST_HOME = $(TEST_MAIN_FOLDER_DIR)cpputest

INCLUDE_DIR = $(TEST_MAIN_FOLDER_DIR)
INCLUDE_DIR += $(TEST_MOCK_DIR) $(TEST_MOCK_CANARD_WRAPER_DIR) 
INCLUDE_DIR += $(TEST_MOCK_CANARD_DIR) $(TEST_MOCK_DSDL_MESSAGES_DIR)
INCLUDE_DIR += $(TEST_PROGRAM_DIR) $(IMPLEMENTATION_DIR) $(UAVCAN_MESSAGES_DIR)
INCLUDE_DIR += $(CPPUTEST_HOME)/include/

COMPILER_INCLUDE_FLAGS = $(addprefix -I, $(INCLUDE_DIR))

FLAG_FOR_DEFINE = -D IS_RUNNING_TESTS

CXX = g++
CXXFLAGS  =  -Wall $(COMPILER_INCLUDE_FLAGS) $(FLAG_FOR_DEFINE)

##UNCOMMENT TO TEST MEMORY LEAK
#CXXFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorNewMacros.h

LD_LIBRARIES  = -L$(CPPUTEST_HOME)/lib -lCppUTest -lCppUTestExt

BUILD_OUTPUT_DIR = build/

all: create_build_folder compile_tests
	@$(BUILD_OUTPUT_DIR)/tests.out

clean:
	@rm -f -r $(BUILD_OUTPUT_DIR)
	@echo "auxiliary_functions: Build folders were removed"

create_build_folder:
	@mkdir -p $(BUILD_OUTPUT_DIR)

FILES = $(filter %.cpp, $(wildcard ./*))
FILES += $(filter %.cpp, $(wildcard $(TEST_MAIN_FOLDER_DIR)*))
FILES += $(filter %.cpp, $(wildcard $(TEST_MOCK_DSDL_MESSAGES_DIR)*))
FILES += auxiliary_functions.cpp

OBJECTS_TO_LINK = $(addprefix $(BUILD_OUTPUT_DIR), $(addsuffix .o, $(basename $(notdir $(FILES)))))

compile_tests: $(OBJECTS_TO_LINK)
	@echo "auxiliary_functions"
	@$(CXX) $(CXXFLAGS) $^ $(LD_LIBRARIES) -o $(BUILD_OUTPUT_DIR)tests.out

INCLUDE_FILES = $(sort $(filter %.h, $(shell find $(INCLUDE_DIR))))
LIST_OF_MODIFIED_TIMES = $(foreach files, $(INCLUDE_FILES), $(shell date +%s -r $(files)))
LAST_LIBRARY_MODIFIED_TIME = $(lastword $(sort $(LIST_OF_MODIFIED_TIMES)))

vpath %.cpp $(TEST_MAIN_FOLDER_DIR)
vpath %.cpp $(TEST_MOCK_DIR)
vpath %.cpp $(TEST_MOCK_DSDL_MESSAGES_DIR)
vpath %.cpp $(IMPLEMENTATION_DIR)

#Compile files into object files.
$(BUILD_OUTPUT_DIR)%.o : %.cpp
	@$(CXX) $(CXXFLAGS) $(LD_LIBRARIES) $? -c -o $@;

print:
	@echo $(foreach file,$(TEST_MOCK_CANARD_DIR),"\r\n"$(file))
	@ls $(TEST_MOCK_CANARD_DIR)