/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.8 */

#ifndef PB_FREERTOS_TRACE_PB_H_INCLUDED
#define PB_FREERTOS_TRACE_PB_H_INCLUDED
#include <pb.h>

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Enum definitions */
typedef enum _QueueKind {
    QueueKind_QK_QUEUE = 0,
    QueueKind_QK_COUNTING_SEMAPHORE = 1,
    QueueKind_QK_BINARY_SEMAPHORE = 2,
    QueueKind_QK_MUTEX = 3,
    QueueKind_QK_RECURSIVE_MUTEX = 4
} QueueKind;

typedef enum _StreamBufferKind {
    StreamBufferKind_SB_NORMAL = 0,
    StreamBufferKind_SB_MESSAGE_BUFFER = 1
} StreamBufferKind;

/* Struct definitions */
typedef struct _TaskPriorityEvent {
    uint32_t task_id;
    uint32_t new_priority;
} TaskPriorityEvent;

typedef struct _NameEvent {
    uint32_t id;
    char name[10];
} NameEvent;

typedef struct _QueueKindEvent {
    uint32_t id;
    QueueKind kind;
} QueueKindEvent;

typedef struct _StreamBufferKindEvent {
    uint32_t id;
    StreamBufferKind kind;
} StreamBufferKindEvent;

typedef struct _StreamBufferTransferEvent {
    uint32_t id;
    uint32_t amnt;
} StreamBufferTransferEvent;

typedef struct _TraceEvent { /* Note: Use tags up to 15 for most common events, as they encode to 1 byte. */
    uint64_t ts_ns;
    pb_size_t which_event;
    union {
        /* A task has been allowed to run. Value is task id. */
        uint32_t task_switched_in;
        /* A task has been put into the ready state. Value is task id. */
        uint32_t task_to_ready_state;
        /* ISR entered ISR. Value is ISR id. */
        uint32_t isr_enter;
        /* Exited ISR. Value is ISR id. */
        uint32_t isr_exit;
        /* A task has been resumed, putting it into the ready state. Value is task
     id. */
        uint32_t task_resumed;
        /* A task has been suspended, putting it into the suspended state. Value is
     task id. */
        uint32_t task_suspended;
        /* ISR name received. */
        NameEvent isr_name;
        /* Current task about to block on delay. Value is do-not-care. */
        bool task_delay;
        /* Task priority has been manually updated. */
        TaskPriorityEvent task_priority_set;
        /* Task priority temporarily increased because it is holding a mutex. */
        TaskPriorityEvent task_priority_inherit;
        /* Task priority decreased to original value because it relased mutex. */
        TaskPriorityEvent task_priority_disinherit;
        /* A new task has been created. Value is task id. */
        uint32_t task_create;
        /* Task name information. */
        NameEvent task_name;
        /* Identify task as FreeRTOS idle task. Value is task id. */
        uint32_t task_is_idle_task;
        /* Identify task as FreeRTOS timer task. Value is task id. */
        uint32_t task_is_timer_task;
        /* A task has been deleted. Value is task id. */
        uint32_t task_deleted;
        /* Queue (which could bequeue, mutex, semaphore) created. Value is queue id. */
        uint32_t queue_create;
        /* Queue named. */
        NameEvent queue_name;
        /* Queue kind specified. */
        QueueKindEvent queue_kind;
        /* Sent to queue. Value is queue id. */
        uint32_t queue_send;
        /* Overwritten to queue. Value is queue id. */
        uint32_t queue_overwrite;
        /* Receive from queue. Value is queue id. */
        uint32_t queue_receive;
        /* Receive from queue. Value is queue id. */
        uint32_t queue_reset;
        /* Current task about to block on queue peek. Value is queue id. */
        uint32_t task_blocking_on_queue_peek;
        /* uint32_t block on queue send. Value is queue id. */
        uint32_t task_blocking_on_queue_send;
        /* uint32_t block on queue receive. Value is queue id. */
        uint32_t task_blocking_on_queue_receive;
        /* Stream buffer created. Value is stream buffer id. */
        uint32_t stream_buffer_create;
        /* Stream buffer named. */
        NameEvent stream_buffer_name;
        /* Stream buffer named. */
        StreamBufferKindEvent stream_buffer_kind;
        /* Receive from stream buffer. */
        StreamBufferTransferEvent stream_buffer_receive;
        /* Send to stream buffer. */
        StreamBufferTransferEvent stream_buffer_send;
        /* Stream buffer reset. Value is stream buffer id. */
        uint32_t stream_buffer_reset;
        /* Current task about to block on stream buffer send. Value is sb id. */
        uint32_t task_blocking_on_sb_send;
        /* Current task about to block on stream buffer receive. Value is sb id. */
        uint32_t task_blocking_on_sb_receive;
    } event;
} TraceEvent;


