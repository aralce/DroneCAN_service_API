/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_INTR_FLAG_LEVEL1        (1<<1)  ///< Accept a Level 1 interrupt vector (lowest priority)

/* -------------------- Default initializers and flags ---------------------- */
/** @cond */    //Doxy command to hide preprocessor definitions from docs
/**
 * @brief Initializer macro for general configuration structure.
 *
 * This initializer macros allows the TX GPIO, RX GPIO, and operating mode to be
 * configured. The other members of the general configuration structure are
 * assigned default values.
 */
#define TWAI_GENERAL_CONFIG_DEFAULT(tx_io_num, rx_io_num, op_mode) {.mode = op_mode, .tx_io = tx_io_num, .rx_io = rx_io_num,        \
                                                                    .clkout_io = TWAI_IO_UNUSED, .bus_off_io = TWAI_IO_UNUSED,      \
                                                                    .tx_queue_len = 5, .rx_queue_len = 5,                           \
                                                                    .alerts_enabled = TWAI_ALERT_NONE,  .clkout_divider = 0,        \
                                                                    .intr_flags = ESP_INTR_FLAG_LEVEL1}

#define TWAI_TIMING_CONFIG_25KBITS()    {.brp = 128, .tseg_1 = 16, .tseg_2 = 8, .sjw = 3, .triple_sampling = false}
#define TWAI_TIMING_CONFIG_50KBITS()    {.brp = 80, .tseg_1 = 15, .tseg_2 = 4, .sjw = 3, .triple_sampling = false}
#define TWAI_TIMING_CONFIG_100KBITS()   {.brp = 40, .tseg_1 = 15, .tseg_2 = 4, .sjw = 3, .triple_sampling = false}
#define TWAI_TIMING_CONFIG_125KBITS()   {.brp = 32, .tseg_1 = 15, .tseg_2 = 4, .sjw = 3, .triple_sampling = false}
#define TWAI_TIMING_CONFIG_250KBITS()   {.brp = 16, .tseg_1 = 15, .tseg_2 = 4, .sjw = 3, .triple_sampling = false}
#define TWAI_TIMING_CONFIG_500KBITS()   {.brp = 8, .tseg_1 = 15, .tseg_2 = 4, .sjw = 3, .triple_sampling = false}
#define TWAI_TIMING_CONFIG_800KBITS()   {.brp = 4, .tseg_1 = 16, .tseg_2 = 8, .sjw = 3, .triple_sampling = false}
#define TWAI_TIMING_CONFIG_1MBITS()     {.brp = 4, .tseg_1 = 15, .tseg_2 = 4, .sjw = 3, .triple_sampling = false}

/**
 * @brief   Initializer macro for filter configuration to accept all IDs
 */
#define TWAI_FILTER_CONFIG_ACCEPT_ALL() {.acceptance_code = 0, .acceptance_mask = 0xFFFFFFFF, .single_filter = true}

/**
 * @brief   Alert flags
 *
 * The following flags represents the various kind of alerts available in
 * the TWAI driver. These flags can be used when configuring/reconfiguring
 * alerts, or when calling twai_read_alerts().
 *
 * @note    The TWAI_ALERT_AND_LOG flag is not an actual alert, but will configure
 *          the TWAI driver to log to UART when an enabled alert occurs.
 */
