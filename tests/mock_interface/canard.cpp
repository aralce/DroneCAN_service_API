#include <canard.h>
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h" 

void canardInit(CanardInstance* out_ins, void* mem_arena, size_t mem_arena_size, CanardOnTransferReception on_reception, CanardShouldAcceptTransfer should_accept, void* user_reference)
{
    mock().actualCall("canardInit");
}