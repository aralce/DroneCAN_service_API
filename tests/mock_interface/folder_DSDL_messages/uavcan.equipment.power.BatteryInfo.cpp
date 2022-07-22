#include <uavcan.equipment.power.BatteryInfo.h>
#include <common_to_all_mocks.h>

uint32_t uavcan_equipment_power_BatteryInfo_encode(struct uavcan_equipment_power_BatteryInfo* msg, uint8_t* buffer) {
    mock().actualCall("uavcan_equipment_power_BatteryInfo_encode")
          .withParameter("msg", msg)
          .withOutputParameter("buffer", buffer);
    return mock().unsignedIntReturnValue();  
}