#define TWAI_ALERT_TX_IDLE                  0x00000001  /**< Alert(1): No more messages to transmit */
#define TWAI_ALERT_TX_SUCCESS               0x00000002  /**< Alert(2): The previous transmission was successful */
#define TWAI_ALERT_BELOW_ERR_WARN           0x00000004  /**< Alert(4): Both error counters have dropped below error warning limit */
#define TWAI_ALERT_ERR_ACTIVE               0x00000008  /**< Alert(8): TWAI controller has become error active */
#define TWAI_ALERT_RECOVERY_IN_PROGRESS     0x00000010  /**< Alert(16): TWAI controller is undergoing bus recovery */
#define TWAI_ALERT_BUS_RECOVERED            0x00000020  /**< Alert(32): TWAI controller has successfully completed bus recovery */
#define TWAI_ALERT_ARB_LOST                 0x00000040  /**< Alert(64): The previous transmission lost arbitration */
#define TWAI_ALERT_ABOVE_ERR_WARN           0x00000080  /**< Alert(128): One of the error counters have exceeded the error warning limit */
#define TWAI_ALERT_BUS_ERROR                0x00000100  /**< Alert(256): A (Bit, Stuff, CRC, Form, ACK) error has occurred on the bus */
#define TWAI_ALERT_TX_FAILED                0x00000200  /**< Alert(512): The previous transmission has failed (for single shot transmission) */
#define TWAI_ALERT_RX_QUEUE_FULL            0x00000400  /**< Alert(1024): The RX queue is full causing a frame to be lost */
#define TWAI_ALERT_ERR_PASS                 0x00000800  /**< Alert(2048): TWAI controller has become error passive */
#define TWAI_ALERT_BUS_OFF                  0x00001000  /**< Alert(4096): Bus-off condition occurred. TWAI controller can no longer influence bus */
#define TWAI_ALERT_RX_FIFO_OVERRUN          0x00002000  /**< Alert(8192): An RX FIFO overrun has occurred */
#define TWAI_ALERT_TX_RETRIED               0x00004000  /**< Alert(16384): An message transmission was cancelled and retried due to an errata workaround */
#define TWAI_ALERT_PERIPH_RESET             0x00008000  /**< Alert(32768): The TWAI controller was reset */
#define TWAI_ALERT_ALL                      0x0000FFFF  /**< Bit mask to enable all alerts during configuration */
#define TWAI_ALERT_NONE                     0x00000000  /**< Bit mask to disable all alerts during configuration */
#define TWAI_ALERT_AND_LOG                  0x00010000  /**< Bit mask to enable alerts to also be logged when they occur. Note that logging from the ISR is disabled if CONFIG_TWAI_ISR_IN_IRAM is enabled (see docs). */

/** @endcond */

#define TWAI_IO_UNUSED                   ((gpio_num_t) -1)   /**< Marks GPIO as unused in TWAI configuration */

/* ----------------------- Enum and Struct Definitions ---------------------- */

/**
 * @brief   TWAI driver states
 */
typedef enum {
    TWAI_STATE_STOPPED,             /**< Stopped state. The TWAI controller will not participate in any TWAI bus activities */
    TWAI_STATE_RUNNING,             /**< Running state. The TWAI controller can transmit and receive messages */
    TWAI_STATE_BUS_OFF,             /**< Bus-off state. The TWAI controller cannot participate in bus activities until it has recovered */
    TWAI_STATE_RECOVERING,          /**< Recovering state. The TWAI controller is undergoing bus recovery */
} twai_state_t;


/**
 * @brief   TWAI Controller operating modes
 */
typedef enum {
    TWAI_MODE_NORMAL,               /**< Normal operating mode where TWAI controller can send/receive/acknowledge messages */
    TWAI_MODE_NO_ACK,               /**< Transmission does not require acknowledgment. Use this mode for self testing */
    TWAI_MODE_LISTEN_ONLY,          /**< The TWAI controller will not influence the bus (No transmissions or acknowledgments) but can receive messages */
} twai_mode_t;

typedef enum {
    GPIO_NUM_NC = -1,    /*!< Use to signal not connected to S/W */
    GPIO_NUM_0 = 0,     /*!< GPIO0, input and output */
    GPIO_NUM_1 = 1,     /*!< GPIO1, input and output */
    GPIO_NUM_2 = 2,     /*!< GPIO2, input and output */
    GPIO_NUM_3 = 3,     /*!< GPIO3, input and output */
    GPIO_NUM_4 = 4,     /*!< GPIO4, input and output */
    GPIO_NUM_5 = 5,     /*!< GPIO5, input and output */
    GPIO_NUM_6 = 6,     /*!< GPIO6, input and output */
    GPIO_NUM_7 = 7,     /*!< GPIO7, input and output */
    GPIO_NUM_8 = 8,     /*!< GPIO8, input and output */
    GPIO_NUM_9 = 9,     /*!< GPIO9, input and output */
    GPIO_NUM_10 = 10,   /*!< GPIO10, input and output */
    GPIO_NUM_11 = 11,   /*!< GPIO11, input and output */
    GPIO_NUM_12 = 12,   /*!< GPIO12, input and output */
    GPIO_NUM_13 = 13,   /*!< GPIO13, input and output */
    GPIO_NUM_14 = 14,   /*!< GPIO14, input and output */
    GPIO_NUM_15 = 15,   /*!< GPIO15, input and output */
    GPIO_NUM_16 = 16,   /*!< GPIO16, input and output */
    GPIO_NUM_17 = 17,   /*!< GPIO17, input and output */
    GPIO_NUM_18 = 18,   /*!< GPIO18, input and output */
    GPIO_NUM_19 = 19,   /*!< GPIO19, input and output */
    GPIO_NUM_20 = 20,   /*!< GPIO20, input and output */
    GPIO_NUM_21 = 21,   /*!< GPIO21, input and output */
    GPIO_NUM_22 = 22,   /*!< GPIO22, input and output */
    GPIO_NUM_23 = 23,   /*!< GPIO23, input and output */
    GPIO_NUM_25 = 25,   /*!< GPIO25, input and output */
    GPIO_NUM_26 = 26,   /*!< GPIO26, input and output */
    GPIO_NUM_27 = 27,   /*!< GPIO27, input and output */
    GPIO_NUM_28 = 28,   /*!< GPIO28, input and output */
    GPIO_NUM_29 = 29,   /*!< GPIO29, input and output */
    GPIO_NUM_30 = 30,   /*!< GPIO30, input and output */
    GPIO_NUM_31 = 31,   /*!< GPIO31, input and output */
    GPIO_NUM_32 = 32,   /*!< GPIO32, input and output */
    GPIO_NUM_33 = 33,   /*!< GPIO33, input and output */
    GPIO_NUM_34 = 34,   /*!< GPIO34, input mode only */
    GPIO_NUM_35 = 35,   /*!< GPIO35, input mode only */
    GPIO_NUM_36 = 36,   /*!< GPIO36, input mode only */
    GPIO_NUM_37 = 37,   /*!< GPIO37, input mode only */
    GPIO_NUM_38 = 38,   /*!< GPIO38, input mode only */
    GPIO_NUM_39 = 39,   /*!< GPIO39, input mode only */
    GPIO_NUM_MAX,
/** @endcond */
} gpio_num_t;

