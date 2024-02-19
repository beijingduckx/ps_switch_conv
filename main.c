/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

/*
    PSX Controller - Pro Controller converter

    based on TinyUSB HID device sample
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bsp/board.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"
#include "psx_controller.h"
#include "sw_controller.h"
#include "tusb.h"

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+
void hid_task(void);

bool g_input_enable = false;

// PSX SPI communication speed
#define SPI_SPEED_KHZ 250

static void io_init(void) {
  spi_init(SPI_PORT, SPI_SPEED_KHZ * 1000);
  gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
  gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
  gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
  gpio_set_function(PIN_ACK, GPIO_FUNC_XIP);
  gpio_set_function(PIN_MODE, GPIO_FUNC_XIP);

  // Chip select is active-low
  gpio_init(PIN_CS);
  gpio_set_dir(PIN_CS, GPIO_OUT);
  gpio_put(PIN_CS, 1);

  // Ack port
  gpio_init(PIN_ACK);
  gpio_set_dir(PIN_ACK, GPIO_IN);
  gpio_set_pulls(PIN_ACK, true, false);

  // Joy mode
  gpio_set_dir(PIN_MODE, GPIO_IN);
  gpio_set_pulls(PIN_MODE, true, false);

  spi_set_format(SPI_PORT, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);

  // LED indicator
  gpio_init(25);
  gpio_set_dir(25, GPIO_OUT);
}

/*------------- MAIN -------------*/
int main() {
  board_init();
  io_init();

  tusb_init();

  while (1) {
    tud_task();  // tinyusb device task
    hid_task();
  }

  return 0;
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void) {}

// Invoked when device is unmounted
void tud_umount_cb(void) {}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en) { (void)remote_wakeup_en; }

// Invoked when usb bus is resumed
void tud_resume_cb(void) {}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id,
                               hid_report_type_t report_type, uint8_t *buf,
                               uint16_t reqlen) {
  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id,
                           hid_report_type_t report_type, uint8_t const *buf,
                           uint16_t bufsize) {
  static bool sw_module_initialized = false;

  if (!sw_module_initialized) {
    init_sw_module();
  }

  if (report_id == 0 && report_type == 0) {
    SW_REPORT_t report;
    memset(&report, 0, sizeof(report));
    handle_host_data(&report, buf, bufsize);
    tud_hid_report(report.report_id, report.data, SW_REPORT_SIZE - 1);
  }
}

//--------------------------------------------------------------------+
// HID TASK
//--------------------------------------------------------------------+
// PSX report: L D R U  St R3 L3 Se   [] X O ^   R1 L1 R2 L2
//             --------------------   -----------------------
//             report[1] Button1      report[2]  Button2

#define IS_BUTTON(button_val, button_const) \
  (((button_val) & (button_const)) / (button_const))

