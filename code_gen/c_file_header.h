/**
 * @file trace_encoder.h
 * @brief FreeRTOS tracing event encoder
 * @warning This file is automatically generated. Do not edit.
 */
// clang-format off
#ifndef TRACE_ENCODER_H_
#define TRACE_ENCODER_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifndef traceconfigMAX_STR_LEN
  #define traceconfigMAX_STR_LEN (10)
#endif /* traceconfigMAX_STR_LEN */

// ==== COBS Framing ===========================================================

// COBS framing state
struct cobs_state {
  uint8_t *buf;    // Start of output buffer.
  size_t pos;      // Current location in output buffer.
  uint8_t code;    // Code value.
  size_t code_pos; // Code position.
  bool just_finished_full_block;
};

// Start COBS frame
static inline struct cobs_state cobs_start(uint8_t *out_buf) {
  struct cobs_state cobs = {
      .buf = out_buf,
      .pos = 1,      // Since first byte is COBS code byte, data starts at 1.
      .code = 1,     // First data byte is one away from first code byte.
      .code_pos = 0, // First COBS code byte is at 0.
      .just_finished_full_block = false,
  };
  return cobs;
}

// Add byte to COBS frame
static inline void cobs_add_byte(struct cobs_state *cobs, uint8_t byte) {

  cobs->just_finished_full_block = false;

  if (byte != 0) {
    // If this is a non-zero byte, add it to the output buffer:
    cobs->buf[cobs->pos] = byte;
    cobs->pos++;
    cobs->code++;
  }

  // If a block of 254 non-zero bytes is complete, restart the COBS coding:
  if (byte == 0 || cobs->code == 0xff) {

    if (byte != 0) {
      cobs->just_finished_full_block = true;
    }

    cobs->buf[cobs->code_pos] = cobs->code;
    cobs->code = 1;
    cobs->code_pos = cobs->pos;
    cobs->pos++;
  }
}

// Finish COBS frame. Returns final output size including trailing
// zero termination.
static inline size_t cobs_finish(struct cobs_state *cobs) {
  if (!cobs->just_finished_full_block) {
    cobs->buf[cobs->code_pos] = cobs->code; // Write last code byte
    cobs->buf[cobs->pos] = 0;               // Zero-terminate
    return cobs->pos + 1;                   // Return final buffer content length
  } else {
    cobs->buf[cobs->code_pos] = 0;
    return cobs->pos; // Return final buffer content length
  }
}

// Max length of a COBS frame with N data bytes and trailing delimiting zero
// added:
#define COBS_MAXLEN(N) ( (N) == 0 ? 2 : (1 + (((N) + 254 - 1)/254) + (N)) )

// ==== Field Encoding Functions ===============================================

static inline void encode_u8(struct cobs_state *cobs, uint8_t v) { cobs_add_byte(cobs, v); }

static inline void encode_u32(struct cobs_state *cobs, uint32_t v) {
  for (size_t i = 0; i < 5; i++) {
    uint8_t bits = v & 0x7F;
    v = v >> 7;
    if (v != 0) {
      cobs_add_byte(cobs, bits | 0x80);
    } else {
      cobs_add_byte(cobs, bits | 0x00);
      break;
    }
  }
}

static inline void encode_u64(struct cobs_state *cobs, uint64_t v) {
  for (size_t i = 0; i < 10; i++) {
    uint8_t bits = v & 0x7F;
    v = v >> 7;
    if (v != 0) {
      cobs_add_byte(cobs, bits | 0x80);
    } else {
      cobs_add_byte(cobs, bits | 0x00);
      break;
    }
  }
}

static inline void encode_str(struct cobs_state *cobs, char *str) {
  for (size_t i = 0; i <traceconfigMAX_STR_LEN; i++) {
    if (*str == 0) {
      break;
    }
    cobs_add_byte(cobs, (uint8_t)*str);
    str++;
  }
}