/**
 * @brief   Structure for general configuration of the TWAI driver
 *
 * @note    Macro initializers are available for this structure
 */
typedef struct {
    twai_mode_t mode;               /**< Mode of TWAI controller */
    gpio_num_t tx_io;               /**< Transmit GPIO number */
    gpio_num_t rx_io;               /**< Receive GPIO number */
    gpio_num_t clkout_io;           /**< CLKOUT GPIO number (optional, set to -1 if unused) */
    gpio_num_t bus_off_io;          /**< Bus off indicator GPIO number (optional, set to -1 if unused) */
    uint32_t tx_queue_len;          /**< Number of messages TX queue can hold (set to 0 to disable TX Queue) */
    uint32_t rx_queue_len;          /**< Number of messages RX queue can hold */
    uint32_t alerts_enabled;        /**< Bit field of alerts to enable (see documentation) */
    uint32_t clkout_divider;        /**< CLKOUT divider. Can be 1 or any even number from 2 to 14 (optional, set to 0 if unused) */
    int intr_flags;                 /**< Interrupt flags to set the priority of the driver's ISR. Note that to use the ESP_INTR_FLAG_IRAM, the CONFIG_TWAI_ISR_IN_IRAM option should be enabled first. */
} twai_general_config_t;

/**
 * @brief   Structure to store status information of TWAI driver
 */
typedef struct {
    twai_state_t state;             /**< Current state of TWAI controller (Stopped/Running/Bus-Off/Recovery) */
    uint32_t msgs_to_tx;            /**< Number of messages queued for transmission or awaiting transmission completion */
    uint32_t msgs_to_rx;            /**< Number of messages in RX queue waiting to be read */
    uint32_t tx_error_counter;      /**< Current value of Transmit Error Counter */
    uint32_t rx_error_counter;      /**< Current value of Receive Error Counter */
    uint32_t tx_failed_count;       /**< Number of messages that failed transmissions */
    uint32_t rx_missed_count;       /**< Number of messages that were lost due to a full RX queue (or errata workaround if enabled) */
    uint32_t rx_overrun_count;      /**< Number of messages that were lost due to a RX FIFO overrun */
    uint32_t arb_lost_count;        /**< Number of instances arbitration was lost */
    uint32_t bus_error_count;       /**< Number of instances a bus error has occurred */
} twai_status_info_t;


/**
 * @brief   Structure for bit timing configuration of the TWAI driver
 *
 * @note    Macro initializers are available for this structure
 */
typedef struct {
    uint32_t brp;                   /**< Baudrate prescaler (i.e., APB clock divider). Any even number from 2 to 128 for ESP32, 2 to 32768 for ESP32S2.
                                         For ESP32 Rev 2 or later, multiples of 4 from 132 to 256 are also supported */
    uint8_t tseg_1;                 /**< Timing segment 1 (Number of time quanta, between 1 to 16) */
    uint8_t tseg_2;                 /**< Timing segment 2 (Number of time quanta, 1 to 8) */
    uint8_t sjw;                    /**< Synchronization Jump Width (Max time quanta jump for synchronize from 1 to 4) */
    bool triple_sampling;           /**< Enables triple sampling when the TWAI controller samples a bit */
} twai_timing_config_t;

