#include "../DroneCAN_service_API.h"
#include "uavcan_driver/uavcan_messages_used.h"
#include "auxiliary_functions.h"
#include <cstring>
#include <limits>
#include <cassert>

CanardPoolAllocatorStatistics statistics
{
};

void DUMMY_error_handler(DroneCAN_error error)
{
}

typedef struct
{
    CanardInstance *instance;
    CanardRxTransfer rx_transfer;
    uint8_t source_node_id;
}canard_reception_t;

canard_reception_t canard_reception;
bool is_there_canard_message_to_handle = false;

void handle_canard_reception(CanardInstance* ins, CanardRxTransfer* transfer)
{
    canard_reception.instance = ins;
    //CanardRxTransfer is copied because It will be used outside this function
    //The canard library can change the value of "transfer" freely without notify
    memcpy(&canard_reception.rx_transfer, transfer, sizeof(CanardRxTransfer));
    canard_reception.source_node_id = transfer->source_node_id;
    is_there_canard_message_to_handle = true;
}

bool should_accept_canard_reception(const CanardInstance* ins, uint64_t* out_data_type_signature,
                                    uint16_t data_type_id, CanardTransferType transfer_type,
                                    uint8_t source_node_id)
{

    switch(data_type_id) {
        case UAVCAN_PROTOCOL_PARAM_GETSET_REQUEST_ID:
            *out_data_type_signature = UAVCAN_PROTOCOL_PARAM_GETSET_REQUEST_SIGNATURE;
            return true;
        case UAVCAN_PROTOCOL_GETNODEINFO_REQUEST_ID:
            *out_data_type_signature = UAVCAN_PROTOCOL_GETNODEINFO_REQUEST_SIGNATURE;
            return true;
        case UAVCAN_PROTOCOL_NODESTATUS_ID:
            *out_data_type_signature = UAVCAN_PROTOCOL_NODESTATUS_SIGNATURE;
            return true;
        default:
            return false;
    }
}

DroneCAN_service::DroneCAN_service(CAN_driver_interface& can_bus, uint8_t node_ID,
                                   droneCAN_handle_error_t handle_error)
: can_driver(can_bus), _node_ID(node_ID)
{
    initialize_system(node_ID, handle_error);

    nodeInfo.name.len = strlen(DRONECAN_NODE_NAME);
    memcpy(nodeInfo.name.data, DRONECAN_NODE_NAME, nodeInfo.name.len);
}

void DroneCAN_service::initialize_system(uint8_t node_ID, droneCAN_handle_error_t handle_error)
{
    _handle_error = handle_error == nullptr ? DUMMY_error_handler : handle_error;
    
    message_sender = new DroneCAN_message_sender(canard, can_driver, _handle_error);

    canard.init(handle_canard_reception, should_accept_canard_reception);
    canard.set_node_ID(node_ID);
}

bool DroneCAN_service::is_CAN_bus_inactive(milliseconds ms_to_consider_can_bus_inactive,
                                           milliseconds actual_time)
{
    if (actual_time - ms_on_last_rx >= ms_to_consider_can_bus_inactive)
        return true;
    return false;
}

bool is_time_to_execute(microseconds& last_time_executed, microseconds actual_time,
                        microseconds time_between_executions);

#define MICROSECONDS_IN_SECOND (uint32_t)1e6
#define NODE_ID_WHEN_NO_BUS_ID_FREE 126

uint32_t DroneCAN_service::run_pending_tasks(microseconds actual_time)
{
    if (is_time_to_publish_nodeStatus_msg(actual_time))
    {
        nodeStatus_struct.uptime_sec = actual_time/MICROSECONDS_IN_SECOND;
        message_sender->broadcast_message(nodeStatus_struct);
    }
    uint32_t ms_until_next_nodeStatus_publish = get_ms_until_next_nodeStatus_publish(actual_time);

    if (is_time_to_publish_batteryInfo_msg(actual_time))
        message_sender->broadcast_message(*_batteryInfo_msg);
    uint32_t ms_until_next_batteryInfo_publish = get_ms_until_next_batteryInfo_publish(actual_time);

    if (is_time_to_clear_bus_nodes_ID_register(actual_time)) //related to dinamic node ID
    {
        memset(node_IDs_on_bus, 0, sizeof(node_IDs_on_bus));
        if (_node_ID == NODE_ID_WHEN_NO_BUS_ID_FREE)
            _node_ID = 1;
    }

    read_can_bus_data_when_is_available(actual_time);
    handle_incoming_message(canard, message_sender);

    canard.set_node_ID(_node_ID);//DEBUG

    return std::min(ms_until_next_nodeStatus_publish, ms_until_next_batteryInfo_publish);
}

