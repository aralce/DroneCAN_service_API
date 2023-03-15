name := CAN_BUS_adaptor -> Arduino_driver

TEST_MAIN_FOLDER_DIR = ../../
TEST_PROGRAM_DIR = $(TEST_MAIN_FOLDER_DIR)../
TEST_SUPPORT_DIR = $(TEST_MAIN_FOLDER_DIR)support/
TEST_MOCK_DIR = $(TEST_MAIN_FOLDER_DIR)mocks/
TEST_MOCK_CANARD_DIR = $(TEST_MOCK_DIR)Canard_wrapper/Canard/
TEST_MOCK_CAN_LIBRARY = $(TEST_MOCK_DIR)CAN_BUS_adaptor/arduino_CAN_library/

CPPUTEST_HOME = $(TEST_MAIN_FOLDER_DIR)cpputest

INCLUDE_DIR = $(TEST_MAIN_FOLDER_DIR) $(TEST_SUPPORT_DIR)
INCLUDE_DIR += $(TEST_MOCK_DIR) $(TEST_MOCK_CANARD_DIR) $(TEST_MOCK_CAN_LIBRARY)
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
	@echo "$(name): Build folders were removed"

create_build_folder:
	@mkdir -p $(BUILD_OUTPUT_DIR)

FILES = $(filter %.cpp, $(wildcard ./*))
FILES += $(filter %.cpp, $(wildcard $(TEST_MAIN_FOLDER_DIR)*))
FILES += $(filter %.cpp, $(wildcard $(TEST_MOCK_DIR)*))
FILES += $(filter %.cpp, $(wildcard $(TEST_MOCK_DIR)/CAN_BUS_adaptor/CAN_driver_implementation/Arduino_CAN_driver/*))
FILES += $(filter %.cpp, $(wildcard $(TEST_MOCK_CAN_LIBRARY)*))
FILES += $(filter %.cpp, $(wildcard $(TEST_PROGRAM_DIR)CAN_bus_adaptor/CAN_drivers_implementation/Arduino/*))

OBJECTS_TO_LINK = $(addprefix $(BUILD_OUTPUT_DIR), $(addsuffix .o, $(basename $(notdir $(FILES)))))

compile_tests: $(OBJECTS_TO_LINK)
	@echo "$(name)"
	@$(CXX) $(CXXFLAGS) $^ $(LD_LIBRARIES) -o $(BUILD_OUTPUT_DIR)tests.out

#Compile files into object files.
vpath %.cpp %.h $(TEST_MAIN_FOLDER_DIR)
vpath %.cpp %.h $(TEST_MOCK_DIR)
vpath %.cpp %.h $(TEST_MOCK_CAN_LIBRARY)
vpath %.cpp %.h $(TEST_PROGRAM_DIR)CAN_bus_adaptor/CAN_drivers_implementation/Arduino/
vpath %.cpp %.h $(TEST_MOCK_DIR)/CAN_BUS_adaptor/CAN_driver_implementation/Arduino_CAN_driver/

$(BUILD_OUTPUT_DIR)%.o : %.cpp
	@$(CXX) $(CXXFLAGS) $(LD_LIBRARIES) $? -c -o $@;

print:
	@echo $(OBJECTS_TO_LINK)