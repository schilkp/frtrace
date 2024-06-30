#include <string.h>

#include "unity.h"
#include "unity_internals.h"

#include "tband.h"
#define tbandPROPER_INTERNAL_INCLUDE
#include "tband_encode.h"
#undef tbandPROPER_INTERNAL_INCLUDE

void util_print_array(uint8_t *in, size_t in_len, char *name) {
  printf("%s: ", name);
  for (size_t i = 0; i < in_len; i++) {
    printf("0x%02x ", in[i]);
  }
  printf("\r\n");
}

void util_cobs_test(uint8_t *in, size_t in_len, uint8_t *expect, size_t expect_len) {
  uint8_t outbuf[expect_len * 2];
  memset(outbuf, 0xAB, expect_len * 2);

  struct cobs_state cobs = cobs_start(outbuf);
  for (size_t i = 0; i < in_len; i++) {
    cobs_add_byte(&cobs, in[i]);
  }
  size_t len = cobs_finish(&cobs);
  util_print_array(in, in_len, "INP");
  util_print_array(expect, expect_len, "EXP");
  util_print_array(outbuf, len, "OUT");

  TEST_ASSERT_EQUAL_INT(expect_len, len);
  TEST_ASSERT_EQUAL_UINT8_ARRAY(expect, outbuf, expect_len);
}

// ======== Tests ==================================================================================

static void test_cobs_nonzero_len_1(void) {
  uint8_t inp[] = {0x01};
  uint8_t outpt[] = {0x02, 0x01, 0x00};
  util_cobs_test(inp, sizeof(inp), outpt, sizeof(outpt));
}

static void test_cobs_zero_len_1(void) {
  uint8_t inp[] = {0x00};
  uint8_t outpt[] = {0x01, 0x01, 0x00};
  util_cobs_test(inp, sizeof(inp), outpt, sizeof(outpt));
}

static void test_cobs_len_zero(void) {
  uint8_t outpt[] = {0x01, 0x00};
  util_cobs_test(0, 0, outpt, sizeof(outpt));
}

static void test_cobs_short_with_zeros(void) {
  uint8_t inp[] = {0x01, 0x02, 0x00, 0x03};
  uint8_t outpt[] = {0x03, 0x01, 0x02, 0x02, 0x03, 0x00};
  util_cobs_test(inp, sizeof(inp), outpt, sizeof(outpt));
}

static void test_cobs_short_zeroes(void) {
  uint8_t inp[] = {0x00, 0x00, 0x00, 0x00};
  uint8_t outpt[] = {0x01, 0x01, 0x01, 0x01, 0x01, 0x00};
  util_cobs_test(inp, sizeof(inp), outpt, sizeof(outpt));
}

static void test_cobs_restarts(void) {
  uint8_t inp[] = {0x01, 0x01, 0x00, 0x02, 0x02, 0x03, 0x00};
  uint8_t outpt[] = {0x03, 0x01, 0x01, 0x4, 0x02, 0x02, 0x03, 0x01, 00};
  util_cobs_test(inp, sizeof(inp), outpt, sizeof(outpt));
}

static void test_cobs_longest_nonbroken_block(void) {
  uint8_t inp[254];
  memset(inp, 0x88, sizeof(inp));

  uint8_t outpt[256];
  memset(outpt, 0x88, sizeof(outpt));
  outpt[0] = 0xff;
  outpt[255] = 0;
  util_cobs_test(inp, sizeof(inp), outpt, sizeof(outpt));
}

static void test_cobs_nonzero_len_255(void) {
  uint8_t inp[255];
  memset(inp, 0x88, sizeof(inp));

  uint8_t outpt[258];
  memset(outpt, 0x88, sizeof(outpt));
  outpt[0] = 0xff;
  outpt[255] = 2;
  outpt[257] = 0;
  util_cobs_test(inp, sizeof(inp), outpt, sizeof(outpt));
}

