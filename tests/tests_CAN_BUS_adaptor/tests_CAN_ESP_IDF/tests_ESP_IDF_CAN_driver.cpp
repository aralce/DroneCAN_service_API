#include <common_to_all_tests.h>
#include <CAN_bus_adaptor/CAN_drivers_implementation/ESP_IDF/ESP_IDF_CAN_driver.h>
#include <mocks/CAN_BUS_adaptor/CAN_driver_implementation/ESP_IDF_CAN_driver/twai.h>
#include <mocks/CAN_BUS_adaptor/CAN_driver_implementation/ESP_IDF_CAN_driver/free_rtos.h>
#include <mocks/CAN_BUS_adaptor/CAN_driver_implementation/ESP_IDF_CAN_driver/common.h>
#include <mocks/CAN_BUS_adaptor/CAN_driver_implementation/ESP_IDF_CAN_driver/task.h>
#include <cstring>

#define COUNT_OF(x) sizeof(x)/sizeof(x[0])
static CAN_bus_adaptor* can_on_startup = nullptr;

TEST_GROUP(CAN_ESP_IDF_driver_on_startup)
{
    Twai_general_config_t_comparator twai_general_config_t_comparator;
    Twai_timing_config_t_comparator twai_timing_config_t_comparator;
    Twai_filter_config_t_comparator twai_filter_config_t_comparator;
    void setup ()
    {
        can_on_startup = new ESP_IDF_CAN_driver();
        mock().installComparator("twai_general_config_t", twai_general_config_t_comparator);
        mock().installComparator("twai_timing_config_t", twai_timing_config_t_comparator);
        mock().installComparator("twai_filter_config_t", twai_filter_config_t_comparator);
    }
    void teardown()
    {
        mock().checkExpectations();
        mock().clear();
        mock().removeAllComparatorsAndCopiers();
        if (can_on_startup != nullptr) {
            mock().disable();
            delete can_on_startup;
            mock().enable();
        }
    }
};

void CHECK_system_is_init(twai_general_config_t* g_config, twai_timing_config_t* t_config, twai_filter_config_t* f_config);

TEST(CAN_ESP_IDF_driver_on_startup, WHEN_driver_is_begun_correctly_THEN_create_a_task_for_receive_and_begin_returns_true)
{
    twai_general_config_t general_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_5, GPIO_NUM_4, TWAI_MODE_NORMAL);
    twai_timing_config_t time_config = TWAI_TIMING_CONFIG_1MBITS();
    twai_filter_config_t filter_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    CHECK_system_is_init(&general_config, &time_config, &filter_config);

    CHECK_TRUE(can_on_startup->begin(CAN_bitrate::CAN_1MBITS));
}

void CHECK_ISSUES_on_twai_driver_install_and_start(int driver_install_return, int start_return);
void CHECK_for_error_message();

TEST(CAN_ESP_IDF_driver_on_startup, WHEN_driver_is_begun_with_issues_THEN_begin_returns_false_and_log_error_message)
{
    CHECK_ISSUES_on_twai_driver_install_and_start(ESP_FAIL, ESP_OK);
    CHECK_for_error_message();
    CHECK_FALSE(can_on_startup->begin(CAN_bitrate::CAN_1MBITS));

    CHECK_ISSUES_on_twai_driver_install_and_start(ESP_OK, ESP_FAIL);
    CHECK_for_error_message();
    CHECK_FALSE(can_on_startup->begin(CAN_bitrate::CAN_1MBITS));
}

void CHECK_ISSUES_on_twai_driver_install_and_start(int driver_install_return, int start_return)
{
    mock().expectOneCall("twai_driver_install")
          .ignoreOtherParameters()
          .andReturnValue(driver_install_return);
    
    mock().expectOneCall("twai_start")
          .andReturnValue(start_return);
}

void CHECK_for_error_message()
{
    mock().expectOneCall("ESP_LOGE")
              .withStringParameter("tag", "CAN_BUS_driver")
              .withStringParameter("message", "Error on CAN BUS driver beginning")
              .ignoreOtherParameters();
}

