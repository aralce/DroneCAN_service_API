#ifdef TARGET_RP2040

#include "CAN_bus_adaptor_for_RP2040.h"
#include <pico.h>
#include <pico-sdk/rp2_common/hardware_irq/include/hardware/irq.h>
#include "hardware/structs/pio.h"
#include <cstring>
#include <cmsis.h>

#include <Arduino.h>

static can2040 can_bus;

bool CAN_bus_adaptor::begin(long baudRate) {
    bitrate = (long)baudRate;
    return true;
}

void CAN_bus_adaptor::setPins(int rx, int tx) {
    can_bus_rx_pin = rx;
    can_bus_tx_pin = tx;
}

bool CAN_bus_adaptor::send_frame(CanardCANFrame& can_frame) {
    if (can2040_check_transmit(&can_bus) == false)
        return false;
    
    can2040_msg msg;
    msg.id = can_frame.id;
    msg.dlc = can_frame.data_len;
    memcpy((void*)&msg.data, (const void*)can_frame.data, sizeof(msg.data));
    
    can2040_transmit(&can_bus, &msg);
    return true;
}

typedef void (*onReceive_callback_ptr)(int packet_size);
onReceive_callback_ptr onReceive_callback;

can2040_msg rx_msg{};

static void can2040_callback(can2040* , uint32_t notify, can2040_msg* msg) {
    if (notify == CAN2040_NOTIFY_RX) {
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        int packet_size = (int)msg->dlc;
        onReceive_callback(packet_size);
        memcpy((void*)&rx_msg, (const void*)msg, sizeof(can2040_msg));
    }
}

static void PIO0_IRQ0_interrupt_handler() {
    can2040_pio_irq_handler(&can_bus);
}

void CAN_bus_adaptor::onReceive(void (*onReceive_callback)(int packet_size)) {
    irq_set_enabled(PIO0_IRQ_0, false);
    
    uint32_t pio_num = 0;
    can2040_setup(&can_bus, pio_num);

    can2040_callback_config(&can_bus, can2040_callback);

    irq_set_exclusive_handler(PIO0_IRQ_0, PIO0_IRQ0_interrupt_handler);
    irq_set_priority(PIO0_IRQ_0, PICO_HIGHEST_IRQ_PRIORITY);
    irq_set_enabled(PIO0_IRQ_0, true);

    uint32_t sys_clock = 125000000;
    can2040_start(&can_bus, sys_clock, bitrate, can_bus_rx_pin, can_bus_tx_pin);
}

int CAN_bus_adaptor::read() {
    static int msg_index = 0;
    int return_val = rx_msg.data[msg_index++];
    if (msg_index >= 8) {
        Serial.println();
        msg_index = 0;
    }
    Serial.print(return_val, HEX);
    return return_val;
}

long CAN_bus_adaptor::get_packet_id() {
    return rx_msg.id;
}

#endif //TARGET_RP2040