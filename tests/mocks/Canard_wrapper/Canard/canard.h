#pragma once
#include <cstddef>
#include <cstdint>

// /// Libcanard version. API will be backwards compatible within the same major version.
// #define CANARD_VERSION_MAJOR                        0
// #define CANARD_VERSION_MINOR                        2

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
    uint8_t* payload_middle;      ///< May be NULL if the buffer was not needed. Always NULL for single-frame
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

#include <common_to_all_mocks.h>

inline void canardInit(CanardInstance* out_ins, void* mem_arena, size_t mem_arena_size, CanardOnTransferReception on_reception, CanardShouldAcceptTransfer should_accept, void* user_reference) {
    mock().actualCall("canardInit")
          .withUnsignedLongLongIntParameter("mem_arena_size", mem_arena_size);

}

inline void canardSetLocalNodeID(CanardInstance* ins, uint8_t self_node_id) {
    mock().actualCall("canardSetLocalNodeID")
          .withUnsignedIntParameter("self_node_id", self_node_id);
}

inline int16_t canardBroadcast(CanardInstance* ins, uint64_t data_type_signature, uint16_t data_type_id, uint8_t* inout_transfer_id, uint8_t priority, const void* payload, uint16_t payload_len) {
    mock().actualCall("canardBroadcast")
          .withUnsignedLongLongIntParameter("data_type_signature", data_type_signature)
          .withUnsignedIntParameter("data_type_id", data_type_id)
          .withUnsignedIntParameter("priority", priority)
          .withPointerParameter("payload", (void*)payload)
          .withUnsignedIntParameter("payload_len", payload_len);
    return mock().returnIntValueOrDefault(0);
}

inline int16_t canardRequestOrRespond(CanardInstance* ins, uint8_t destination_node_id, uint64_t data_type_signature, uint8_t data_type_id, uint8_t* inout_transfer_id, uint8_t priority, CanardRequestResponse kind, const void* payload, uint16_t payload_lens) {
    mock().actualCall("canardRequestOrRespond")
          .withUnsignedIntParameter("destination_node_id", destination_node_id)
          .withUnsignedLongLongIntParameter("data_type_signature", data_type_signature)
          .withUnsignedIntParameter("data_type_id", data_type_id)
          .withUnsignedIntParameter("priority", priority)
          .withPointerParameter("payload", (void*)payload)
          .withUnsignedIntParameter("payload_len", payload_lens);
    return mock().returnIntValueOrDefault(0);
}

inline const CanardCANFrame* canardPeekTxQueue(const CanardInstance* ins) {
    mock().actualCall("canardPeekTxQueue");
    return (CanardCANFrame*)mock().returnPointerValueOrDefault(nullptr);
}

inline void canardPopTxQueue(CanardInstance* ins) {
    mock().actualCall("canardPopTxQueue");
}

inline int16_t canardHandleRxFrame(CanardInstance* ins, CanardCANFrame* frame, uint64_t timestamp_usec) {
    mock().actualCall("canardHandleRxFrame")
          .withParameterOfType("CanardCANFrame", "frame", (void*)frame)
          .withUnsignedLongLongIntParameter("timestamp_usec", timestamp_usec);
    return mock().returnIntValueOrDefault(0);
}

inline void canardReleaseRxTransferPayload(CanardInstance* ins, CanardRxTransfer* transfer) {
    mock().actualCall("canardReleaseRxTransferPayload")
          .withPointerParameter("transfer", (void*)transfer);
}

inline void canardCleanupStaleTransfers(CanardInstance* ins, uint64_t current_time_usec) {
    mock().actualCall("canardCleanupStaleTransfers")
          .withUnsignedLongLongIntParameter("current_time_usec", current_time_usec);
}

inline CanardPoolAllocatorStatistics canardGetPoolAllocatorStatistics(CanardInstance* ins) {
    CanardPoolAllocatorStatistics statistics{};
    return statistics;
} //DEBUG PURPOSE