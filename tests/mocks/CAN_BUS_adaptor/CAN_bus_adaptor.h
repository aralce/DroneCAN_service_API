#pragma once
#include <canard.h>

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

class CAN_bus_adaptor {
public:
    bool send_frame(CanardCANFrame& can_frame) {
        mock().actualCall("CAN_bus_adaptor->send_frame")
              .withParameterOfType("CanardCANFrame", "can_frame", (const void*)&can_frame);
        return mock().returnBoolValueOrDefault(SUCCESS);
    }

    CanardCANFrame read() {
        static CanardCANFrame frame{};
        mock().actualCall("CAN_bus_adaptor->read");
        return *(CanardCANFrame*)mock().returnPointerValueOrDefault(&frame);
    }

    CanardCANFrame read_master_mailbox() {
        static CanardCANFrame frame{};
        mock().actualCall("CAN_bus_adaptor->read_master_mailbox");
        return *(CanardCANFrame*)mock().returnPointerValueOrDefault(&frame);
    }

private:
    const int SUCCESS = true;
};

class CanardCANFrame_comparator : public MockNamedValueComparator
{
public:
    virtual bool isEqual(const void* object1, const void* object2)
    {
        CanardCANFrame* can_frame_1 = (CanardCANFrame*)object1;
        CanardCANFrame* can_frame_2 = (CanardCANFrame*)object2;

        if(can_frame_1->id != can_frame_2->id)
            return false;
        if(can_frame_1->data_len != can_frame_2->data_len)
            return false;
        
        uint8_t* pointer_data_1 = can_frame_1->data;
        uint8_t* pointer_data_2 = can_frame_2->data;
        if(!are_data_pointer_equal(pointer_data_1, pointer_data_2, can_frame_1->data_len))
            return false;    
        return true;
    }

    virtual SimpleString valueToString(const void* can_frame)
    {
        return StringFrom(can_frame);
    }
};