TEST(CAN_ESP_IDF_driver_on_startup, GIVEN_the_system_is_not_begun_WHEN_setPins_THEN_system_is_begun_with_the_required_pins)
{
    gpio_num_t RX_PIN_TO_SET = GPIO_NUM_20;
    gpio_num_t TX_PIN_TO_SET = GPIO_NUM_25;
    can_on_startup->setPins(RX_PIN_TO_SET, TX_PIN_TO_SET);

    twai_general_config_t general_config = TWAI_GENERAL_CONFIG_DEFAULT(TX_PIN_TO_SET, RX_PIN_TO_SET, TWAI_MODE_NORMAL);
    mock().expectOneCall("twai_driver_install")
          .withParameterOfType("twai_general_config_t", "g_config", (const void*)&general_config)
          .ignoreOtherParameters();
    mock().ignoreOtherCalls();

    can_on_startup->begin(CAN_bitrate::CAN_1MBITS);
}

static CAN_bus_adaptor* can = nullptr;

TEST_GROUP(CAN_ESP_IDF_driver)
{
    Twai_general_config_t_comparator twai_general_config_t_comparator;
    Twai_timing_config_t_comparator twai_timing_config_t_comparator;
    Twai_filter_config_t_comparator twai_filter_config_t_comparator;
    Twai_message_t_comparator twai_message_t_comparator;
    Twai_message_t_copier twai_message_t_copier;
    void setup ()
    {
        can = new ESP_IDF_CAN_driver();
        
        mock().installComparator("twai_general_config_t", twai_general_config_t_comparator);
        mock().installComparator("twai_timing_config_t", twai_timing_config_t_comparator);
        mock().installComparator("twai_filter_config_t", twai_filter_config_t_comparator);
        mock().installComparator("twai_message_t", twai_message_t_comparator);
        mock().installCopier("twai_message_t", twai_message_t_copier);
        
        mock().disable();
        can->begin(CAN_bitrate::CAN_1MBITS);
        mock().enable();
    }
    void teardown()
    {
        mock().checkExpectations();
        mock().clear();
        mock().removeAllComparatorsAndCopiers();
        if (can != nullptr) {
            mock().disable();
            delete can;
            mock().enable();
        }
    }
};

TEST(CAN_ESP_IDF_driver, GIVEN_driver_is_begun_WHEN_receiver_task_runs_THEN_receive_msg_from_CAN_with_blocking)
{
    TaskFunction_t call_receiver_task_function = spy_task_function();
    mock().expectOneCall("twai_receive")
          .withUnsignedLongIntParameter("ticks_to_wait", portMAX_DELAY)
          .ignoreOtherParameters();
    call_receiver_task_function(spy_parameter_passed_to_task_function());
}

