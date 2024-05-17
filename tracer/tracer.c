/**
 * @file tracer.c
 * @brief FreeRTOS tracer
 * @author Philipp Schilk, 2024
 */

#include "FreeRTOS.h"
#if (traceconfigENABLE == 1)

// std:
#include <assert.h>
#include <stdatomic.h>
#include <stdbool.h>

// FreeRTOS:
#include "queue.h"
#include "stream_buffer.h"
#include "task.h"

// nanopb:
#include "FreeRTOS_trace.pb.h"
#include "pb_encode.h"

// ===== VALIDATION ============================================================

#if (configUSE_TRACE_FACILITY == 0)
#error "configUSE_TRACE_FACILITY is not enabled!"
#endif /* (configUSE_TRACE_FACILITY == 0) */

// ===== PRIVATE FUNCS =========================================================

static inline void handle_trace_evt(TraceEvent *evt, bool is_metadata);
static inline void cobs_frame(uint8_t *buf, size_t msg_len);

static inline uint32_t normalise_isr_id(int32_t isr_id) {
  if (isr_id < 0) {
    return traceportMAX_ISR_ID - isr_id;
  } else {
    return isr_id;
  }
}

static inline NameEvent name_event(uint32_t id, char *name) {

  static const size_t trace_max_name_len = pb_arraysize(NameEvent, name);

  NameEvent evt = {.id = id, .name = {0}};

  for (size_t i = 0; i < trace_max_name_len; i++) {
    evt.name[i] = name[i];
    if (name[i] == 0) {
      break;
    }
  }

  // Ensure string is terminated no matter what:
  evt.name[trace_max_name_len - 1] = 0;

  return evt;
}

// ===== STATE =================================================================

volatile atomic_uint dropped_evt_cnt = 0;
volatile atomic_uint dropped_evt_cnt_inclusion_cycle = 0;

volatile atomic_bool dropped_evt_cnt_has_increased = false;

volatile atomic_uint next_queue_id = 0;
volatile atomic_uint next_stream_buffer_id = 0;

static inline void include_dropped_evt_cnt(TraceEvent *evt) {
  evt->has_dropped_evts_cnt = false;

#if traceconfigTRACE_DROP_CNT_EVERY > 1
  // Include drop count if there was a new dropped event:
  bool new_dropped_evt = atomic_exchange(&dropped_evt_cnt_has_increased, false);
  if (new_dropped_evt) {
    evt->has_dropped_evts_cnt = true;
    evt->dropped_evts_cnt = dropped_evt_cnt;
  }

  // Include drop count if inclusion cycle lines up:
  unsigned int cnt = atomic_fetch_add(&dropped_evt_cnt_inclusion_cycle, 1);
  if (cnt % traceconfigTRACE_DROP_CNT_EVERY == 0) {
    evt->has_dropped_evts_cnt = true;
    evt->dropped_evts_cnt = dropped_evt_cnt;
  }
#else
  // Always include dropped count:
  evt->has_dropped_evts_cnt = true;
  evt->dropped_evts_cnt = dropped_evt_cnt;
#endif
}

// ===== TRACE HOOKS ===========================================================

void trace_task_switched_in(uint32_t task_id) {
  TraceEvent evt = {
      .ts_ns = traceportTIMESTAMP_NS(),
      .which_event = TraceEvent_task_switched_in_tag,
      .event.task_switched_in = task_id,
  };
  include_dropped_evt_cnt(&evt);
  handle_trace_evt(&evt, false);
}

void trace_moved_task_to_ready_state(uint32_t task_id) {
  TraceEvent evt = {
      .ts_ns = traceportTIMESTAMP_NS(),
      .which_event = TraceEvent_task_to_ready_state_tag,
      .event.task_to_ready_state = task_id,
  };
  include_dropped_evt_cnt(&evt);
  handle_trace_evt(&evt, false);
}

void trace_task_resume(uint32_t task_id) {
  TraceEvent evt = {
      .ts_ns = traceportTIMESTAMP_NS(),
      .which_event = TraceEvent_task_resumed_tag,
      .event.task_resumed = task_id,
  };
  include_dropped_evt_cnt(&evt);
  handle_trace_evt(&evt, false);
}

void trace_task_suspend(uint32_t task_id) {
  TraceEvent evt = {
      .ts_ns = traceportTIMESTAMP_NS(),
      .which_event = TraceEvent_task_suspended_tag,
      .event.task_suspended = task_id,
  };
  include_dropped_evt_cnt(&evt);
  handle_trace_evt(&evt, false);
}

