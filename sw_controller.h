#pragma once

#include <stdbool.h>
#include <stdint.h>

#define SW_REPORT_SIZE 64
#define SW_REPORT_INTERVAL_MS 12

typedef struct {
  uint8_t data[SW_REPORT_SIZE];
  int len;
  uint8_t report_id;
} SW_REPORT_t;

#ifdef __cplusplus
extern "C" {
#endif

void init_sw_module(void);
void handle_host_data(SW_REPORT_t *report, const uint8_t *buf,
                      uint16_t bufsize);
void build_sw_report(SW_REPORT_t *report, uint8_t report_id, uint8_t cmd,
                     const uint8_t *data, int len);

#ifdef __cplusplus
}
#endif

extern const uint8_t sw_initial_input_report[11];
extern bool g_input_enable;

// Switch Button Report Bitmap

#define SW_REP0_BITPOS_ZR 7
#define SW_REP0_BITPOS_R 6
#define SW_REP0_BITPOS_R_SR 5
#define SW_REP0_BITPOS_R_SL 4
#define SW_REP0_BITPOS_A 3
#define SW_REP0_BITPOS_B 2
#define SW_REP0_BITPOS_X 1
#define SW_REP0_BITPOS_Y 0

#define SW_REP1_BITPOS_HOME 4
#define SW_REP1_BITPOS_THUMBL 3
#define SW_REP1_BITPOS_THUMBR 2
#define SW_REP1_BITPOS_PLUS 1
#define SW_REP1_BITPOS_MINUS 0

#define SW_REP2_BITPOS_ZL 7
#define SW_REP2_BITPOS_L 6
#define SW_REP2_BITPOS_L_SL 5
#define SW_REP2_BITPOS_L_SR 4
#define SW_REP2_BITPOS_LEFT 3
#define SW_REP2_BITPOS_RIGHT 2
#define SW_REP2_BITPOS_UP 1
#define SW_REP2_BITPOS_DOWN 0