static void test_cobs_zero_at_end_len_255(void) {
  uint8_t inp[255];
  memset(inp, 0x88, sizeof(inp));
  inp[254] = 0;

  uint8_t outpt[258];
  memset(outpt, 0x88, sizeof(outpt));
  outpt[0] = 0xff;
  outpt[255] = 1;
  outpt[256] = 1;
  outpt[257] = 0;
  util_cobs_test(inp, sizeof(inp), outpt, sizeof(outpt));
}

static void test_cobs_zero_at_start_len_255(void) {
  uint8_t inp[255];
  memset(inp, 0x88, sizeof(inp));
  inp[0] = 0;

  uint8_t outpt[257];
  memset(outpt, 0x88, sizeof(outpt));
  outpt[0] = 0x01;
  outpt[1] = 0xFF;
  outpt[256] = 0;
  util_cobs_test(inp, sizeof(inp), outpt, sizeof(outpt));
}

static void test_cobs_255_len_nonzero_block_in_middle(void) {
  // clang-format off
  uint8_t inp[] = { 0x01, 0x00, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0x00, 0x01 };
  uint8_t output[] = { 0x02, 0x01, 0xff, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0x02, 0xbb, 0x02, 0x01, 0x00 };
  // clang-format on
  util_cobs_test(inp, sizeof(inp), output, sizeof(output));
}

static void test_cobs_254_len_nonzero_block_in_middle(void) {
  // clang-format off
  uint8_t inp[] = { 0x01, 0x00, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0x00, 0x01 };
  uint8_t output[] = { 0x02, 0x01, 0xff, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0x01, 0x02, 0x01, 0x00 };
  // clang-format on
  util_cobs_test(inp, sizeof(inp), output, sizeof(output));
}

static void test_cobs_random1(void) {
  // clang-format off
  uint8_t inp[] = { 0x60, 0x0c, 0x73, 0x24, 0x3e, 0xb2, 0x51, 0x17, 0x9a, 0x05, 0x95, 0x3c, 0x1d, 0x47, 0x6c, 0x4e, 0xeb, 0xda, 0x02, 0x3d, 0x46, 0x0f, 0xd8, 0xe7, 0x15, 0x6a, 0xcd, 0xf4, 0x4b, 0x91, 0x1b, 0x59, 0xb5, 0x63, 0xd6, 0x5e, 0x1d, 0x06, 0x4d, 0x78, 0xa3, 0xb8, 0x17, 0x57, 0x41, 0xc5, 0xbd, 0xf8, 0x36, 0xc9, 0x66, 0xb8, 0x8f, 0x00, 0xc7, 0xa9, 0x2d, 0xa7, 0x61, 0x2d, 0x3d, 0xa2, 0xa1, 0x84 };
  uint8_t output[] = { 0x36, 0x60, 0x0c, 0x73, 0x24, 0x3e, 0xb2, 0x51, 0x17, 0x9a, 0x05, 0x95, 0x3c, 0x1d, 0x47, 0x6c, 0x4e, 0xeb, 0xda, 0x02, 0x3d, 0x46, 0x0f, 0xd8, 0xe7, 0x15, 0x6a, 0xcd, 0xf4, 0x4b, 0x91, 0x1b, 0x59, 0xb5, 0x63, 0xd6, 0x5e, 0x1d, 0x06, 0x4d, 0x78, 0xa3, 0xb8, 0x17, 0x57, 0x41, 0xc5, 0xbd, 0xf8, 0x36, 0xc9, 0x66, 0xb8, 0x8f, 0x0b, 0xc7, 0xa9, 0x2d, 0xa7, 0x61, 0x2d, 0x3d, 0xa2, 0xa1, 0x84, 0x00 };
  // clang-format on
  util_cobs_test(inp, sizeof(inp), output, sizeof(output));
}

