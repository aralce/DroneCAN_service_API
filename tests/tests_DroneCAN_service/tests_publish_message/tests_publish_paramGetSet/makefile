TEST_MAIN_FOLDER_DIR = ../../../
TEST_DRONE_CAN_DIR = ../../
TEST_PROGRAM_DIR = $(TEST_MAIN_FOLDER_DIR)../
TEST_SUPPORT_DIR = $(TEST_MAIN_FOLDER_DIR)support/
TEST_MOCK_DIR = $(TEST_MAIN_FOLDER_DIR)mocks/
TEST_MOCK_CAN_BUS_ADAPTOR_DIR = $(TEST_MOCK_DIR)CAN_BUS_adaptor/
TEST_MOCK_CANARD_WRAPPER_DIR = $(TEST_MOCK_DIR)Canard_wrapper/
TEST_MOCK_DSDL_MESSAGES_DIR = $(TEST_MOCK_DIR)/folder_DSDL_messages/

CPPUTEST_HOME = $(TEST_MAIN_FOLDER_DIR)cpputest

INCLUDE_DIR = $(TEST_MAIN_FOLDER_DIR) $(TEST_SUPPORT_DIR) $(TEST_DRONE_CAN_DIR)
INCLUDE_DIR += $(TEST_MOCK_DIR) $(TEST_MOCK_CAN_BUS_ADAPTOR_DIR)
INCLUDE_DIR += $(TEST_MOCK_CANARD_WRAPPER_DIR) $(TEST_MOCK_DSDL_MESSAGES_DIR)
INCLUDE_DIR += $(TEST_PROGRAM_DIR)
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
	@echo "DroneCAN_service-->publish_message-->paramGetSet: Build folders were removed"

create_build_folder:
	@mkdir -p $(BUILD_OUTPUT_DIR)

FILES = $(filter %.cpp, $(wildcard ./*))
FILES += $(filter %.cpp, $(wildcard $(TEST_MAIN_FOLDER_DIR)*))
FILES += $(filter %.cpp, $(wildcard $(TEST_MOCK_DIR)*))
FILES += $(filter %.cpp, $(wildcard $(TEST_MOCK_DSDL_MESSAGES_DIR)*))
FILES += $(filter %.cpp, $(wildcard $(TEST_PROGRAM_DIR)*))
FILES += $(filter %.cpp, $(wildcard $(TEST_DRONE_CAN_DIR)*))
FILES += $(filter %.cpp, $(wildcard ../common_to_publish_message/*))

OBJECTS_TO_LINK = $(addprefix $(BUILD_OUTPUT_DIR), $(addsuffix .o, $(basename $(notdir $(FILES)))))

compile_tests: $(OBJECTS_TO_LINK) $(BUILD_OUTPUT_DIR)common_to_paramGetSet.o
	@echo "DroneCAN_service-->publish_message-->paramGetSet"
	@$(CXX) $(CXXFLAGS) $^ $(LD_LIBRARIES) -o $(BUILD_OUTPUT_DIR)tests.out

INCLUDE_FILES = $(sort $(filter %.h, $(shell find $(INCLUDE_DIR))))
LIST_OF_MODIFIED_TIMES = $(foreach files, $(INCLUDE_FILES), $(shell date +%s -r $(files)))
LAST_LIBRARY_MODIFIED_TIME = $(lastword $(sort $(LIST_OF_MODIFIED_TIMES)))

#Compile files into object files.
$(BUILD_OUTPUT_DIR)%.o : %.cpp
	@$(CXX) $(CXXFLAGS) $(LD_LIBRARIES) $? -c -o $@;

$(BUILD_OUTPUT_DIR)%.o : $(TEST_MAIN_FOLDER_DIR)%.cpp
	@$(CXX) $(CXXFLAGS) $(LD_LIBRARIES) $? -c -o $@;

$(BUILD_OUTPUT_DIR)%.o : $(TEST_MOCK_DIR)%.cpp
	@$(CXX) $(CXXFLAGS) $(LD_LIBRARIES) $? -c -o $@;

$(BUILD_OUTPUT_DIR)%.o : $(TEST_MOCK_DSDL_MESSAGES_DIR)%.cpp
	@$(CXX) $(CXXFLAGS) $(LD_LIBRARIES) $? -c -o $@;

$(BUILD_OUTPUT_DIR)%.o : $(TEST_PROGRAM_DIR)%.cpp
	@$(CXX) $(CXXFLAGS) $(LD_LIBRARIES) $? -c -o $@;

$(BUILD_OUTPUT_DIR)%.o : ../common_to_publish_message/%.cpp
	@$(CXX) $(CXXFLAGS) $(LD_LIBRARIES) $? -c -o $@;

$(BUILD_OUTPUT_DIR)%.o : common_to_paramGetSet/%.cpp
	@$(CXX) $(CXXFLAGS) $(LD_LIBRARIES) $? -c -o $@;

$(BUILD_OUTPUT_DIR)%.o : $(TEST_DRONE_CAN_DIR)%.cpp
	@$(CXX) $(CXXFLAGS) $(LD_LIBRARIES) $? -c -o $@;