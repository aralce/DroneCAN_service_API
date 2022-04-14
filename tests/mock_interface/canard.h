#pragma once
#include <cstddef>
#include <cstdint>

/// Libcanard version. API will be backwards compatible within the same major version.
#define CANARD_VERSION_MAJOR                        0
#define CANARD_VERSION_MINOR                        2


#ifndef CANARD_ENABLE_CANFD
#define CANARD_ENABLE_CANFD                         0
#endif


#ifndef CANARD_ENABLE_TAO_OPTION
#if CANARD_ENABLE_CANFD
#define CANARD_ENABLE_TAO_OPTION                    1
#else
#define CANARD_ENABLE_TAO_OPTION                    0
#endif
#endif

/// By default this macro resolves to the standard assert(). The user can redefine this if necessary.
#ifndef CANARD_ASSERT
# define CANARD_ASSERT(x)   assert(x)
#endif

#define CANARD_GLUE(a, b)           CANARD_GLUE_IMPL_(a, b)
#define CANARD_GLUE_IMPL_(a, b)     a##b

/// By default this macro expands to static_assert if supported by the language (C11, C++11, or newer).
/// The user can redefine this if necessary.
#ifndef CANARD_STATIC_ASSERT
# if (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)) ||\
     (defined(__cplusplus) && (__cplusplus >= 201103L))
#  define CANARD_STATIC_ASSERT(...) static_assert(__VA_ARGS__)
# else
#  define CANARD_STATIC_ASSERT(x, ...) typedef char CANARD_GLUE(_static_assertion_, __LINE__)[(x) ? 1 : -1]
# endif
#endif

/// Error code definitions; inverse of these values may be returned from API calls.
#define CANARD_OK                                      0
// Value 1 is omitted intentionally, since -1 is often used in 3rd party code
#define CANARD_ERROR_INVALID_ARGUMENT                  2
#define CANARD_ERROR_OUT_OF_MEMORY                     3
#define CANARD_ERROR_NODE_ID_NOT_SET                   4
#define CANARD_ERROR_INTERNAL                          9
#define CANARD_ERROR_RX_INCOMPATIBLE_PACKET            10
#define CANARD_ERROR_RX_WRONG_ADDRESS                  11
#define CANARD_ERROR_RX_NOT_WANTED                     12
#define CANARD_ERROR_RX_MISSED_START                   13
#define CANARD_ERROR_RX_WRONG_TOGGLE                   14
#define CANARD_ERROR_RX_UNEXPECTED_TID                 15
#define CANARD_ERROR_RX_SHORT_FRAME                    16
#define CANARD_ERROR_RX_BAD_CRC                        17

/// The size of a memory block in bytes.
#if CANARD_ENABLE_CANFD
#define CANARD_MEM_BLOCK_SIZE                       128U
#else
#define CANARD_MEM_BLOCK_SIZE                       32U
#endif

#define CANARD_CAN_FRAME_MAX_DATA_LEN               8U
#if CANARD_ENABLE_CANFD
#define CANARD_CANFD_FRAME_MAX_DATA_LEN             64U
#endif

/// Node ID values. Refer to the specification for more info.
#define CANARD_BROADCAST_NODE_ID                    0
#define CANARD_MIN_NODE_ID                          1
#define CANARD_MAX_NODE_ID                          127

/// Refer to the type CanardRxTransfer
#define CANARD_MULTIFRAME_RX_PAYLOAD_HEAD_SIZE      (CANARD_MEM_BLOCK_SIZE - offsetof(CanardRxState, buffer_head))

/// Refer to the type CanardBufferBlock
#define CANARD_BUFFER_BLOCK_DATA_SIZE               (CANARD_MEM_BLOCK_SIZE - offsetof(CanardBufferBlock, data))

/// Refer to canardCleanupStaleTransfers() for details.
#define CANARD_RECOMMENDED_STALE_TRANSFER_CLEANUP_INTERVAL_USEC     1000000U

/// Transfer priority definitions
#define CANARD_TRANSFER_PRIORITY_HIGHEST            0
#define CANARD_TRANSFER_PRIORITY_HIGH               8
#define CANARD_TRANSFER_PRIORITY_MEDIUM             16
#define CANARD_TRANSFER_PRIORITY_LOW                24
#define CANARD_TRANSFER_PRIORITY_LOWEST             31

/// Related to CanardCANFrame
#define CANARD_CAN_EXT_ID_MASK                      0x1FFFFFFFU
#define CANARD_CAN_STD_ID_MASK                      0x000007FFU
#define CANARD_CAN_FRAME_EFF                        (1UL << 31U)         ///< Extended frame format
#define CANARD_CAN_FRAME_RTR                        (1UL << 30U)         ///< Remote transmission (not used by UAVCAN)
#define CANARD_CAN_FRAME_ERR                        (1UL << 29U)         ///< Error frame (not used by UAVCAN)