/**
 * @brief   Structure for acceptance filter configuration of the TWAI driver (see documentation)
 *
 * @note    Macro initializers are available for this structure
 */
typedef struct {
    uint32_t acceptance_code;       /**< 32-bit acceptance code */
    uint32_t acceptance_mask;       /**< 32-bit acceptance mask */
    bool single_filter;             /**< Use Single Filter Mode (see documentation) */
} twai_filter_config_t;

typedef int esp_err_t;

/* Definitions for error constants. */
#define ESP_OK          0       /*!< esp_err_t value indicating success (no error) */
#define ESP_FAIL        -1      /*!< Generic esp_err_t code indicating failure */

#define ESP_ERR_NO_MEM              0x101   /*!< Out of memory */
#define ESP_ERR_INVALID_ARG         0x102   /*!< Invalid argument */
#define ESP_ERR_INVALID_STATE       0x103   /*!< Invalid state */
#define ESP_ERR_INVALID_SIZE        0x104   /*!< Invalid size */
#define ESP_ERR_NOT_FOUND           0x105   /*!< Requested resource not found */
#define ESP_ERR_NOT_SUPPORTED       0x106   /*!< Operation or feature not supported */
#define ESP_ERR_TIMEOUT             0x107   /*!< Operation timed out */
#define ESP_ERR_INVALID_RESPONSE    0x108   /*!< Received response was invalid */
#define ESP_ERR_INVALID_CRC         0x109   /*!< CRC or checksum was invalid */
#define ESP_ERR_INVALID_VERSION     0x10A   /*!< Version was invalid */
#define ESP_ERR_INVALID_MAC         0x10B   /*!< MAC address was invalid */
#define ESP_ERR_NOT_FINISHED        0x10C   /*!< There are items remained to retrieve */


#define ESP_ERR_WIFI_BASE           0x3000  /*!< Starting number of WiFi error codes */
#define ESP_ERR_MESH_BASE           0x4000  /*!< Starting number of MESH error codes */
#define ESP_ERR_FLASH_BASE          0x6000  /*!< Starting number of flash error codes */
#define ESP_ERR_HW_CRYPTO_BASE      0xc000  /*!< Starting number of HW cryptography module error codes */

/**
 * @brief   TWAI Constants
 */
#define TWAI_EXTD_ID_MASK               0x1FFFFFFF  /**< Bit mask for 29 bit Extended Frame Format ID */
#define TWAI_STD_ID_MASK                0x7FF       /**< Bit mask for 11 bit Standard Frame Format ID */
#define TWAI_FRAME_MAX_DLC              8           /**< Max data bytes allowed in TWAI */
#define TWAI_FRAME_EXTD_ID_LEN_BYTES    4           /**< EFF ID requires 4 bytes (29bit) */
#define TWAI_FRAME_STD_ID_LEN_BYTES     2           /**< SFF ID requires 2 bytes (11bit) */
#define TWAI_ERR_PASS_THRESH            128         /**< Error counter threshold for error passive */

/**
 * @brief   Structure to store a TWAI message
 *
 * @note    The flags member is deprecated
 */
typedef struct {
    union {
        struct {
            //The order of these bits must match deprecated message flags for compatibility reasons
            uint32_t extd: 1;           /**< Extended Frame Format (29bit ID) */
            uint32_t rtr: 1;            /**< Message is a Remote Frame */
            uint32_t ss: 1;             /**< Transmit as a Single Shot Transmission. Unused for received. */
            uint32_t self: 1;           /**< Transmit as a Self Reception Request. Unused for received. */
            uint32_t dlc_non_comp: 1;   /**< Message's Data length code is larger than 8. This will break compliance with ISO 11898-1 */
            uint32_t reserved: 27;      /**< Reserved bits */
        };
        //Todo: Deprecate flags
        uint32_t flags;                 /**< Deprecated: Alternate way to set bits using message flags */
    };
    uint32_t identifier;                /**< 11 or 29 bit identifier */
    uint8_t data_length_code;           /**< Data length code */
    uint8_t data[TWAI_FRAME_MAX_DLC];    /**< Data bytes (not relevant in RTR frame) */
} twai_message_t;

typedef uint32_t TickType_t;

/* ------------------------------ Public API -------------------------------- */

