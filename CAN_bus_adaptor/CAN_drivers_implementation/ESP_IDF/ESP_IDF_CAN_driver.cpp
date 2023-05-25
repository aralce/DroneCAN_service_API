#include "ESP_IDF_CAN_driver.h"
#include <string.h>

#if defined(IS_RUNNING_TESTS)
    #include <CAN_BUS_adaptor/CAN_driver_implementation/ESP_IDF_CAN_driver/twai.h>
    #include <CAN_BUS_adaptor/CAN_driver_implementation/ESP_IDF_CAN_driver/common.h>
    #include <CAN_BUS_adaptor/CAN_driver_implementation/ESP_IDF_CAN_driver/free_rtos.h>
    #include <CAN_BUS_adaptor/CAN_driver_implementation/ESP_IDF_CAN_driver/task.h>
#else
    #include <driver/twai.h>
    #include <esp_log.h>
    #include <freertos/FreeRTOS.h>
    #include <freertos/task.h>
#endif

static const char TAG[] = "CAN_BUS_driver";
twai_timing_config_t get_twai_config_from_CAN_bitrate(CAN_bitrate bitrate);

typedef void (*onReceive_callback_t)(int packet_size);
static onReceive_callback_t onReceive_callback = nullptr;
static TaskHandle_t receiver_task_handler = nullptr;

ESP_IDF_CAN_driver::~ESP_IDF_CAN_driver() {
    onReceive_callback = nullptr;
    twai_stop();
    twai_driver_uninstall();
    vTaskDelete(receiver_task_handler);
}

#define CORE_0 0
static void on_receive_caller_task(void* received_frame_buffer);

bool ESP_IDF_CAN_driver::begin(CAN_bitrate bitrate) {
    _bitrate = bitrate;
    twai_general_config_t general_config = TWAI_GENERAL_CONFIG_DEFAULT((gpio_num_t)tx_pin, (gpio_num_t)rx_pin, TWAI_MODE_NORMAL);
    twai_timing_config_t timing_config = get_twai_config_from_CAN_bitrate(bitrate);
    twai_filter_config_t filter_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    bool is_successful = twai_driver_install(&general_config, &timing_config, &filter_config) == ESP_OK;
    is_successful &= twai_start() == ESP_OK;
    if (not is_successful) {
        ESP_LOGE(TAG, "%s", "Error on CAN BUS driver beginning");
        return false;
    }

    const uint32_t TASK_STACK_1KB  = 1000;
    UBaseType_t main_task_priority = uxTaskPriorityGet(nullptr);
    xTaskCreatePinnedToCore(on_receive_caller_task, "CAN_BUS_receiver_task", TASK_STACK_1KB, received_frame_buffer, main_task_priority+1, &receiver_task_handler, CORE_0);
    is_driver_running = true;
    return true;
}

twai_timing_config_t get_twai_config_from_CAN_bitrate(CAN_bitrate bitrate) {
    switch(bitrate) {
        case CAN_bitrate::CAN_25KBITS:
            return TWAI_TIMING_CONFIG_25KBITS();
        case CAN_bitrate::CAN_50KBITS:
            return TWAI_TIMING_CONFIG_50KBITS();
        case CAN_bitrate::CAN_100KBITS:
            return TWAI_TIMING_CONFIG_100KBITS();
        case CAN_bitrate::CAN_125KBITS:
            return TWAI_TIMING_CONFIG_125KBITS();
        case CAN_bitrate::CAN_250KBITS:
            return TWAI_TIMING_CONFIG_250KBITS();
        case CAN_bitrate::CAN_500KBITS:
            return TWAI_TIMING_CONFIG_500KBITS();
        case CAN_bitrate::CAN_800KBITS:
            return TWAI_TIMING_CONFIG_800KBITS();
        case CAN_bitrate::CAN_1MBITS:
            return TWAI_TIMING_CONFIG_1MBITS();
        default:
            return TWAI_TIMING_CONFIG_1MBITS();
    }
}

static void on_receive_caller_task(void* received_frame_buffer) {
    twai_status_info_t status_info{};

    #ifndef IS_RUNNING_TESTS
    for(;;) {
    #endif
        twai_get_status_info(&status_info);

        if (onReceive_callback != nullptr && status_info.msgs_to_rx > 0)
            onReceive_callback(0);
    #ifndef IS_RUNNING_TESTS
    }
    #endif
}

void ESP_IDF_CAN_driver::setPins(int rx, int tx) {
    rx_pin = rx;
    tx_pin = tx;

    if (is_driver_running) {
        twai_stop();
        twai_driver_uninstall();
        begin(_bitrate);
    }
}

#define WITHOUT_WAIT 0

bool ESP_IDF_CAN_driver::send_frame(CanardCANFrame& can_frame) {
    twai_message_t message_to_send{};
    message_to_send.identifier = can_frame.id & CANARD_CAN_EXT_ID_MASK;
    message_to_send.extd = 1;
    message_to_send.data_length_code = can_frame.data_len;
    memcpy(message_to_send.data, can_frame.data, message_to_send.data_length_code);
    
    return twai_transmit(&message_to_send, WITHOUT_WAIT) == ESP_OK;
}

void ESP_IDF_CAN_driver::onReceive(onReceive_callback_t onReceive_function) {
    onReceive_callback = onReceive_function;
}

#define NO_MORE_BYTES_TO_READ -1
int ESP_IDF_CAN_driver::read() {
    if (should_request_a_new_frame())
        receive_new_CAN_frame_from_buffer();

    if (is_there_error_on_frame_reception)
        return NO_MORE_BYTES_TO_READ;

    uint8_t last_byte = msg_data_length - 1;
    if (index_of_byte_read_from_buffer == last_byte) {
        clear_should_request_new_frame_flag();
        return received_frame_buffer[last_byte];
    }
    else
        return received_frame_buffer[index_of_byte_read_from_buffer++];
}

void ESP_IDF_CAN_driver::clear_should_request_new_frame_flag() {
    index_of_byte_read_from_buffer = 0;
    was_packet_id_gotten = false;
}

long ESP_IDF_CAN_driver::get_packet_id() {
    if (should_request_a_new_frame())
        receive_new_CAN_frame_from_buffer();
    
    was_packet_id_gotten = true;
    return received_frame_id;
}

bool ESP_IDF_CAN_driver::should_request_a_new_frame() {
    return (index_of_byte_read_from_buffer == 0) && (was_packet_id_gotten == false);
}

void ESP_IDF_CAN_driver::receive_new_CAN_frame_from_buffer() {
    twai_message_t received_msg{};

    esp_err_t error = twai_receive(&received_msg, 0);

    is_there_error_on_frame_reception = error != ESP_OK ? true : false;
    received_frame_id = received_msg.identifier;
    msg_data_length = received_msg.data_length_code;
    memcpy(received_frame_buffer, received_msg.data, msg_data_length);
}

bool ESP_IDF_CAN_driver::try_CAN_write(const uint8_t* buffer, size_t size) {
    return false;
}