void trace_task_delay(void) {
  TraceEvent evt = {
      .ts_ns = traceportTIMESTAMP_NS(),
      .which_event = TraceEvent_task_delay_tag,
      .event.task_delay = true, // Value is dont-care
  };
  include_dropped_evt_cnt(&evt);
  handle_trace_evt(&evt, false);
}

void trace_blocking_on_queue_peek(uint32_t queue_id) {
  TraceEvent evt = {
      .ts_ns = traceportTIMESTAMP_NS(),
      .which_event = TraceEvent_task_blocking_on_queue_peek_tag,
      .event.task_blocking_on_queue_peek = queue_id,
  };
  include_dropped_evt_cnt(&evt);
  handle_trace_evt(&evt, false);
}

void trace_blocking_on_queue_receive(uint32_t queue_id) {

  TraceEvent evt = {
      .ts_ns = traceportTIMESTAMP_NS(),
      .which_event = TraceEvent_task_blocking_on_queue_receive_tag,
      .event.task_blocking_on_queue_receive = queue_id,
  };
  include_dropped_evt_cnt(&evt);
  handle_trace_evt(&evt, false);
}

void trace_blocking_on_queue_send(uint32_t queue_id) {

  TraceEvent evt = {
      .ts_ns = traceportTIMESTAMP_NS(),
      .which_event = TraceEvent_task_blocking_on_queue_send_tag,
      .event.task_blocking_on_queue_send = queue_id,
  };
  include_dropped_evt_cnt(&evt);
  handle_trace_evt(&evt, false);
}

void trace_blocking_on_stream_buffer_receive(uint32_t sb_id) {

  TraceEvent evt = {
      .ts_ns = traceportTIMESTAMP_NS(),
      .which_event = TraceEvent_task_blocking_on_sb_receive_tag,
      .event.task_blocking_on_sb_receive = sb_id,
  };
  include_dropped_evt_cnt(&evt);
  handle_trace_evt(&evt, false);
}

void trace_blocking_on_stream_buffer_send(uint32_t sb_id) {
  TraceEvent evt = {
      .ts_ns = traceportTIMESTAMP_NS(),
      .which_event = TraceEvent_task_blocking_on_sb_send_tag,
      .event.task_blocking_on_sb_send = sb_id,
  };
  include_dropped_evt_cnt(&evt);
  handle_trace_evt(&evt, false);
}

// #### Tasks Priority Changes ####

void trace_task_priority_inherit(uint32_t task_id, uint32_t priority) {
  TraceEvent evt = {
      .ts_ns = traceportTIMESTAMP_NS(),
      .which_event = TraceEvent_task_priority_inherit_tag,
      .event.task_priority_inherit = {.task_id = task_id, .new_priority = priority},
  };
  include_dropped_evt_cnt(&evt);
  handle_trace_evt(&evt, false);
}

void trace_task_priority_disinherit(uint32_t task_id, uint32_t priority) {
  TraceEvent evt = {
      .ts_ns = traceportTIMESTAMP_NS(),
      .which_event = TraceEvent_task_priority_disinherit_tag,
      .event.task_priority_disinherit = {.task_id = task_id, .new_priority = priority},
  };
  include_dropped_evt_cnt(&evt);
  handle_trace_evt(&evt, false);
}

void trace_task_priority_set(uint32_t task_id, uint32_t priority) {
  TraceEvent evt = {
      .ts_ns = traceportTIMESTAMP_NS(),
      .which_event = TraceEvent_task_priority_set_tag,
      .event.task_priority_set = {.task_id = task_id, .new_priority = priority},
  };
  include_dropped_evt_cnt(&evt);
  handle_trace_evt(&evt, false);
}

// #### Tasks Creation / Destruction ####

void trace_task_create(uint32_t task_id, uint32_t priority, char *name) {

  // Submit 'task created` event:
  TraceEvent evt1 = {
      .ts_ns = traceportTIMESTAMP_NS(),
      .which_event = TraceEvent_task_create_tag,
      .event.task_create = task_id,
  };
  include_dropped_evt_cnt(&evt1);
  handle_trace_evt(&evt1, false);

  // Submit task metadata:
  TraceEvent evt2 = {
      .ts_ns = traceportTIMESTAMP_NS(),
      .which_event = TraceEvent_task_name_tag,
      .event.task_name = name_event(task_id, name),
  };
  include_dropped_evt_cnt(&evt2);
  handle_trace_evt(&evt2, true);

  // Submit task priority:
  TraceEvent evt3 = {
      .ts_ns = traceportTIMESTAMP_NS(),
      .which_event = TraceEvent_task_priority_set_tag,
      .event.task_priority_set = {.task_id = task_id, .new_priority = priority},
  };
  include_dropped_evt_cnt(&evt3);
  handle_trace_evt(&evt3, true);
}