static void make_button_report(uint8_t *psx_recv, uint8_t *sw_input) {
  uint8_t psx_button1 = psx_recv[1];
  uint8_t psx_button2 = psx_recv[2];

  int joy_mode = gpio_get(PIN_MODE);

  if (joy_mode == false) {
    // Normal Pro-con mode
    sw_input[0] =
        (IS_BUTTON(psx_button2, PSX_BUTTON2_CIRCLE) << SW_REP0_BITPOS_A) |
        (IS_BUTTON(psx_button2, PSX_BUTTON2_CROSS) << SW_REP0_BITPOS_B) |
        (IS_BUTTON(psx_button2, PSX_BUTTON2_TRIANGLE) << SW_REP0_BITPOS_X) |
        (IS_BUTTON(psx_button2, PSX_BUTTON2_RECT) << SW_REP0_BITPOS_Y) |
        (IS_BUTTON(psx_button2, PSX_BUTTON2_R2) << SW_REP0_BITPOS_ZR) |
        (IS_BUTTON(psx_button2, PSX_BUTTON2_R1) << SW_REP0_BITPOS_R);

    sw_input[1] =
        (IS_BUTTON(psx_button1, PSX_BUTTON1_SELECT) << SW_REP1_BITPOS_HOME) |
        (IS_BUTTON(psx_button1, PSX_BUTTON1_START) << SW_REP1_BITPOS_PLUS) |
        (IS_BUTTON(psx_button1, PSX_BUTTON1_L3) << SW_REP1_BITPOS_THUMBL) |
        (IS_BUTTON(psx_button1, PSX_BUTTON1_R3) << SW_REP1_BITPOS_THUMBR);

    sw_input[2] =
        (IS_BUTTON(psx_button1, PSX_BUTTON1_DOWN) << SW_REP2_BITPOS_DOWN) |
        (IS_BUTTON(psx_button1, PSX_BUTTON1_UP) << SW_REP2_BITPOS_UP) |
        (IS_BUTTON(psx_button1, PSX_BUTTON1_RIGHT) << SW_REP2_BITPOS_RIGHT) |
        (IS_BUTTON(psx_button1, PSX_BUTTON1_LEFT) << SW_REP2_BITPOS_LEFT) |
        (IS_BUTTON(psx_button2, PSX_BUTTON2_L1) << SW_REP2_BITPOS_L) |
        (IS_BUTTON(psx_button2, PSX_BUTTON2_L2) << SW_REP2_BITPOS_ZL);
  } else {
    // Tata-con mode
    // Tata-con    ML      MR   TL     TR    ST   SEL
    // PSX         LEFT    O    L1     R1    ST   SEL
    // Switch      RIGHT   B    LEFT   A     DOWN HOME
    sw_input[0] =
        (IS_BUTTON(psx_button2, PSX_BUTTON2_CIRCLE) << SW_REP0_BITPOS_B) |
        (IS_BUTTON(psx_button2, PSX_BUTTON2_R1) << SW_REP0_BITPOS_A);

    sw_input[1] =
        (IS_BUTTON(psx_button1, PSX_BUTTON1_SELECT) << SW_REP1_BITPOS_HOME);

    sw_input[2] =
        (IS_BUTTON(psx_button1, PSX_BUTTON1_START) << SW_REP2_BITPOS_DOWN) |
        (IS_BUTTON(psx_button1, PSX_BUTTON1_LEFT) << SW_REP2_BITPOS_RIGHT) |
        (IS_BUTTON(psx_button2, PSX_BUTTON2_L1) << SW_REP2_BITPOS_LEFT);
  }
}

void input_response(void) {
  uint8_t sw_report[SW_REPORT_SIZE];

  memcpy(sw_report, sw_initial_input_report, sizeof(sw_initial_input_report));

  // Read PSX pad data
  uint8_t pad_id;
  uint8_t psx_recv[22];
  uint8_t sw_input[9];
  uint8_t report_length;

  bool result = get_psx_pad_data(psx_recv, &pad_id);

  // Build HID report according to the controller type
  uint8_t dir;

  // extract stick/button info
  dir = (psx_recv[1] & 0xf0) >> 4;  // L D R U

  switch (pad_id) {
    case PSX_CTRLID_DIGITAL:
      // build HID report for digital mode
      make_button_report(psx_recv, sw_input);
      memset(sw_input + 3, 0, 6);
      report_length = 3;
      break;

    case PSX_CTRLID_DUAL_ANALOG:
      // build HID report for analog mode
      make_button_report(psx_recv, sw_input);
      report_length = 9;

      // psx 3: X2
      // psx 4: Y2
      // psx 5: X1
      // psx 6: Y1

      // Left X  - PSX Left X
      sw_input[3] = (psx_recv[5] << 4) & 0xff;
      sw_input[4] = psx_recv[5] >> 4;  // Upper 4bits are zero (Y lower)

      // Left Y  - PSX Left Y
      sw_input[5] = ~psx_recv[6];

      // Right X  - PSX Right X
      sw_input[6] = (psx_recv[3] << 4) & 0xff;
      sw_input[7] = psx_recv[3] >> 4;

      // Right Y  - PSX Right Y
      sw_input[8] = ~psx_recv[4];
      break;

    default:
      break;
  }
  // Copy input data into report .. skipping connection_info | bettery_level
  memcpy(sw_report + 1, sw_input, report_length);

  SW_REPORT_t report;
  memset(&report, 0, sizeof(report));
  build_sw_report(&report, 0x30, (board_millis() / 10) % 256, sw_report,
                  sizeof(sw_initial_input_report));
  tud_hid_report(report.report_id, report.data, SW_REPORT_SIZE - 1);
}

void hid_task(void) {
  const uint32_t interval_ms = SW_REPORT_INTERVAL_MS;
  static uint32_t start_ms = 0;

  if (board_millis() - start_ms < interval_ms) return;  // not enough time
  start_ms += interval_ms;

  if (!tud_hid_ready()) return;

  if (g_input_enable) {
    input_response();
  }
}