bool DroneCAN_service::is_time_to_publish_nodeStatus_msg(microseconds actual_time)
{
    return is_time_to_execute(last_microsecs_since_node_status_publish,
                              actual_time, MICROSECONDS_BETWEEN_NODE_STATUS_PUBLISHES);
}

uint32_t DroneCAN_service::get_ms_until_next_nodeStatus_publish(microseconds actual_time)
{
    uint32_t last_publish_in_ms = last_microsecs_since_node_status_publish/1000;
    constexpr uint32_t ms_publish = MICROSECONDS_BETWEEN_NODE_STATUS_PUBLISHES/1000;
    uint32_t actual_ms = actual_time/1000;
    
    return ms_publish - (actual_ms - last_publish_in_ms);
}

bool DroneCAN_service::is_time_to_publish_batteryInfo_msg(microseconds actual_time)
{
    if (_batteryInfo_msg == nullptr)
        return false;
    return is_time_to_execute(last_microsecs_since_battery_info_publish,
                              actual_time, microsecs_between_battery_info_publish);
}

uint32_t DroneCAN_service::get_ms_until_next_batteryInfo_publish(microseconds actual_time)
{
    if (_batteryInfo_msg == nullptr)
        return std::numeric_limits<uint32_t>::max();
    uint32_t last_publish_in_ms = last_microsecs_since_battery_info_publish/1000;
    uint32_t ms_publish = microsecs_between_battery_info_publish/1000;
    uint32_t actual_ms = actual_time/1000;
    
    return ms_publish - (actual_ms - last_publish_in_ms);
}

bool DroneCAN_service::is_time_to_clear_bus_nodes_ID_register(microseconds actual_time)
{
    return is_time_to_execute(last_microsecs_since_clear_node_ID_reg,
                              actual_time, MICROSECONDS_IN_SECOND);
}

bool is_time_to_execute(microseconds& last_time_executed, microseconds actual_time,
                        microseconds time_between_executions)
{
    bool is_time_to_execute_now = actual_time - last_time_executed >=  time_between_executions;
    if (is_time_to_execute_now) {
        last_time_executed = actual_time;
        return true;
    }else
        return false;
}

#define are_there_data_to_receive()  memcmp(&canard_frame, &no_data_frame, \
                                            sizeof(no_data_frame)) != 0
#define USECS_TO_MS(x) x/1000
//Variable is not stored on stack due it is passed through reference.
static CanardCANFrame no_data_frame{};
static CanardCANFrame canard_frame{};

void DroneCAN_service::read_can_bus_data_when_is_available(microseconds actual_time)
{
    // CanardCANFrame canard_frame{can_driver.read_frame()};
    canard_frame = std::move(can_driver.read_frame());
    if (are_there_data_to_receive())
    {
        ms_on_last_rx = USECS_TO_MS(actual_time);
        
        uint8_t source_id = canard_frame.id & 0x7F; // only first 7 bits are used for source ID
        process_nodeStatus_reception(source_id);

        canard.handle_rx_frame(canard_frame, actual_time);
    }
}

static uavcan_protocol_param_GetSetResponse get_param_getSet(uavcan_parameter_t& param);
using param_request = uavcan_protocol_param_GetSetRequest;
static bool is_param_write_operation(param_request& request);
static bool is_write_on_param_valid(param_request& request, uavcan_parameter_t& param);