TEST(CAN_ESP_IDF_driver, GIVEN_the_system_is_begun_WHEN_setPins_THEN_system_is_re_init)
{
    mock().expectOneCall("twai_stop");
    mock().expectOneCall("twai_driver_uninstall");

    gpio_num_t rx_pin = GPIO_NUM_15;
    gpio_num_t tx_pin = GPIO_NUM_10;
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(tx_pin, rx_pin, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_1MBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    CHECK_system_is_init(&g_config, &t_config, &f_config);

    can->setPins(rx_pin, tx_pin);
}

void CHECK_system_is_init(twai_general_config_t* g_config, twai_timing_config_t* t_config, twai_filter_config_t* f_config)
{
    mock().expectOneCall("twai_driver_install")
          .withParameterOfType("twai_general_config_t", "g_config", (const void*)g_config)
          .withParameterOfType("twai_timing_config_t", "t_config", (const void*)t_config)
          .withParameterOfType("twai_filter_config_t", "f_config", (const void*)f_config);
    
    mock().expectOneCall("twai_start");

    const int PRIORITY = 0;
    mock().expectOneCall("uxTaskPriorityGet")
          .withPointerParameter("xTask", nullptr)
          .andReturnValue(PRIORITY);

    const int HIGHER_PRIORITY = PRIORITY + 1;
    mock().expectOneCall("xTaskCreatePinnedToCore")
          .withIntParameter("uxPriority", HIGHER_PRIORITY)
          .withIntParameter("xCoreID", tskNO_AFFINITY)
          .ignoreOtherParameters();
}

TEST(CAN_ESP_IDF_driver, GIVEN_the_system_is_begun_WHEN_delete_instance_of_driver_THEN_task_is_deleted_and_driver_is_uninstalled)
{
    mock().expectOneCall("twai_stop");
    mock().expectOneCall("twai_driver_uninstall");
    
    TaskHandle_t receiver_task_handle = spy_task_handler();
    mock().expectOneCall("vTaskDelete")
          .withPointerParameter("xTaskToDelete", (void*)receiver_task_handle);
    delete can;
    can = nullptr;
}

TEST(CAN_ESP_IDF_driver, send_can_frame_with_success)
{
    CanardCANFrame can_frame{};
    can_frame.id = 32;
    uint8_t frame_data[8] = {1,2,3,4,5,6,7,8};
    can_frame.data_len = sizeof(frame_data);
    memcpy(can_frame.data, frame_data, can_frame.data_len);

    twai_message_t message{};
    message.identifier = can_frame.id & CANARD_CAN_EXT_ID_MASK;
    message.extd = 1;
    message.data_length_code = can_frame.data_len;
    memcpy(message.data, can_frame.data, can_frame.data_len);
    
    const uint8_t WITHOUT_WAIT = 0;
    mock().expectOneCall("twai_transmit")
          .withParameterOfType("twai_message_t", "message", (const void*)&message)
          .withUnsignedLongIntParameter("ticks_to_wait", WITHOUT_WAIT);

    CHECK_TRUE(can->send_frame(can_frame));
}

TEST(CAN_ESP_IDF_driver, send_can_frame_fails)
{
    mock().expectOneCall("twai_transmit")
          .ignoreOtherParameters()
          .andReturnValue(ESP_FAIL);
    mock().ignoreOtherCalls();

    CanardCANFrame can_frame{};
    CHECK_FALSE(can->send_frame(can_frame));
}

typedef void (*onReceive_callback)(int packet_size);
void onReceive_callback_dummy(int) {
    mock().actualCall("onReceive_callback");
}

TEST(CAN_ESP_IDF_driver, register_an_on_receive_callback_function_for_the_first_time)
{
    onReceive_callback callback = onReceive_callback_dummy;

    can->onReceive(callback);

    TaskFunction_t call_task_function = spy_task_function();
    mock().expectOneCall("onReceive_callback");
    mock().ignoreOtherCalls();
    call_task_function(spy_parameter_passed_to_task_function());
}

TEST(CAN_ESP_IDF_driver, GIVEN_can_frame_is_received_WHEN_read_bytes_from_can_THEN_get_the_correct_data)
{
    twai_message_t received_message = {
        .data_length_code = 8,
        .data = {1, 2, 3, 4, 5, 6, 7, 8},
    };
    received_message.extd = 1;
    mock().expectOneCall("twai_receive")
          .withOutputParameterOfTypeReturning("twai_message_t", "message", (void*)&received_message)
          .ignoreOtherParameters();
    
    TaskFunction_t call_receiver_task_function = spy_task_function();
    call_receiver_task_function(spy_parameter_passed_to_task_function());

    for(int i = 0; i < received_message.data_length_code; i++) {
        int expected_byte = received_message.data[i];
        int actual_value = can->read();
        CHECK_EQUAL(expected_byte, actual_value);
    }
    
    int NO_MORE_DATA_TO_READ = -1;
    int actual_value = can->read();
    CHECK_EQUAL(NO_MORE_DATA_TO_READ, actual_value);
}

TEST(CAN_ESP_IDF_driver, GIVEN_can_frame_is_received_WHEN_get_packet_id_THEN_receive_the_correct_packet_id) {
    uint32_t MESSAGE_ID = 12345 & CANARD_CAN_EXT_ID_MASK;
    twai_message_t received_message = {.identifier = MESSAGE_ID};
    mock().expectOneCall("twai_receive")
          .withOutputParameterOfTypeReturning("twai_message_t", "message", (void*)&received_message)
          .ignoreOtherParameters();

    TaskFunction_t call_receiver_task_function = spy_task_function();
    call_receiver_task_function(spy_parameter_passed_to_task_function());

    uint32_t actual_value = can->get_packet_id();
    CHECK_EQUAL(MESSAGE_ID, actual_value);
}