#define CANARD_TRANSFER_PAYLOAD_LEN_BITS            10U
#define CANARD_MAX_TRANSFER_PAYLOAD_LEN             ((1U << CANARD_TRANSFER_PAYLOAD_LEN_BITS) - 1U)


/**
 * This data type holds a standard CAN 2.0B data frame with 29-bit ID.
 */
typedef struct
{
    /**
     * Refer to the following definitions:
     *  - CANARD_CAN_FRAME_EFF
     *  - CANARD_CAN_FRAME_RTR
     *  - CANARD_CAN_FRAME_ERR
     */
    uint32_t id;
#if CANARD_ENABLE_CANFD
    uint8_t data[CANARD_CANFD_FRAME_MAX_DATA_LEN];
#else
    uint8_t data[CANARD_CAN_FRAME_MAX_DATA_LEN];
#endif
    uint8_t data_len;
#if CANARD_ENABLE_CANFD
    bool canfd;
#endif
} CanardCANFrame;

/**
 * Transfer types are defined by the UAVCAN specification.
 */
typedef enum
{
    CanardTransferTypeResponse  = 0,
    CanardTransferTypeRequest   = 1,
    CanardTransferTypeBroadcast = 2
} CanardTransferType;

/**
 * Types of service transfers. These are not applicable to message transfers.
 */
typedef enum
{
    CanardResponse,
    CanardRequest
} CanardRequestResponse;

/*
 * Forward declarations.
 */
typedef struct CanardInstance CanardInstance;
typedef struct CanardRxTransfer CanardRxTransfer;
typedef struct CanardRxState CanardRxState;
typedef struct CanardTxQueueItem CanardTxQueueItem;

/**
 * The application must implement this function and supply a pointer to it to the library during initialization.
 * The library calls this function to determine whether the transfer should be received.
 *
 * If the application returns true, the value pointed to by 'out_data_type_signature' must be initialized with the
 * correct data type signature, otherwise transfer reception will fail with CRC mismatch error. Please refer to the
 * specification for more details about data type signatures. Signature for any data type can be obtained in many
 * ways; for example, using the command line tool distributed with Libcanard (see the repository).
 */
typedef bool (* CanardShouldAcceptTransfer)(const CanardInstance* ins,          ///< Library instance
                                            uint64_t* out_data_type_signature,  ///< Must be set by the application!
                                            uint16_t data_type_id,              ///< Refer to the specification
                                            CanardTransferType transfer_type,   ///< Refer to CanardTransferType
                                            uint8_t source_node_id);            ///< Source node ID or Broadcast (0)

/**
 * This function will be invoked by the library every time a transfer is successfully received.
 * If the application needs to send another transfer from this callback, it is highly recommended
 * to call canardReleaseRxTransferPayload() first, so that the memory that was used for the block
 * buffer can be released and re-used by the TX queue.
 */
typedef void (* CanardOnTransferReception)(CanardInstance* ins,                 ///< Library instance
                                           CanardRxTransfer* transfer);         ///< Ptr to temporary transfer object

/**
 * INTERNAL DEFINITION, DO NOT USE DIRECTLY.
 * A memory block used in the memory block allocator.
 */
typedef union CanardPoolAllocatorBlock_u
{
    char bytes[CANARD_MEM_BLOCK_SIZE];
    union CanardPoolAllocatorBlock_u* next;
} CanardPoolAllocatorBlock;

/**
 * This structure provides usage statistics of the memory pool allocator.
 * This data helps to evaluate whether the allocated memory is sufficient for the application.
 */
typedef struct
{
    uint16_t capacity_blocks;               ///< Pool capacity in number of blocks
    uint16_t current_usage_blocks;          ///< Number of blocks that are currently allocated by the library
    uint16_t peak_usage_blocks;             ///< Maximum number of blocks used since initialization
} CanardPoolAllocatorStatistics;

/**
 * INTERNAL DEFINITION, DO NOT USE DIRECTLY.
 */
typedef struct
{
    CanardPoolAllocatorBlock* free_list;
    CanardPoolAllocatorStatistics statistics;
} CanardPoolAllocator;

/**
 * INTERNAL DEFINITION, DO NOT USE DIRECTLY.
 * Buffer block for received data.
 */
typedef struct CanardBufferBlock
{
    struct CanardBufferBlock* next;
    uint8_t data[];
} CanardBufferBlock;

/**
 * INTERNAL DEFINITION, DO NOT USE DIRECTLY.
 */
struct CanardRxState
{
    struct CanardRxState* next;

    CanardBufferBlock* buffer_blocks;

    uint64_t timestamp_usec;

    const uint32_t dtid_tt_snid_dnid;