void DroneCAN_service::handle_incoming_message(Canard& canard,
                                               DroneCAN_message_sender* message_sender)
{
    if (is_there_canard_message_to_handle == false)
        return;
    
    //Variables are not stored on stack due they are passed by reference
    static uavcan_protocol_param_GetSetRequest paramGetSet_request{};

    if(canard_reception.rx_transfer.data_type_id == UAVCAN_PROTOCOL_GETNODEINFO_REQUEST_ID)
    {
        this->nodeInfo.status = this->nodeStatus_struct;
        // strcpy((char*)get_node_info_response.name.data, DRONECAN_NODE_NAME);
        // get_node_info_response.name.len = strlen(DRONECAN_NODE_NAME);
        message_sender->send_response_message(this->nodeInfo,
                                              canard_reception.source_node_id);
    }
    else if(canard_reception.rx_transfer.data_type_id == UAVCAN_PROTOCOL_PARAM_GETSET_REQUEST_ID)
    {
        uavcan_protocol_param_GetSetRequest_decode(&canard_reception.rx_transfer,
                                                   &paramGetSet_request);

        uavcan_parameter_t parameter_to_send{};
        if (is_param_write_operation(paramGetSet_request))
        {
            uavcan_parameter_t param = this->get_parameter_by_name((char*)paramGetSet_request.name.data);
            parameter_to_send = std::move(param);
            if (is_write_on_param_valid(paramGetSet_request, parameter_to_send))
                parameter_to_send.value = paramGetSet_request.value;
        }
        else
        {
            uavcan_parameter_t param = this->get_parameter(paramGetSet_request.index);
            parameter_to_send = std::move(param);
        }

        uavcan_protocol_param_GetSetResponse response = get_param_getSet(parameter_to_send);
        if (strcmp(NAME_FOR_INVALID_PARAMETER, (char*)parameter_to_send.name.data) != 0)
        {
            message_sender->send_response_message(response,
                                                  canard_reception.source_node_id);
        }
    }
    is_there_canard_message_to_handle = false;
}

static uavcan_protocol_param_GetSetResponse get_param_getSet(uavcan_parameter_t& param)
{
    uavcan_protocol_param_GetSetResponse param_response{};
    param_response.default_value = param.default_value;
    param_response.max_value = param.max_value;
    param_response.min_value = param.min_value;
    param_response.value = param.value;

    memcpy(&param_response.name, &param.name, sizeof(param.name));

    return param_response;
}

static bool is_param_write_operation(param_request& request)
{
    return (request.name.data[0] != 0);
}

static bool is_write_on_param_valid(param_request& request, uavcan_parameter_t& param)
{
    if (param.is_read_only)
        return false;

    if (request.value.union_tag != param.value.union_tag)
        return false;

    if (param.value.union_tag == UAVCAN_PROTOCOL_PARAM_VALUE_REAL_VALUE)
    {
        if (request.value.real_value < param.min_value.real_value)
            return false;
        if (param.max_value.real_value < request.value.real_value)
            return false;
    }

    if (param.value.union_tag == UAVCAN_PROTOCOL_PARAM_VALUE_INTEGER_VALUE)
    {
        if (request.value.integer_value < param.min_value.integer_value)
            return false;
        if (param.max_value.integer_value < request.value.integer_value)
            return false;
    }

    return true;
}

#define NODE_ID_WAS_CHANGED true
#define NODE_ID_NOT_CHANGED false
static void set_flag_on_register_node_IDs(uint8_t node_id, uint32_t* node_IDs_on_bus);
static bool check_flag_on_register_node_IDs(uint8_t node_id, uint32_t* node_IDs_on_bus);
static bool set_node_ID_based_on_registered_IDs(uint8_t& node_ID, uint8_t from, uint8_t to,
                                                uint32_t* node_IDs_on_bus);

