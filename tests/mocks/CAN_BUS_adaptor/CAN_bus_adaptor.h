#pragma once
#include <canard.h>

#include <common_to_all_mocks.h>

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

class CAN_driver_interface {
public:
    bool send_frame(CanardCANFrame& can_frame) {
        mock().actualCall("CAN_driver_interface->send_frame")
              .withParameterOfType("CanardCANFrame", "can_frame", (const void*)&can_frame);
        return mock().returnBoolValueOrDefault(SUCCESS);
    }

    bool read_frame(CanardCANFrame& frame) {
        mock().actualCall("CAN_driver_interface->read_frame")
              .withOutputParameterOfType("CanardCANFrame", "frame", (void*)&frame);
        return mock().returnBoolValueOrDefault(false);
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
        {
            printf("ID_MISMATCH -- id_1: %u | id_2: %u\r\n", can_frame_1->id, can_frame_2->id);
            return false;
        }
        if(can_frame_1->data_len != can_frame_2->data_len)
        {
            printf("LENGTH_MISMATCH -- data_len_1: %u | data_len_2: %u\r\n", can_frame_1->data_len, can_frame_2->data_len);
            return false;
        }
        
        uint8_t* pointer_data_1 = can_frame_1->data;
        uint8_t* pointer_data_2 = can_frame_2->data;
        if(!are_data_pointer_equal(pointer_data_1, pointer_data_2, can_frame_1->data_len))
        {
            printf("DATA MISMATCH\r\n");
            return false;
        }
        return true;
    }

    virtual SimpleString valueToString(const void* can_frame)
    {
        return StringFrom(can_frame);
    }
};

#include <cstring>

class CanardCANFrame_copier : public MockNamedValueCopier
{
public:
    virtual void copy(void* out, const void* in)
    {
        const CanardCANFrame* input = (const CanardCANFrame*)in;
        CanardCANFrame* output = (CanardCANFrame*) out;
        memset(output, 0, sizeof(CanardCANFrame));

        output->id = input->id;
        output->data_len = input->data_len;
        for (int i = 0; i < output->data_len; i++)
            output->data[i] = input->data[i];
    }
};