static void test_cobs_random2(void) {
  // clang-format off
  uint8_t inp[] = { 0x03, 0x93, 0x2c, 0x02, 0x6a, 0x81, 0x71, 0x20, 0x5c, 0x6b, 0x60, 0x3a, 0xc1, 0x44, 0x5f, 0xdc, 0xe8, 0x02, 0x1e, 0xf2, 0xdc, 0x24, 0x1d, 0x00, 0xc6, 0x3e, 0xc0, 0x10, 0xfe, 0xf2, 0xa8, 0xf3, 0x0e, 0xa2, 0xc2, 0xca, 0x9d, 0x4f, 0x2b, 0x32, 0xb9, 0xb8, 0x73, 0xc6, 0x44, 0xb3, 0x87, 0x84, 0x3e, 0x2e, 0x2a, 0x53, 0x7a, 0xcd, 0x68, 0x61, 0xc2, 0xa5, 0x45, 0x53, 0xf1, 0x6c, 0xed, 0x57, 0x74, 0xfe, 0x79, 0xce, 0x56, 0x4e, 0x9a, 0x8c, 0x26, 0xa2, 0x0f, 0x7f, 0x19, 0x24, 0x03, 0xb7, 0xff, 0xa9, 0xdd, 0xcf, 0x75, 0x5b, 0x3d, 0xe1, 0xfe, 0x87, 0xc9, 0x78, 0xcc, 0xd0, 0x08, 0x6f, 0x2d, 0xf7, 0x3d, 0x3a, 0x43, 0x42, 0x69, 0xbb, 0x0e, 0x94, 0x2c, 0x95, 0xe8, 0xa6, 0x6a, 0x74, 0x42, 0xdf, 0x7e, 0x80, 0xf0, 0x10, 0x1c, 0x31, 0x73, 0x3e, 0x1b, 0xfb, 0xfb, 0x29, 0xdf, 0xb9, 0x3c, 0x43, 0xac, 0x18, 0x2f, 0x2e, 0xba, 0xcc, 0x78, 0x34, 0xe2, 0x11, 0xd5, 0xda, 0x91, 0x62, 0x3f, 0x8f, 0x11, 0xf7, 0x14, 0xf5, 0xdc, 0x78, 0xd9, 0xbe, 0xa6, 0x91, 0xe5, 0x8b, 0x4d, 0x4c, 0x2a, 0x82, 0x45, 0xe8, 0x6e, 0x60, 0x28, 0x22, 0x53, 0x05, 0x19, 0x71, 0x22, 0x90, 0xe8, 0x5e, 0x69, 0x17, 0x53, 0x9c, 0x72, 0x3b, 0xbd, 0xae, 0x27, 0xcd, 0xf4, 0x2e, 0x32, 0x2d, 0xcd, 0xd7, 0x2d, 0xa8, 0xc1, 0x21, 0x09, 0xbc, 0xaa, 0x36, 0xc6, 0x2f, 0x27, 0x98, 0xf3, 0x08, 0xa2, 0x79, 0x55, 0x9a, 0x79, 0xd3, 0x15, 0x13, 0x21, 0x87, 0xbd, 0x34, 0xb0, 0x17, 0x1a, 0x54, 0xdf, 0x32, 0x9e, 0xa9, 0x97, 0xff, 0xb1, 0x92, 0x9b, 0x6b, 0x08, 0x5e, 0xe0, 0x2f, 0xf4, 0xcf, 0x95, 0x61, 0xdd, 0x64, 0xe0, 0xdc, 0xc4, 0xbc, 0x63, 0x10, 0xd3, 0xe2, 0xaf, 0x2b, 0x49, 0xd8, 0x0b, 0x5c, 0xd5, 0xd8, 0x07, 0x39, 0x37, 0x1c, 0x8c, 0x2e, 0xec, 0x8a, 0xde, 0x24, 0x26, 0xf5, 0x34, 0x1f, 0xab, 0x68, 0x3c, 0x20, 0xeb, 0x33, 0xaf, 0x5f, 0x9d, 0x74, 0x99, 0xad, 0xc1, 0xd9, 0x11, 0x43, 0x19, 0x55, 0xd9, 0xfa, 0x99, 0xca, 0x68, 0xdb, 0xdc, 0x93, 0x8f, 0x4e, 0xc8, 0x74, 0xac, 0x76, 0x34, 0xcf, 0xbc, 0x90, 0x7e, 0xbb, 0xd6, 0x07, 0x1c, 0x99, 0x0e, 0x7e, 0x29, 0x77, 0xd2, 0x2a, 0x75, 0xc3, 0xf9, 0x8c, 0x03, 0x0f, 0x69, 0x13, 0x2a, 0x6d, 0x25, 0xf4, 0xd3, 0x26, 0x9e, 0x44, 0xdf, 0x50, 0xcf, 0x45, 0xd8, 0x50, 0x3b, 0xc6, 0xa8, 0x0d, 0x65, 0x1b, 0xf9, 0xf4, 0x42, 0xf5, 0x0c, 0x72, 0x3f, 0x5e, 0xc9, 0xfa, 0x79, 0x84, 0x4b, 0x4b, 0xd1, 0x85, 0x88, 0xf1, 0xda, 0x0d, 0x35, 0x8f, 0x80, 0xd6, 0x10, 0x35, 0x66, 0x42, 0x60, 0x5e, 0x1d, 0x42, 0xd7, 0x32, 0x97, 0x1a, 0x94, 0xdc, 0xae, 0x42, 0x5d, 0x5b, 0xb1, 0x2b, 0x06, 0x05, 0xa0, 0x58, 0xd4, 0xec, 0xa2, 0x54, 0x2a, 0xb0, 0xf8, 0xce, 0x56, 0x48, 0x55, 0xc0, 0x58, 0x9d, 0x77, 0x5e, 0x96, 0xe2, 0x49, 0xd1, 0x87, 0x50, 0xdb, 0x70, 0x99, 0xbc, 0x46, 0x17, 0x74, 0xfd, 0x9a, 0x98, 0xd1, 0x17, 0x01, 0x84, 0xe2, 0x7d, 0xe7, 0x3b, 0xcb, 0xdc, 0xc0, 0x28, 0xaf, 0x3c, 0x26, 0x19, 0x4b, 0xdc, 0x61, 0x95, 0x4c, 0x57, 0x10, 0x61, 0x7e, 0xec, 0x51, 0x61, 0xcb, 0xe7, 0x84, 0x47, 0xbe, 0xa5, 0x04, 0x16, 0xe4, 0x33, 0xe4, 0x30, 0x6c, 0xd4, 0xd1, 0x14, 0xa6, 0x41, 0x42, 0xfc, 0xf4, 0x66, 0xe3, 0xab, 0x02, 0x2d, 0xd4, 0x1c, 0x00, 0xb2, 0x95, 0x88, 0xa2, 0x92, 0x5b, 0x0e, 0x41, 0xa9, 0x79, 0x11, 0xfe, 0xe2, 0xa2, 0xb3, 0xc5, 0x76, 0xd2, 0x1b, 0x03, 0xe2, 0xe2, 0x2a, 0x9d, 0xf7, 0x35, 0x42, 0x03, 0xdf, 0xe3, 0x38, 0x29, 0x49, 0xfd, 0xcc, 0xf0, 0xdd, 0x97, 0x77, 0xd8, 0x21, 0x7c, 0x79, 0xc0, 0x31 };
  uint8_t output[] = { 0x18, 0x03, 0x93, 0x2c, 0x02, 0x6a, 0x81, 0x71, 0x20, 0x5c, 0x6b, 0x60, 0x3a, 0xc1, 0x44, 0x5f, 0xdc, 0xe8, 0x02, 0x1e, 0xf2, 0xdc, 0x24, 0x1d, 0xff, 0xc6, 0x3e, 0xc0, 0x10, 0xfe, 0xf2, 0xa8, 0xf3, 0x0e, 0xa2, 0xc2, 0xca, 0x9d, 0x4f, 0x2b, 0x32, 0xb9, 0xb8, 0x73, 0xc6, 0x44, 0xb3, 0x87, 0x84, 0x3e, 0x2e, 0x2a, 0x53, 0x7a, 0xcd, 0x68, 0x61, 0xc2, 0xa5, 0x45, 0x53, 0xf1, 0x6c, 0xed, 0x57, 0x74, 0xfe, 0x79, 0xce, 0x56, 0x4e, 0x9a, 0x8c, 0x26, 0xa2, 0x0f, 0x7f, 0x19, 0x24, 0x03, 0xb7, 0xff, 0xa9, 0xdd, 0xcf, 0x75, 0x5b, 0x3d, 0xe1, 0xfe, 0x87, 0xc9, 0x78, 0xcc, 0xd0, 0x08, 0x6f, 0x2d, 0xf7, 0x3d, 0x3a, 0x43, 0x42, 0x69, 0xbb, 0x0e, 0x94, 0x2c, 0x95, 0xe8, 0xa6, 0x6a, 0x74, 0x42, 0xdf, 0x7e, 0x80, 0xf0, 0x10, 0x1c, 0x31, 0x73, 0x3e, 0x1b, 0xfb, 0xfb, 0x29, 0xdf, 0xb9, 0x3c, 0x43, 0xac, 0x18, 0x2f, 0x2e, 0xba, 0xcc, 0x78, 0x34, 0xe2, 0x11, 0xd5, 0xda, 0x91, 0x62, 0x3f, 0x8f, 0x11, 0xf7, 0x14, 0xf5, 0xdc, 0x78, 0xd9, 0xbe, 0xa6, 0x91, 0xe5, 0x8b, 0x4d, 0x4c, 0x2a, 0x82, 0x45, 0xe8, 0x6e, 0x60, 0x28, 0x22, 0x53, 0x05, 0x19, 0x71, 0x22, 0x90, 0xe8, 0x5e, 0x69, 0x17, 0x53, 0x9c, 0x72, 0x3b, 0xbd, 0xae, 0x27, 0xcd, 0xf4, 0x2e, 0x32, 0x2d, 0xcd, 0xd7, 0x2d, 0xa8, 0xc1, 0x21, 0x09, 0xbc, 0xaa, 0x36, 0xc6, 0x2f, 0x27, 0x98, 0xf3, 0x08, 0xa2, 0x79, 0x55, 0x9a, 0x79, 0xd3, 0x15, 0x13, 0x21, 0x87, 0xbd, 0x34, 0xb0, 0x17, 0x1a, 0x54, 0xdf, 0x32, 0x9e, 0xa9, 0x97, 0xff, 0xb1, 0x92, 0x9b, 0x6b, 0x08, 0x5e, 0xe0, 0x2f, 0xf4, 0xcf, 0x95, 0x61, 0xdd, 0x64, 0xe0, 0xdc, 0xc4, 0xbc, 0x63, 0x10, 0xd3, 0xe2, 0xaf, 0x2b, 0x49, 0xd8, 0x0b, 0x5c, 0xd5, 0xd8, 0x07, 0x39, 0x37, 0x1c, 0x8c, 0x2e, 0xec, 0x8a, 0xde, 0x24, 0x26, 0xf5, 0x34, 0x1f, 0xab, 0x68, 0x3c, 0x20, 0xeb, 0x33, 0xcf, 0xaf, 0x5f, 0x9d, 0x74, 0x99, 0xad, 0xc1, 0xd9, 0x11, 0x43, 0x19, 0x55, 0xd9, 0xfa, 0x99, 0xca, 0x68, 0xdb, 0xdc, 0x93, 0x8f, 0x4e, 0xc8, 0x74, 0xac, 0x76, 0x34, 0xcf, 0xbc, 0x90, 0x7e, 0xbb, 0xd6, 0x07, 0x1c, 0x99, 0x0e, 0x7e, 0x29, 0x77, 0xd2, 0x2a, 0x75, 0xc3, 0xf9, 0x8c, 0x03, 0x0f, 0x69, 0x13, 0x2a, 0x6d, 0x25, 0xf4, 0xd3, 0x26, 0x9e, 0x44, 0xdf, 0x50, 0xcf, 0x45, 0xd8, 0x50, 0x3b, 0xc6, 0xa8, 0x0d, 0x65, 0x1b, 0xf9, 0xf4, 0x42, 0xf5, 0x0c, 0x72, 0x3f, 0x5e, 0xc9, 0xfa, 0x79, 0x84, 0x4b, 0x4b, 0xd1, 0x85, 0x88, 0xf1, 0xda, 0x0d, 0x35, 0x8f, 0x80, 0xd6, 0x10, 0x35, 0x66, 0x42, 0x60, 0x5e, 0x1d, 0x42, 0xd7, 0x32, 0x97, 0x1a, 0x94, 0xdc, 0xae, 0x42, 0x5d, 0x5b, 0xb1, 0x2b, 0x06, 0x05, 0xa0, 0x58, 0xd4, 0xec, 0xa2, 0x54, 0x2a, 0xb0, 0xf8, 0xce, 0x56, 0x48, 0x55, 0xc0, 0x58, 0x9d, 0x77, 0x5e, 0x96, 0xe2, 0x49, 0xd1, 0x87, 0x50, 0xdb, 0x70, 0x99, 0xbc, 0x46, 0x17, 0x74, 0xfd, 0x9a, 0x98, 0xd1, 0x17, 0x01, 0x84, 0xe2, 0x7d, 0xe7, 0x3b, 0xcb, 0xdc, 0xc0, 0x28, 0xaf, 0x3c, 0x26, 0x19, 0x4b, 0xdc, 0x61, 0x95, 0x4c, 0x57, 0x10, 0x61, 0x7e, 0xec, 0x51, 0x61, 0xcb, 0xe7, 0x84, 0x47, 0xbe, 0xa5, 0x04, 0x16, 0xe4, 0x33, 0xe4, 0x30, 0x6c, 0xd4, 0xd1, 0x14, 0xa6, 0x41, 0x42, 0xfc, 0xf4, 0x66, 0xe3, 0xab, 0x02, 0x2d, 0xd4, 0x1c, 0x2e, 0xb2, 0x95, 0x88, 0xa2, 0x92, 0x5b, 0x0e, 0x41, 0xa9, 0x79, 0x11, 0xfe, 0xe2, 0xa2, 0xb3, 0xc5, 0x76, 0xd2, 0x1b, 0x03, 0xe2, 0xe2, 0x2a, 0x9d, 0xf7, 0x35, 0x42, 0x03, 0xdf, 0xe3, 0x38, 0x29, 0x49, 0xfd, 0xcc, 0xf0, 0xdd, 0x97, 0x77, 0xd8, 0x21, 0x7c, 0x79, 0xc0, 0x31, 0x00 };
  // clang-format on
  util_cobs_test(inp, sizeof(inp), output, sizeof(output));
}

