#pragma once

#ifdef IS_RUNNING_TESTS
    #include "tests/mocks/folder_DSDL_messages/uavcan.equipment.power.BatteryInfo.h"
    #include "tests/mocks/folder_DSDL_messages/uavcan.protocol.GetNodeInfo.h"
    #include "tests/mocks/folder_DSDL_messages/uavcan.protocol.NodeStatus.h"
    #include "tests/mocks/folder_DSDL_messages/uavcan.protocol.param.GetSet_req.h"
    #include "tests/mocks/folder_DSDL_messages/uavcan.protocol.param.GetSet_res.h"
    #include "tests/mocks/folder_DSDL_messages/uavcan.protocol.param.NumericValue.h"
    #include "tests/mocks/folder_DSDL_messages/uavcan.protocol.param.Value.h"
#else
    #include "uavcan_messages/uavcan.equipment.power.BatteryInfo.h"
    #include "uavcan_messages/uavcan.protocol.GetNodeInfo.h"
    #include "uavcan_messages/uavcan.protocol.NodeStatus.h"
    #include "uavcan_messages/uavcan.protocol.param.GetSet_req.h"
    #include "uavcan_messages/uavcan.protocol.param.GetSet_res.h"
    #include "uavcan_messages/uavcan.protocol.param.NumericValue.h"
    #include "uavcan_messages/uavcan.protocol.param.Value.h"
#endif // IS_RUNNING_TESTS


#ifdef IS_RUNNING_TESTS
#else
#endif // IS_RUNNING_TESTS