void trace_task_delete(uint32_t task_id) {
  TraceEvent evt = {
      .ts_ns = traceportTIMESTAMP_NS(),
      .which_event = TraceEvent_task_deleted_tag,
      .event.task_deleted = task_id,
  };
  include_dropped_evt_cnt(&evt);
  handle_trace_evt(&evt, false);
}

// #### ISRs ####

void impl_trace_isr_enter(int32_t isr_id) {
  if (xTaskGetSchedulerState() != taskSCHEDULER_RUNNING) return;

  TraceEvent evt = {
      .ts_ns = traceportTIMESTAMP_NS(),
      .which_event = TraceEvent_isr_enter_tag,
      .event.isr_enter = normalise_isr_id(isr_id),
  };
  // We don't include the dropped count to avoid slowing ISRs
  // unecessarily.
  evt.has_dropped_evts_cnt = false;
  handle_trace_evt(&evt, false);
}

void impl_trace_isr_exit(int32_t isr_id) {
  if (xTaskGetSchedulerState() != taskSCHEDULER_RUNNING) return;

  TraceEvent evt = {
      .ts_ns = traceportTIMESTAMP_NS(),
      .which_event = TraceEvent_isr_exit_tag,
      .event.isr_exit = normalise_isr_id(isr_id),
  };
  // We don't include the dropped count to avoid slowing ISRs
  // unecessarily.
  evt.has_dropped_evts_cnt = false;
  handle_trace_evt(&evt, false);
}

void impl_trace_isr_name(int32_t isr_id, char *name) {
  TraceEvent evt = {
      .ts_ns = traceportTIMESTAMP_NS(),
      .which_event = TraceEvent_isr_name_tag,
      .event.isr_name = name_event(normalise_isr_id(isr_id), name),
  };
  include_dropped_evt_cnt(&evt);
  handle_trace_evt(&evt, true);
}

// #### Queues (and other basic RTOS resources) ####

void trace_queue_create(void *handle, uint8_t type_val) {

  QueueKind kind;

  QueueHandle_t queue = (QueueHandle_t)handle;

  switch (type_val) {
    case queueQUEUE_TYPE_BASE:
      kind = QueueKind_QK_QUEUE;
      break;
    case queueQUEUE_TYPE_MUTEX:
      kind = QueueKind_QK_MUTEX;
      break;
    case queueQUEUE_TYPE_COUNTING_SEMAPHORE:
      kind = QueueKind_QK_COUNTING_SEMAPHORE;
      break;
    case queueQUEUE_TYPE_BINARY_SEMAPHORE:
      kind = QueueKind_QK_BINARY_SEMAPHORE;
      break;
    case queueQUEUE_TYPE_RECURSIVE_MUTEX:
      kind = QueueKind_QK_RECURSIVE_MUTEX;
      break;
    default:
      kind = QueueKind_QK_QUEUE;
      break;
  }

  uint32_t id = (uint32_t)atomic_fetch_add(&next_queue_id, 1);
  vQueueSetQueueNumber(queue, (UBaseType_t)id);

  // Submit created event:
  TraceEvent evt1 = {
      .ts_ns = traceportTIMESTAMP_NS(),
      .which_event = TraceEvent_queue_create_tag,
      .event.queue_create = id,
  };
  include_dropped_evt_cnt(&evt1);
  handle_trace_evt(&evt1, false);

  // Submit metadata:
  TraceEvent evt2 = {
      .ts_ns = traceportTIMESTAMP_NS(),
      .which_event = TraceEvent_queue_kind_tag,
      .event.queue_kind = {.id = id, .kind = kind},
  };
  include_dropped_evt_cnt(&evt2);
  handle_trace_evt(&evt2, true);
}

void impl_trace_queue_name(void *queue_handle, char *name) {
  uint32_t id = (uint32_t)uxQueueGetQueueNumber((QueueHandle_t)queue_handle);
  TraceEvent evt = {
      .ts_ns = traceportTIMESTAMP_NS(),
      .which_event = TraceEvent_queue_name_tag,
      .event.queue_name = name_event(id, name),

  };
  include_dropped_evt_cnt(&evt);
  handle_trace_evt(&evt, true);
}

void trace_queue_send(uint32_t id) {
  TraceEvent evt = {
      .ts_ns = traceportTIMESTAMP_NS(),
      .which_event = TraceEvent_queue_send_tag,
      .event.queue_send = id,
  };
  include_dropped_evt_cnt(&evt);
  handle_trace_evt(&evt, false);
}

void trace_queue_receive(uint32_t id) {
  TraceEvent evt = {
      .ts_ns = traceportTIMESTAMP_NS(),
      .which_event = TraceEvent_queue_receive_tag,
      .event.queue_receive = id,
  };
  include_dropped_evt_cnt(&evt);
  handle_trace_evt(&evt, false);
}