/**
 * @brief   Install TWAI driver
 *
 * This function installs the TWAI driver using three configuration structures.
 * The required memory is allocated and the TWAI driver is placed in the stopped
 * state after running this function.
 *
 * @param[in]   g_config    General configuration structure
 * @param[in]   t_config    Timing configuration structure
 * @param[in]   f_config    Filter configuration structure
 *
 * @note    Macro initializers are available for the configuration structures (see documentation)
 *
 * @note    To reinstall the TWAI driver, call twai_driver_uninstall() first
 *
 * @return
 *      - ESP_OK: Successfully installed TWAI driver
 *      - ESP_ERR_INVALID_ARG: Arguments are invalid
 *      - ESP_ERR_NO_MEM: Insufficient memory
 *      - ESP_ERR_INVALID_STATE: Driver is already installed
 */
inline esp_err_t twai_driver_install(const twai_general_config_t *g_config, const twai_timing_config_t *t_config, const twai_filter_config_t *f_config) {
    mock().actualCall("twai_driver_install")
          .withParameterOfType("twai_general_config_t", "g_config", (const void*)g_config)
          .withParameterOfType("twai_timing_config_t", "t_config", (const void*)t_config)
          .withParameterOfType("twai_filter_config_t", "f_config", (const void*)f_config);
    return mock().returnIntValueOrDefault(ESP_OK);
}

/**
 * @brief   Uninstall the TWAI driver
 *
 * This function uninstalls the TWAI driver, freeing the memory utilized by the
 * driver. This function can only be called when the driver is in the stopped
 * state or the bus-off state.
 *
 * @warning The application must ensure that no tasks are blocked on TX/RX
 *          queues or alerts when this function is called.
 *
 * @return
 *      - ESP_OK: Successfully uninstalled TWAI driver
 *      - ESP_ERR_INVALID_STATE: Driver is not in stopped/bus-off state, or is not installed
 */
inline esp_err_t twai_driver_uninstall(void) {
    mock().actualCall("twai_driver_uninstall");
    return mock().returnIntValueOrDefault(ESP_OK);
}

/**
 * @brief   Start the TWAI driver
 *
 * This function starts the TWAI driver, putting the TWAI driver into the running
 * state. This allows the TWAI driver to participate in TWAI bus activities such
 * as transmitting/receiving messages. The TX and RX queue are reset in this function,
 * clearing any messages that are unread or pending transmission. This function
 * can only be called when the TWAI driver is in the stopped state.
 *
 * @return
 *      - ESP_OK: TWAI driver is now running
 *      - ESP_ERR_INVALID_STATE: Driver is not in stopped state, or is not installed
 */
inline esp_err_t twai_start(void) {
    mock().actualCall("twai_start");
    return mock().returnIntValueOrDefault(ESP_OK);
}

/**
 * @brief   Stop the TWAI driver
 *
 * This function stops the TWAI driver, preventing any further message from being
 * transmitted or received until twai_start() is called. Any messages in the TX
 * queue are cleared. Any messages in the RX queue should be read by the
 * application after this function is called. This function can only be called
 * when the TWAI driver is in the running state.
 *
 * @warning A message currently being transmitted/received on the TWAI bus will
 *          be ceased immediately. This may lead to other TWAI nodes interpreting
 *          the unfinished message as an error.
 *
 * @return
 *      - ESP_OK: TWAI driver is now Stopped
 *      - ESP_ERR_INVALID_STATE: Driver is not in running state, or is not installed
 */
inline esp_err_t twai_stop(void) {
    mock().actualCall("twai_stop");
    return mock().returnIntValueOrDefault(ESP_OK);
}

/**
 * @brief   Transmit a TWAI message
 *
 * This function queues a TWAI message for transmission. Transmission will start
 * immediately if no other messages are queued for transmission. If the TX queue
 * is full, this function will block until more space becomes available or until
 * it times out. If the TX queue is disabled (TX queue length = 0 in configuration),
 * this function will return immediately if another message is undergoing
 * transmission. This function can only be called when the TWAI driver is in the
 * running state and cannot be called under Listen Only Mode.
 *
 * @param[in]   message         Message to transmit
 * @param[in]   ticks_to_wait   Number of FreeRTOS ticks to block on the TX queue
 *
 * @note    This function does not guarantee that the transmission is successful.
 *          The TX_SUCCESS/TX_FAILED alert can be enabled to alert the application
 *          upon the success/failure of a transmission.
 *
 * @note    The TX_IDLE alert can be used to alert the application when no other
 *          messages are awaiting transmission.
 *
 * @return
 *      - ESP_OK: Transmission successfully queued/initiated
 *      - ESP_ERR_INVALID_ARG: Arguments are invalid
 *      - ESP_ERR_TIMEOUT: Timed out waiting for space on TX queue
 *      - ESP_FAIL: TX queue is disabled and another message is currently transmitting
 *      - ESP_ERR_INVALID_STATE: TWAI driver is not in running state, or is not installed
 *      - ESP_ERR_NOT_SUPPORTED: Listen Only Mode does not support transmissions
 */
