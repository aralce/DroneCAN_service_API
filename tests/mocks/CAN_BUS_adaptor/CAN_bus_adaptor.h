#pragma once
#include <canard.h>

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

class CAN_driver_interface {
public:
    bool send_frame(CanardCANFrame& can_frame) {
        mock().actualCall("CAN_driver_interface->send_frame")
              .withParameterOfType("CanardCANFrame", "can_frame", (const void*)&can_frame);
        return mock().returnBoolValueOrDefault(SUCCESS);
    }

    CanardCANFrame read_frame() {
        static CanardCANFrame frame{};
        mock().actualCall("CAN_driver_interface->read_frame");
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