void DroneCAN_service::process_nodeStatus_reception(uint8_t source_node_id)
{
    set_flag_on_register_node_IDs(source_node_id, node_IDs_on_bus);
    
    bool was_node_changed = set_node_ID_based_on_registered_IDs(_node_ID, _node_ID, 125, node_IDs_on_bus);

    if (was_node_changed == false)
    {
        was_node_changed = set_node_ID_based_on_registered_IDs(_node_ID, 1, 
                                                               _node_ID, node_IDs_on_bus);
    }
    if (was_node_changed == false)
        _node_ID = NODE_ID_WHEN_NO_BUS_ID_FREE;
}

static bool set_node_ID_based_on_registered_IDs(uint8_t& node_ID, uint8_t from, uint8_t to, uint32_t* node_IDs_on_bus)
{
    int i = from;
    for (; i <= to; i++)
    {
        if (check_flag_on_register_node_IDs(i, node_IDs_on_bus) == false)
        {
            node_ID = i;
            return NODE_ID_WAS_CHANGED;
        }
    }
    return NODE_ID_NOT_CHANGED;
}

#define MAX_NODE_ID_TO_RECEIVE 125
#define FLAG_BITS_PER_REGISTER 32

static void get_register_index_and_bit(uint8_t node_id, uint8_t& reg_index, uint8_t& reg_bit);

static void set_flag_on_register_node_IDs(uint8_t node_id, uint32_t* node_IDs_on_bus)
{
    if (node_id > MAX_NODE_ID_TO_RECEIVE)
        return;
    
    uint8_t register_index, register_bit;
    get_register_index_and_bit(node_id, register_index, register_bit);

    node_IDs_on_bus[register_index] |= (1 << register_bit);
}

static bool check_flag_on_register_node_IDs(uint8_t node_id, uint32_t* node_IDs_on_bus)
{
    if (node_id > MAX_NODE_ID_TO_RECEIVE)
        return true; //Returns as an used ID
    
    uint8_t register_index, register_bit;
    get_register_index_and_bit(node_id, register_index, register_bit);
    
    bool ret = (node_IDs_on_bus[register_index] & (1 << register_bit));
    return ret;
}

#define NODE_IDs_ON_BUS_MAX_INDEX 3

static void get_register_index_and_bit(uint8_t node_id, uint8_t& reg_index, uint8_t& reg_bit)
{
    reg_index = (node_id - 1) / FLAG_BITS_PER_REGISTER;

    #ifdef IS_RUNNING_TESTS
        //this test buffer overflow and underflow(unsigned) when unit test are run
        assert(reg_index <= NODE_IDs_ON_BUS_MAX_INDEX);
    #endif
    //id 0 case
    //id out of range case
    reg_bit = (node_id - 1) % FLAG_BITS_PER_REGISTER;
}

void DroneCAN_service::add_parameter(uavcan_parameter_t& parameter)
{
    if (number_of_parameters < MAX_NUMBER_OF_PARAMETERS) {
        ++number_of_parameters;
        parameter_list.push_back(parameter);
    }
}

void DroneCAN_service::remove_parameter(uint8_t parameter_index_from_0)
{
    if (number_of_parameters != 0) {
        --number_of_parameters;
        auto iterator = parameter_list.begin();
        std::advance(iterator, parameter_index_from_0);
        parameter_list.erase(iterator);
    }
}

bool DroneCAN_service::set_parameter_value_by_name(const char* name, void* pointer_to_value_to_set,
                                                   uavcan_protocol_param_Value_type_t data_type)
{
    switch (data_type) {
        case UAVCAN_PROTOCOL_PARAM_VALUE_BOOLEAN_VALUE:
            return set_parameter_value_by_name(name, *(bool*)pointer_to_value_to_set);
        case UAVCAN_PROTOCOL_PARAM_VALUE_INTEGER_VALUE:
            return set_parameter_value_by_name(name, *(int64_t*)pointer_to_value_to_set);
        case UAVCAN_PROTOCOL_PARAM_VALUE_REAL_VALUE:
            return set_parameter_value_by_name(name, *(float*)pointer_to_value_to_set);
        default:
            return false;
    }
}