inline esp_err_t twai_transmit(const twai_message_t *message, TickType_t ticks_to_wait) {
    mock().actualCall("twai_transmit")
          .withParameterOfType("twai_message_t", "message", (const void*)message)
          .withUnsignedLongIntParameter("ticks_to_wait", ticks_to_wait);
    return mock().returnIntValueOrDefault(ESP_OK);
}

/**
 * @brief   Receive a TWAI message
 *
 * This function receives a message from the RX queue. The flags field of the
 * message structure will indicate the type of message received. This function
 * will block if there are no messages in the RX queue
 *
 * @param[out]  message         Received message
 * @param[in]   ticks_to_wait   Number of FreeRTOS ticks to block on RX queue
 *
 * @warning The flags field of the received message should be checked to determine
 *          if the received message contains any data bytes.
 *
 * @return
 *      - ESP_OK: Message successfully received from RX queue
 *      - ESP_ERR_TIMEOUT: Timed out waiting for message
 *      - ESP_ERR_INVALID_ARG: Arguments are invalid
 *      - ESP_ERR_INVALID_STATE: TWAI driver is not installed
 */
inline esp_err_t twai_receive(twai_message_t *message, TickType_t ticks_to_wait) {
    mock().actualCall("twai_receive")
          .withOutputParameterOfType("twai_message_t", "message", (void*)message)
          .withUnsignedLongIntParameter("ticks_to_wait", ticks_to_wait);
    return mock().returnIntValueOrDefault(ESP_OK);
}

/**
 * @brief   Read TWAI driver alerts
 *
 * This function will read the alerts raised by the TWAI driver. If no alert has
 * been issued when this function is called, this function will block until an alert
 * occurs or until it timeouts.
 *
 * @param[out]  alerts          Bit field of raised alerts (see documentation for alert flags)
 * @param[in]   ticks_to_wait   Number of FreeRTOS ticks to block for alert
 *
 * @note    Multiple alerts can be raised simultaneously. The application should
 *          check for all alerts that have been enabled.
 *
 * @return
 *      - ESP_OK: Alerts read
 *      - ESP_ERR_TIMEOUT: Timed out waiting for alerts
 *      - ESP_ERR_INVALID_ARG: Arguments are invalid
 *      - ESP_ERR_INVALID_STATE: TWAI driver is not installed
 */
inline esp_err_t twai_read_alerts(uint32_t *alerts, TickType_t ticks_to_wait) {
    mock().actualCall("twai_read_alerts")
          .withPointerParameter("alerts", (void*)alerts)
          .withUnsignedLongIntParameter("ticks_to_wait", ticks_to_wait);
    return mock().returnIntValueOrDefault(ESP_OK);
}

/**
 * @brief   Reconfigure which alerts are enabled
 *
 * This function reconfigures which alerts are enabled. If there are alerts
 * which have not been read whilst reconfiguring, this function can read those
 * alerts.
 *
 * @param[in]   alerts_enabled  Bit field of alerts to enable (see documentation for alert flags)
 * @param[out]  current_alerts  Bit field of currently raised alerts. Set to NULL if unused
 *
 * @return
 *      - ESP_OK: Alerts reconfigured
 *      - ESP_ERR_INVALID_STATE: TWAI driver is not installed
 */
inline esp_err_t twai_reconfigure_alerts(uint32_t alerts_enabled, uint32_t *current_alerts) {
    mock().actualCall("twai_reconfigure_alerts")
          .withUnsignedLongIntParameter("alerts_enabled", alerts_enabled)
          .withPointerParameter("current_alerts", (void*)current_alerts);
    return mock().returnIntValueOrDefault(ESP_OK);
}

/**
 * @brief   Start the bus recovery process
 *
 * This function initiates the bus recovery process when the TWAI driver is in
 * the bus-off state. Once initiated, the TWAI driver will enter the recovering
 * state and wait for 128 occurrences of the bus-free signal on the TWAI bus
 * before returning to the stopped state. This function will reset the TX queue,
 * clearing any messages pending transmission.
 *
 * @note    The BUS_RECOVERED alert can be enabled to alert the application when
 *          the bus recovery process completes.
 *
 * @return
 *      - ESP_OK: Bus recovery started
 *      - ESP_ERR_INVALID_STATE: TWAI driver is not in the bus-off state, or is not installed
 */