#ifdef __cplusplus
extern "C" {
#endif

/* Helper constants for enums */
#define _QueueKind_MIN QueueKind_QK_QUEUE
#define _QueueKind_MAX QueueKind_QK_RECURSIVE_MUTEX
#define _QueueKind_ARRAYSIZE ((QueueKind)(QueueKind_QK_RECURSIVE_MUTEX+1))

#define _StreamBufferKind_MIN StreamBufferKind_SB_NORMAL
#define _StreamBufferKind_MAX StreamBufferKind_SB_MESSAGE_BUFFER
#define _StreamBufferKind_ARRAYSIZE ((StreamBufferKind)(StreamBufferKind_SB_MESSAGE_BUFFER+1))




#define QueueKindEvent_kind_ENUMTYPE QueueKind

#define StreamBufferKindEvent_kind_ENUMTYPE StreamBufferKind



/* Initializer values for message structs */
#define TraceEvent_init_default                  {0, 0, {0}}
#define TaskPriorityEvent_init_default           {0, 0}
#define NameEvent_init_default                   {0, ""}
#define QueueKindEvent_init_default              {0, _QueueKind_MIN}
#define StreamBufferKindEvent_init_default       {0, _StreamBufferKind_MIN}
#define StreamBufferTransferEvent_init_default   {0, 0}
#define TraceEvent_init_zero                     {0, 0, {0}}
#define TaskPriorityEvent_init_zero              {0, 0}
#define NameEvent_init_zero                      {0, ""}
#define QueueKindEvent_init_zero                 {0, _QueueKind_MIN}
#define StreamBufferKindEvent_init_zero          {0, _StreamBufferKind_MIN}
#define StreamBufferTransferEvent_init_zero      {0, 0}

/* Field tags (for use in manual encoding/decoding) */
#define TaskPriorityEvent_task_id_tag            1
#define TaskPriorityEvent_new_priority_tag       2
#define NameEvent_id_tag                         1
#define NameEvent_name_tag                       2
#define QueueKindEvent_id_tag                    1
#define QueueKindEvent_kind_tag                  2
#define StreamBufferKindEvent_id_tag             1
#define StreamBufferKindEvent_kind_tag           2
#define StreamBufferTransferEvent_id_tag         1
#define StreamBufferTransferEvent_amnt_tag       2
#define TraceEvent_ts_ns_tag                     1
#define TraceEvent_task_switched_in_tag          3
#define TraceEvent_task_to_ready_state_tag       4
#define TraceEvent_isr_enter_tag                 5
#define TraceEvent_isr_exit_tag                  6
#define TraceEvent_task_resumed_tag              16
#define TraceEvent_task_suspended_tag            17
#define TraceEvent_isr_name_tag                  18
#define TraceEvent_task_delay_tag                19
#define TraceEvent_task_priority_set_tag         20
#define TraceEvent_task_priority_inherit_tag     21
#define TraceEvent_task_priority_disinherit_tag  22
#define TraceEvent_task_create_tag               23
#define TraceEvent_task_name_tag                 24
#define TraceEvent_task_is_idle_task_tag         25
#define TraceEvent_task_is_timer_task_tag        26
#define TraceEvent_task_deleted_tag              27
#define TraceEvent_queue_create_tag              28
#define TraceEvent_queue_name_tag                29
#define TraceEvent_queue_kind_tag                30
#define TraceEvent_queue_send_tag                31
#define TraceEvent_queue_overwrite_tag           32
#define TraceEvent_queue_receive_tag             33
#define TraceEvent_queue_reset_tag               34
#define TraceEvent_task_blocking_on_queue_peek_tag 35
#define TraceEvent_task_blocking_on_queue_send_tag 36
#define TraceEvent_task_blocking_on_queue_receive_tag 37
#define TraceEvent_stream_buffer_create_tag      38
#define TraceEvent_stream_buffer_name_tag        39
#define TraceEvent_stream_buffer_kind_tag        40
#define TraceEvent_stream_buffer_receive_tag     41
#define TraceEvent_stream_buffer_send_tag        42
#define TraceEvent_stream_buffer_reset_tag       43
#define TraceEvent_task_blocking_on_sb_send_tag  44
#define TraceEvent_task_blocking_on_sb_receive_tag 45

/* Struct field encoding specification for nanopb */
#define TraceEvent_FIELDLIST(X, a) \
X(a, STATIC,   REQUIRED, UINT64,   ts_ns,             1) \
X(a, STATIC,   ONEOF,    UINT32,   (event,task_switched_in,event.task_switched_in),   3) \
X(a, STATIC,   ONEOF,    UINT32,   (event,task_to_ready_state,event.task_to_ready_state),   4) \
X(a, STATIC,   ONEOF,    UINT32,   (event,isr_enter,event.isr_enter),   5) \
X(a, STATIC,   ONEOF,    UINT32,   (event,isr_exit,event.isr_exit),   6) \
X(a, STATIC,   ONEOF,    UINT32,   (event,task_resumed,event.task_resumed),  16) \
X(a, STATIC,   ONEOF,    UINT32,   (event,task_suspended,event.task_suspended),  17) \
X(a, STATIC,   ONEOF,    MESSAGE,  (event,isr_name,event.isr_name),  18) \
X(a, STATIC,   ONEOF,    BOOL,     (event,task_delay,event.task_delay),  19) \
X(a, STATIC,   ONEOF,    MESSAGE,  (event,task_priority_set,event.task_priority_set),  20) \
X(a, STATIC,   ONEOF,    MESSAGE,  (event,task_priority_inherit,event.task_priority_inherit),  21) \
X(a, STATIC,   ONEOF,    MESSAGE,  (event,task_priority_disinherit,event.task_priority_disinherit),  22) \
X(a, STATIC,   ONEOF,    UINT32,   (event,task_create,event.task_create),  23) \
X(a, STATIC,   ONEOF,    MESSAGE,  (event,task_name,event.task_name),  24) \
X(a, STATIC,   ONEOF,    UINT32,   (event,task_is_idle_task,event.task_is_idle_task),  25) \
X(a, STATIC,   ONEOF,    UINT32,   (event,task_is_timer_task,event.task_is_timer_task),  26) \
X(a, STATIC,   ONEOF,    UINT32,   (event,task_deleted,event.task_deleted),  27) \
X(a, STATIC,   ONEOF,    UINT32,   (event,queue_create,event.queue_create),  28) \
X(a, STATIC,   ONEOF,    MESSAGE,  (event,queue_name,event.queue_name),  29) \
X(a, STATIC,   ONEOF,    MESSAGE,  (event,queue_kind,event.queue_kind),  30) \
X(a, STATIC,   ONEOF,    UINT32,   (event,queue_send,event.queue_send),  31) \
X(a, STATIC,   ONEOF,    UINT32,   (event,queue_overwrite,event.queue_overwrite),  32) \
X(a, STATIC,   ONEOF,    UINT32,   (event,queue_receive,event.queue_receive),  33) \
X(a, STATIC,   ONEOF,    UINT32,   (event,queue_reset,event.queue_reset),  34) \
X(a, STATIC,   ONEOF,    UINT32,   (event,task_blocking_on_queue_peek,event.task_blocking_on_queue_peek),  35) \
X(a, STATIC,   ONEOF,    UINT32,   (event,task_blocking_on_queue_send,event.task_blocking_on_queue_send),  36) \
X(a, STATIC,   ONEOF,    UINT32,   (event,task_blocking_on_queue_receive,event.task_blocking_on_queue_receive),  37) \
X(a, STATIC,   ONEOF,    UINT32,   (event,stream_buffer_create,event.stream_buffer_create),  38) \
X(a, STATIC,   ONEOF,    MESSAGE,  (event,stream_buffer_name,event.stream_buffer_name),  39) \
X(a, STATIC,   ONEOF,    MESSAGE,  (event,stream_buffer_kind,event.stream_buffer_kind),  40) \
X(a, STATIC,   ONEOF,    MESSAGE,  (event,stream_buffer_receive,event.stream_buffer_receive),  41) \
X(a, STATIC,   ONEOF,    MESSAGE,  (event,stream_buffer_send,event.stream_buffer_send),  42) \
X(a, STATIC,   ONEOF,    UINT32,   (event,stream_buffer_reset,event.stream_buffer_reset),  43) \
X(a, STATIC,   ONEOF,    UINT32,   (event,task_blocking_on_sb_send,event.task_blocking_on_sb_send),  44) \
X(a, STATIC,   ONEOF,    UINT32,   (event,task_blocking_on_sb_receive,event.task_blocking_on_sb_receive),  45)
#define TraceEvent_CALLBACK NULL
#define TraceEvent_DEFAULT NULL
#define TraceEvent_event_isr_name_MSGTYPE NameEvent
#define TraceEvent_event_task_priority_set_MSGTYPE TaskPriorityEvent
#define TraceEvent_event_task_priority_inherit_MSGTYPE TaskPriorityEvent
#define TraceEvent_event_task_priority_disinherit_MSGTYPE TaskPriorityEvent
#define TraceEvent_event_task_name_MSGTYPE NameEvent
#define TraceEvent_event_queue_name_MSGTYPE NameEvent
#define TraceEvent_event_queue_kind_MSGTYPE QueueKindEvent
#define TraceEvent_event_stream_buffer_name_MSGTYPE NameEvent
#define TraceEvent_event_stream_buffer_kind_MSGTYPE StreamBufferKindEvent
#define TraceEvent_event_stream_buffer_receive_MSGTYPE StreamBufferTransferEvent
#define TraceEvent_event_stream_buffer_send_MSGTYPE StreamBufferTransferEvent

#define TaskPriorityEvent_FIELDLIST(X, a) \
X(a, STATIC,   REQUIRED, UINT32,   task_id,           1) \
X(a, STATIC,   REQUIRED, UINT32,   new_priority,      2)
#define TaskPriorityEvent_CALLBACK NULL
#define TaskPriorityEvent_DEFAULT NULL

#define NameEvent_FIELDLIST(X, a) \
X(a, STATIC,   REQUIRED, UINT32,   id,                1) \
X(a, STATIC,   REQUIRED, STRING,   name,              2)
#define NameEvent_CALLBACK NULL
#define NameEvent_DEFAULT NULL

#define QueueKindEvent_FIELDLIST(X, a) \
X(a, STATIC,   REQUIRED, UINT32,   id,                1) \
X(a, STATIC,   REQUIRED, UENUM,    kind,              2)
#define QueueKindEvent_CALLBACK NULL
#define QueueKindEvent_DEFAULT NULL

#define StreamBufferKindEvent_FIELDLIST(X, a) \
X(a, STATIC,   REQUIRED, UINT32,   id,                1) \
X(a, STATIC,   REQUIRED, UENUM,    kind,              2)
#define StreamBufferKindEvent_CALLBACK NULL
#define StreamBufferKindEvent_DEFAULT NULL

#define StreamBufferTransferEvent_FIELDLIST(X, a) \
X(a, STATIC,   REQUIRED, UINT32,   id,                1) \
X(a, STATIC,   REQUIRED, UINT32,   amnt,              2)
#define StreamBufferTransferEvent_CALLBACK NULL
#define StreamBufferTransferEvent_DEFAULT NULL

extern const pb_msgdesc_t TraceEvent_msg;
extern const pb_msgdesc_t TaskPriorityEvent_msg;
extern const pb_msgdesc_t NameEvent_msg;
extern const pb_msgdesc_t QueueKindEvent_msg;
extern const pb_msgdesc_t StreamBufferKindEvent_msg;
extern const pb_msgdesc_t StreamBufferTransferEvent_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define TraceEvent_fields &TraceEvent_msg
#define TaskPriorityEvent_fields &TaskPriorityEvent_msg
#define NameEvent_fields &NameEvent_msg
#define QueueKindEvent_fields &QueueKindEvent_msg
#define StreamBufferKindEvent_fields &StreamBufferKindEvent_msg
#define StreamBufferTransferEvent_fields &StreamBufferTransferEvent_msg

/* Maximum encoded size of messages (where known) */
#define FREERTOS_TRACE_PB_H_MAX_SIZE             TraceEvent_size
#define NameEvent_size                           17
#define QueueKindEvent_size                      8
#define StreamBufferKindEvent_size               8
#define StreamBufferTransferEvent_size           12
#define TaskPriorityEvent_size                   12
#define TraceEvent_size                          31

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