// #### Stream Buffers ####

void trace_stream_buffer_create(void *handle, int is_message_buffer) {

  StreamBufferHandle_t sb = (StreamBufferHandle_t)handle;

  uint32_t id = (uint32_t)atomic_fetch_add(&next_stream_buffer_id, 1);
  vStreamBufferSetStreamBufferNumber(sb, (UBaseType_t)id);

  // Submit stream buffer created event:
  TraceEvent evt1 = {
      .ts_ns = traceportTIMESTAMP_NS(),
      .which_event = TraceEvent_stream_buffer_create_tag,
      .event.stream_buffer_create = id,
  };
  include_dropped_evt_cnt(&evt1);
  handle_trace_evt(&evt1, false);

  StreamBufferKind kind =
      is_message_buffer ? StreamBufferKind_SB_MESSAGE_BUFFER : StreamBufferKind_SB_NORMAL;

  // Submit metadata:
  TraceEvent evt2 = {.ts_ns = traceportTIMESTAMP_NS(),
                     .which_event = TraceEvent_stream_buffer_kind_tag,
                     .event.stream_buffer_kind = {.id = id, .kind = kind}};
  include_dropped_evt_cnt(&evt2);
  handle_trace_evt(&evt2, true);
}

void impl_trace_stream_buffer_name(void *handle, char *name) {
  uint32_t id = (uint32_t)uxStreamBufferGetStreamBufferNumber((StreamBufferHandle_t)handle);
  TraceEvent evt = {
      .ts_ns = traceportTIMESTAMP_NS(),
      .which_event = TraceEvent_stream_buffer_name_tag,
      .event.stream_buffer_name = name_event(id, name),
  };
  include_dropped_evt_cnt(&evt);
  handle_trace_evt(&evt, true);
}

void trace_stream_buffer_send(uint32_t id, uint32_t len) {
  TraceEvent evt = {
      .ts_ns = traceportTIMESTAMP_NS(),
      .which_event = TraceEvent_stream_buffer_send_tag,
      .event.stream_buffer_send = {.id = id, .amnt = len},
  };
  include_dropped_evt_cnt(&evt);
  handle_trace_evt(&evt, false);
}

void trace_stream_buffer_receive(uint32_t id, uint32_t len) {
  TraceEvent evt = {
      .ts_ns = traceportTIMESTAMP_NS(),
      .which_event = TraceEvent_stream_buffer_receive_tag,
      .event.stream_buffer_receive = {.id = id, .amnt = len},
  };
  include_dropped_evt_cnt(&evt);
  handle_trace_evt(&evt, false);
}

// ===== Trace Handling ========================================================

static_assert(TraceEvent_size < 245,
              "Trace Event too large for fast simplistics COBS framing used.");

inline static void handle_trace_evt(TraceEvent *evt, bool is_metadata) {

  // Output buffer.
  // COBS framing will always require exactly two bytes of overhead since we
  // restrict  events to be less than 254 bytes.
  uint8_t evt_bytes[TraceEvent_size + 2] = {0};

  // Nanopb serialisation into buffer starting at second byte, since the first
  // will contain the COBS overhead byte/pointer.
  pb_ostream_t ostream = pb_ostream_from_buffer(evt_bytes + 1, TraceEvent_size);
  (void)pb_encode(&ostream, TraceEvent_fields, evt);

  size_t proto_len = ostream.bytes_written;

  cobs_frame(evt_bytes, proto_len);

  // Submit to port:
  bool did_drop_evt = traceportHANDLE_RAW_EVT(evt_bytes, proto_len + 2, is_metadata);

  // Handle dropped evt:
  if (did_drop_evt) {
    dropped_evt_cnt += 1;
#if traceconfigTRACE_DROP_CNT_EVERY > 1
    dropped_evt_cnt_has_increased = true;
#else
    // don't waste time accessing atomic var if emitting drop count in
    // every event.
#endif
  }
}

static inline void cobs_frame(uint8_t *buf, size_t msg_len) {

  size_t buf_len = msg_len + 2;

  // COBS: Add zero termination:
  buf[buf_len - 1] = 0;

  // COBS: Iterate backwards from last non-termination byte and assign all zero pointers:
  size_t last_seen_zero_at = buf_len - 1;
  for (size_t i = 1; i < buf_len; i++) {
    size_t pos = buf_len - i - 1;
    if (buf[pos] == 0) {
      buf[pos] = last_seen_zero_at - pos;
      last_seen_zero_at = pos;
    }
  }
}

#endif /* traceconfigENABLE == 1*/