inline esp_err_t twai_initiate_recovery(void) {
    mock().actualCall("twai_initiate_recovery");
    return mock().returnIntValueOrDefault(ESP_OK);
}

/**
 * @brief   Get current status information of the TWAI driver
 *
 * @param[out]  status_info     Status information
 *
 * @return
 *      - ESP_OK: Status information retrieved
 *      - ESP_ERR_INVALID_ARG: Arguments are invalid
 *      - ESP_ERR_INVALID_STATE: TWAI driver is not installed
 */
inline esp_err_t twai_get_status_info(twai_status_info_t *status_info) {
    mock().actualCall("twai_get_status_info")
          .withParameterOfType("twai_status_info_t", "status_info", (const void*)status_info);
    return mock().returnIntValueOrDefault(ESP_OK);
}

/**
 * @brief   Clear the transmit queue
 *
 * This function will clear the transmit queue of all messages.
 *
 * @note    The transmit queue is automatically cleared when twai_stop() or
 *          twai_initiate_recovery() is called.
 *
 * @return
 *      - ESP_OK: Transmit queue cleared
 *      - ESP_ERR_INVALID_STATE: TWAI driver is not installed or TX queue is disabled
 */
inline esp_err_t twai_clear_transmit_queue(void) {
    mock().actualCall("twai_clear_transmit_queue");
    return mock().returnIntValueOrDefault(ESP_OK);
}

/**
 * @brief   Clear the receive queue
 *
 * This function will clear the receive queue of all messages.
 *
 * @note    The receive queue is automatically cleared when twai_start() is
 *          called.
 *
 * @return
 *      - ESP_OK: Transmit queue cleared
 *      - ESP_ERR_INVALID_STATE: TWAI driver is not installed
 */
inline esp_err_t twai_clear_receive_queue(void) {
    mock().actualCall("twai_clear_receive_queue");
    return mock().returnIntValueOrDefault(ESP_OK);
}

class Twai_general_config_t_comparator : public MockNamedValueComparator
{
public:
    virtual bool isEqual(const void* object1, const void* object2)
    {
        const twai_general_config_t* config1 = static_cast<const twai_general_config_t*>(object1);
        const twai_general_config_t* config2 = static_cast<const twai_general_config_t*>(object2);

        if (config1->mode != config2->mode) {
            printf("mode mismatch: %d | %d\r\n", config1->mode, config2->mode);
            return false;
        }
        if (config1->tx_io != config2->tx_io) {
            printf("tx_io mismatch: %d | %d\r\n", config1->tx_io, config2->tx_io);
            return false;
        }
        if (config1->rx_io != config2->rx_io) {
            printf("rx_io mismatch: %d | %d\r\n", config1->rx_io, config2->rx_io);
            return false;
        }
        if (config1->clkout_io != config2->clkout_io) {
            printf("clkout mismatch: %d | %d\r\n", config1->clkout_io, config2->clkout_io);
            return false;
        }
        if (config1->bus_off_io != config2->bus_off_io) {
            printf("bus_off_io mismatch: %d | %d\r\n", config1->bus_off_io, config2->bus_off_io);
            return false;
        }
        if (config1->tx_queue_len != config2->tx_queue_len) {
            printf("tx_queue_len mismatch: %d | %d\r\n", config1->tx_queue_len, config2->tx_queue_len);
            return false;
        }
        if (config1->rx_queue_len != config2->rx_queue_len) {
            printf("rx_queue_len mismatch: %d | %d\r\n", config1->rx_queue_len, config2->rx_queue_len);
            return false;
        }
        if (config1->alerts_enabled != config2->alerts_enabled) {
            printf("alerts_enabled mismatch: %X | %X\r\n", config1->alerts_enabled, config2->alerts_enabled);
            return false;
        }
        if (config1->clkout_divider != config2->clkout_divider) {
            printf("clkout_divider mismatch: %d | %d\r\n", config1->clkout_divider, config2->clkout_divider);
            return false;
        }
        if (config1->intr_flags != config2->intr_flags) {
            printf("intr_flags mismatch: %d | %d\r\n", config1->intr_flags, config2->intr_flags);
            return false;
        }
        return true;
    }
    virtual SimpleString valueToString(const void* object)
    {
        return StringFrom(object);
    }
};

