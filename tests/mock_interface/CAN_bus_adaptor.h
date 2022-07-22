#pragma once
#include <common_to_all_mocks.h>
#include <canard.h>

typedef struct {
    uint8_t* pointer;
    uint32_t length;
}CAN_data_t;

typedef struct {
    long id;
    CAN_data_t data;
}CAN_bus_driver_frame_t;

class CAN_bus_adaptor {
public:
    CAN_bus_adaptor() {

    }

    int begin(long baudRate) {
        mock().actualCall("begin")
            .withLongIntParameter("baudRate", baudRate);
        return mock().returnIntValueOrDefault(SUCCESS);
    }

    void setPins(int rx, int tx) {
        mock().actualCall("setPins")
            .withIntParameter("rx", rx)
            .withIntParameter("tx", tx);
    }

    bool send_frame(CanardCANFrame& can_frame) {
        mock().actualCall("send_frame")
              .withParameterOfType("CanardCANFrame", "can_frame", (const void*)&can_frame);
        return mock().returnIntValueOrDefault(SUCCESS);
    }

private:
    const int SUCCESS = 1;
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