static void test_cobs_maxlen(void) {
  TEST_ASSERT_EQUAL_INT(0 + 2, COBS_MAXLEN(0));
  TEST_ASSERT_EQUAL_INT(1 + 2, COBS_MAXLEN(1));
  TEST_ASSERT_EQUAL_INT(2 + 2, COBS_MAXLEN(2));
  TEST_ASSERT_EQUAL_INT(254 + 2, COBS_MAXLEN(254));
  TEST_ASSERT_EQUAL_INT(255 + 3, COBS_MAXLEN(255));
}

// ======== Main ===================================================================================

void setUp(void) {}

void tearDown(void) {}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_cobs_nonzero_len_1);
  RUN_TEST(test_cobs_zero_len_1);
  RUN_TEST(test_cobs_len_zero);
  RUN_TEST(test_cobs_short_with_zeros);
  RUN_TEST(test_cobs_short_zeroes);
  RUN_TEST(test_cobs_restarts);
  RUN_TEST(test_cobs_longest_nonbroken_block);
  RUN_TEST(test_cobs_nonzero_len_255);
  RUN_TEST(test_cobs_zero_at_end_len_255);
  RUN_TEST(test_cobs_zero_at_start_len_255);
  RUN_TEST(test_cobs_255_len_nonzero_block_in_middle);
  RUN_TEST(test_cobs_254_len_nonzero_block_in_middle);
  RUN_TEST(test_cobs_random1);
  RUN_TEST(test_cobs_random2);
  RUN_TEST(test_cobs_maxlen);
  return UNITY_END();
}