class Twai_timing_config_t_comparator : public MockNamedValueComparator
{
public:
    virtual bool isEqual(const void* object1, const void* object2)
    {
        const twai_timing_config_t* config1 = static_cast<const twai_timing_config_t*>(object1);
        const twai_timing_config_t* config2 = static_cast<const twai_timing_config_t*>(object2);

        if (config1->brp != config2->brp) {
            printf("brp mismatch: %d | %d\r\n", config1->brp, config2->brp);
            return false;
        }
        if (config1->tseg_1 != config2->tseg_1) {
            printf("tseg_1 mismatch: %d | %d\r\n", config1->tseg_1, config2->tseg_1);
            return false;
        }
        if (config1->tseg_2 != config2->tseg_2) {
            printf("tseg_2 mismatch: %d | %d\r\n", config1->tseg_2, config2->tseg_2);
            return false;
        }
        if (config1->sjw != config2->sjw) {
            printf("sjw mismatch: %d | %d\r\n", config1->sjw, config2->sjw);
            return false;
        }
        if (config1->triple_sampling != config2->triple_sampling) {
            printf("triple_sampling mismatch: %d | %d\r\n", config1->triple_sampling, config2->triple_sampling);
            return false;
        }
        return true;
    }
    virtual SimpleString valueToString(const void* object)
    {
        return StringFrom(object);
    }
};

class Twai_filter_config_t_comparator : public MockNamedValueComparator
{
public:
    virtual bool isEqual(const void* object1, const void* object2)
    {
        const twai_filter_config_t* config1 = static_cast<const twai_filter_config_t*>(object1);
        const twai_filter_config_t* config2 = static_cast<const twai_filter_config_t*>(object2);

        if (config1->acceptance_code != config2->acceptance_code) {
            printf("acceptance_code mismatch: %u | %u\r\n", config1->acceptance_code, config2->acceptance_code);
            return false;
        }
        if (config1->acceptance_mask != config2->acceptance_mask) {
            printf("acceptancte_mask mismatch: %u | %u\r\n", config1->acceptance_mask, config2->acceptance_mask);
            return false;
        }
        if (config1->single_filter != config2->single_filter) {
            printf("single_filter mismatch: %d | %d\r\n", config1->single_filter, config2->single_filter);
            return false;
        }
        return true;
    }
    virtual SimpleString valueToString(const void* object)
    {
        return StringFrom(object);
    }
};

class Twai_message_t_comparator : public MockNamedValueComparator
{
public:
    virtual bool isEqual(const void* object1, const void* object2)
    {
        const twai_message_t* message1 = static_cast<const twai_message_t*>(object1);
        const twai_message_t* message2 = static_cast<const twai_message_t*>(object2);

        if (message1->data_length_code != message2->data_length_code) {
            printf("data_lenth_code mismatch: %d | %d\r\n", message1->data_length_code, message2->data_length_code);
            return false;
        }
        for (int i = 0; i < message1->data_length_code; i++) {
            if (message1->data[i] != message2->data[i]) {
                printf("data[%d] mismatch: %d | %d\r\n", i, message1->data[i], message2->data[i]);
                return false;
            }
        }
        if (message1->dlc_non_comp != message2->dlc_non_comp) {
            printf("dlc_non_comp mismatch: %d | %d\r\n", message1->dlc_non_comp, message2->dlc_non_comp);
            return false;
        }
        if (message1->extd != message2->extd) {
            printf("extd mismatch: %d | %d\r\n", message1->extd, message2->extd);
            return false;
        }
        if (message1->identifier != message2->identifier) {
            printf("identifier mismatch: %d | %d\r\n", message1->identifier, message2->identifier);
            return false;
        }
        if (message1->rtr != message2->rtr) {
            printf("rtr mismatch: %d | %d\r\n", message1->rtr, message2->rtr);
            return false;
        }
        if (message1->self != message2->self) {
            printf("self mismatch: %d | %d\r\n", message1->self, message2->self);
            return false;
        }
        if (message1->ss != message2->ss) {
            printf("ss mismatch: %d | %d\r\n", message1->ss, message2->ss);
            return false;
        }
        return true;
    }
    virtual SimpleString valueToString(const void* object)
    {
        return StringFrom(object);
    }
};

class Twai_message_t_copier : public MockNamedValueCopier
{
public:
    virtual void copy(void* out, const void* in)
    {
        const twai_message_t* input = static_cast<const twai_message_t*>(in);
        twai_message_t* output = static_cast<twai_message_t*>(out);
        
        for (int i = 0; i < input->data_length_code; i++)
            output->data[i] = input->data[i];
        output->data_length_code = input->data_length_code;
        output->dlc_non_comp = input->dlc_non_comp;
        output->extd = input->extd;
        output->flags = input->flags;
        output->identifier = input->identifier;
        output->reserved = input->reserved;
        output->rtr = input->rtr;
        output->self = input->self;
        output->ss = input->self;
    }
};

#ifdef __cplusplus
}
#endif