    // We're using plain 'unsigned' here, because C99 doesn't permit explicit field type specification
    unsigned calculated_crc : 16;
    unsigned payload_len    : CANARD_TRANSFER_PAYLOAD_LEN_BITS;
    unsigned transfer_id    : 5;
    unsigned next_toggle    : 1;    // 16+10+5+1 = 32, aligned.

    uint16_t payload_crc;

    uint8_t buffer_head[];
};

/**
 * This is the core structure that keeps all of the states and allocated resources of the library instance.
 * The application should never access any of the fields directly! Instead, API functions should be used.
 */
struct CanardInstance
{
    uint8_t node_id;                                ///< Local node ID; may be zero if the node is anonymous

    CanardShouldAcceptTransfer should_accept;       ///< Function to decide whether the application wants this transfer
    CanardOnTransferReception on_reception;         ///< Function the library calls after RX transfer is complete

    CanardPoolAllocator allocator;                  ///< Pool allocator

    CanardRxState* rx_states;                       ///< RX transfer states
    CanardTxQueueItem* tx_queue;                    ///< TX frames awaiting transmission

    void* user_reference;                           ///< User pointer that can link this instance with other objects

#if CANARD_ENABLE_TAO_OPTION
    bool tao_disabled;                              ///< True if TAO is disabled
#endif
};

/**
 * This structure represents a received transfer for the application.
 * An instance of it is passed to the application via callback when the library receives a new transfer.
 * Pointers to the structure and all its fields are invalidated after the callback returns.
 */
struct CanardRxTransfer
{
    /**
     * Timestamp at which the first frame of this transfer was received.
     */
    uint64_t timestamp_usec;

    /**
     * Payload is scattered across three storages:
     *  - Head points to CanardRxState.buffer_head (length of which is up to CANARD_PAYLOAD_HEAD_SIZE), or to the
     *    payload field (possibly with offset) of the last received CAN frame.
     *
     *  - Middle is located in the linked list of dynamic blocks (only for multi-frame transfers).
     *
     *  - Tail points to the payload field (possibly with offset) of the last received CAN frame
     *    (only for multi-frame transfers).
     *
     * The tail offset depends on how much data of the last frame was accommodated in the last allocated block.
     *
     * For single-frame transfers, middle and tail will be NULL, and the head will point at first byte
     * of the payload of the CAN frame.
     *
     * In simple cases it should be possible to get data directly from the head and/or tail pointers.
     * Otherwise it is advised to use canardDecodeScalar().
     */
    const uint8_t* payload_head;            ///< Always valid, i.e. not NULL.
                                            ///< For multi frame transfers, the maximum size is defined in the constant
                                            ///< CANARD_MULTIFRAME_RX_PAYLOAD_HEAD_SIZE.
                                            ///< For single-frame transfers, the size is defined in the
                                            ///< field payload_len.
    CanardBufferBlock* payload_middle;      ///< May be NULL if the buffer was not needed. Always NULL for single-frame
                                            ///< transfers.
    const uint8_t* payload_tail;            ///< Last bytes of multi-frame transfers. Always NULL for single-frame
                                            ///< transfers.
    uint16_t payload_len;                   ///< Effective length of the payload in bytes.

    /**
     * These fields identify the transfer for the application.
     */
    uint16_t data_type_id;                  ///< 0 to 255 for services, 0 to 65535 for messages
    uint8_t transfer_type;                  ///< See CanardTransferType
    uint8_t transfer_id;                    ///< 0 to 31
    uint8_t priority;                       ///< 0 to 31
    uint8_t source_node_id;                 ///< 1 to 127, or 0 if the source is anonymous
#if CANARD_ENABLE_TAO_OPTION
    bool tao;
#endif
#if CANARD_ENABLE_CANFD
    bool canfd;                             ///< frame canfd
#endif
};

void canardInit(CanardInstance* out_ins, void* mem_arena, size_t mem_arena_size, CanardOnTransferReception on_reception, CanardShouldAcceptTransfer should_accept, void* user_reference);

int16_t canardBroadcast(CanardInstance* ins, uint64_t data_type_signature, uint16_t data_type_id, uint8_t* inout_transfer_id, uint8_t priority, const void* payload, uint16_t payload_len);


int16_t canardDecodeScalar(const CanardRxTransfer* transfer,    ///< The RX transfer where the data will be copied from
                           uint32_t bit_offset,                 ///< Offset, in bits, from the beginning of the transfer
                           uint8_t bit_length,                  ///< Length of the value, in bits; see the table
                           bool value_is_signed,                ///< True if the value can be negative; see the table
                           void* out_value);                    ///< Pointer to the output storage; see the table

void canardEncodeScalar(void* destination,
                        uint32_t bit_offset,
                        uint8_t bit_length,
                        const void* value);

float canardConvertFloat16ToNativeFloat(uint16_t value);

uint16_t canardConvertNativeFloatToFloat16(float value);