uavcan_parameter_t DroneCAN_service::get_parameter_by_name(const char* name)
{
    auto iterator = parameter_list.begin();
    while(iterator != parameter_list.end()) {
        if (strcmp((char*)iterator->name.data, name) == 0)
            return *iterator;
        ++iterator;
    }
    uavcan_parameter_t invalid_parameter;
    strcpy((char*)invalid_parameter.name.data, NAME_FOR_INVALID_PARAMETER);
    return invalid_parameter;
}

uavcan_parameter_t DroneCAN_service::get_parameter(uint8_t parameter_index_from_0)
{
    if (parameter_list.empty() || parameter_index_from_0 >= parameter_list.size())
    {
        uavcan_parameter_t invalid_param;
        strcpy((char*)invalid_param.name.data, NAME_FOR_INVALID_PARAMETER);
        return invalid_param;
    }
    auto iterator = parameter_list.begin();
    std::advance(iterator, parameter_index_from_0);
    return *iterator;
}

bool DroneCAN_service::set_parameter_value_by_name(const char* name, bool value_to_set)
{
    return set_generic_parameter_value_by_name(name, value_to_set);
}

bool DroneCAN_service::set_parameter_value_by_name(const char* name, int32_t value_to_set)
{
    return set_generic_parameter_value_by_name(name, value_to_set);
}

bool DroneCAN_service::set_parameter_value_by_name(const char* name, int64_t value_to_set)
{
    return set_generic_parameter_value_by_name(name, value_to_set);
}

bool DroneCAN_service::set_parameter_value_by_name(const char* name, float value_to_set)
{
    return set_generic_parameter_value_by_name(name, value_to_set);
}

bool DroneCAN_service::set_parameter_value_by_name(const char* name, char* value_to_set)
{
    return set_generic_parameter_value_by_name(name, value_to_set);
}

template <typename PARAM_VALUE_TYPE>
bool DroneCAN_service::set_generic_parameter_value_by_name(const char* name,
                                                           PARAM_VALUE_TYPE value_to_set)
{
    auto iterator = parameter_list.begin();
    while (iterator != parameter_list.end()) {
        if (strcmp((char*)iterator->name.data, name) == 0) {
            iterator->value = package_uavcan_param_value(value_to_set);
            return true;
        }
        ++iterator;
    }
    return false;
}

bool DroneCAN_service::set_parameter_value(uint8_t parameter_index_from_0, bool value_to_set)
{
    return set_generic_parameter_value(parameter_index_from_0, value_to_set);
}

bool DroneCAN_service::set_parameter_value(uint8_t parameter_index_from_0, int32_t value_to_set)
{
    return set_generic_parameter_value(parameter_index_from_0, value_to_set);
}

bool DroneCAN_service::set_parameter_value(uint8_t parameter_index_from_0, int64_t value_to_set)
{ //TODO: add test
    return set_generic_parameter_value(parameter_index_from_0, value_to_set);
}

bool DroneCAN_service::set_parameter_value(uint8_t parameter_index_from_0, float value_to_set)
{
    return set_generic_parameter_value(parameter_index_from_0, value_to_set);
}

bool DroneCAN_service::set_parameter_value(uint8_t parameter_index_from_0, char* value_to_set)
{
    return set_generic_parameter_value(parameter_index_from_0, value_to_set);
}

template <typename PARAM_VALUE_TYPE>
bool DroneCAN_service::set_generic_parameter_value(uint8_t parameter_index_from_0,
                                                   PARAM_VALUE_TYPE value_to_set)
{
    if (parameter_index_from_0 >= parameter_list.size())
        return false;
    auto iterator = parameter_list.begin();
    std::advance(iterator, parameter_index_from_0);
    iterator->value = package_uavcan_param_value(value_to_set);
    return true;
}

void DroneCAN_service::set_node_info(const node_info_data_t& node_info_data)
{
    memcpy(&nodeInfo.software_version, &node_info_data.software_version,
           sizeof(nodeInfo.software_version));
    
    memcpy(&nodeInfo.hardware_version, &node_info_data.hardware_version,
           sizeof(nodeInfo.hardware_version));

    memcpy(&nodeInfo.name, &node_info_data.name, sizeof(nodeInfo.name));
}