/*
    Switch Controller Definitions
*/

#include "sw_controller.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "bsp/board.h"
#include "pico/stdlib.h"

// SPI flash data (from 0x6000)
const uint8_t spi_factory_calib_data[] = {
    // 0x00-0x0f: serial number (16bytes)
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    // 0x10-0x11: padding
    0xff,
    0xff,
    // 0x12: Device type (not used)
    0x03,
    // 0x13: unknown
    0xa0,
    // 0x14-1a: padding
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    // 0x1b: color info exists
    0x01,
    // 0x1c-0x1f: padding
    0xff,
    0xff,
    0xff,
    0xff,
    // 0x20-0x37: 6-axis motion sensor calib
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    // 0x38-0x3c: padding
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    // 0x3d-0x45: left analog stick calib
    0xf0,
    0x07,
    0x7f,
    0xf0,
    0x07,
    0x7f,
    0xf0,
    0x07,
    0x7f,
    // 0x46-0x4e: right analog stick calib
    0xf0,
    0x07,
    0x7f,
    0xf0,
    0x07,
    0x7f,
    0xf0,
    0x07,
    0x7f,
    // 0x4f: padding
    0xFF,
    // 0x50-0x5b: body color
    // body color
    0xd5,
    0xd6,
    0xb9,
    // button color
    0x85,
    0x87,
    0xba,
    // left grip
    0xff,
    0xff,
    0xff,
    // right grip
    0xff,
    0xff,
    0xff,
    // 0x5c-0x7f: padding
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    // 0x80-0x85: 6-Axis Horizontal Offsets. (JC sideways)
    0x50,
    0xfd,
    0x00,
    0x00,
    0xc6,
    0x0f,
    // 0x86-0x97: Stick device parameters 1
    0x0f,
    0x30,
    0x61,
    0x00,
    0x32,
    0xf3,
    0xd4,
    0x14,
    0x54,
    0x41,
    0x15,
    0x54,
    0xc7,
    0x79,
    0x9c,
    0x33,
    0x36,
    0x63,
    // 0x98-0xa9: Stick device parameters 2
    0x0F,
    0x30,
    0x61,
    0x00,
    0x32,
    0xF3,
    0xD4,
    0x14,
    0x54,
    0x41,
    0x15,
    0x54,
    0xC7,
    0x79,
    0x9C,
    0x33,
    0x36,
    0x63,
};

// SPI flash data (from 0x8000)
const uint8_t spi_user_calib_data[] = {
    // 0x10 - 0x27
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    // 0x28 - 0x3F
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
};

const uint8_t sw_initial_input_report[11] = {0x81, 0x00, 0x00, 0x00, 0xf0, 0x07,
                                             0x7f, 0xf0, 0x07, 0x7f, 0x0c};

uint8_t mac_addr[6];

void init_sw_module(void) {
  int i;
  srand(board_millis());

  for (i = 0; i < sizeof(mac_addr); i++) {
    mac_addr[i] = rand() % 256;
  }
}

void build_sw_report(SW_REPORT_t *report, uint8_t report_id, uint8_t cmd,
                     const uint8_t *data, int len) {
  report->data[0] = cmd;
  memcpy(report->data + 1, data, len);
  memset(report->data + 1 + len, 0, SW_REPORT_SIZE - len - 1);
  report->len += 1 + len;
  report->report_id = report_id;
}

static void build_uart_report(SW_REPORT_t *report, uint8_t code, uint8_t subcmd,
                              const uint8_t *data, uint8_t len) {
  build_sw_report(report, 0x21, (board_millis() / 10) % 256, NULL, 0);

  memcpy(report->data + report->len, sw_initial_input_report,
         sizeof(sw_initial_input_report));
  report->len += sizeof(sw_initial_input_report);
  report->data[report->len++] = code;
  report->data[report->len++] = subcmd;
  memcpy(report->data + report->len, data, len);
  report->len += len;
}

static void build_spi_report(SW_REPORT_t *report, uint16_t addr,
                             const uint8_t *data, uint8_t len) {
  build_uart_report(report, 0x90, 0x10, NULL, 0);
  int index = report->len;
  report->data[index++] = addr & 0xff;
  report->data[index++] = addr >> 8;
  report->data[index++] = 0x00;
  report->data[index++] = 0x00;
  report->data[index++] = len;
  report->len = index;

  memcpy(report->data + report->len, data, len);
  report->len += len;
}

void handle_spi_flash_read(SW_REPORT_t *report, const uint8_t *host_data,
                           const uint16_t host_data_size) {
  uint16_t spi_addr = host_data[11] | host_data[12] << 8;
  uint8_t spi_len = host_data[15];
  uint16_t offset = spi_addr - 0x6000;
  uint8_t const *flash_data;
  size_t flash_data_size;
  size_t copy_len = 0;

  uint8_t spi_data[SW_REPORT_SIZE];
  memset(spi_data, 0xff, sizeof(spi_data));

  if ((spi_addr & 0xF000) == 0x6000) {
    flash_data = spi_factory_calib_data;
    flash_data_size = sizeof(spi_factory_calib_data);
    copy_len = spi_len;
  } else if ((spi_addr & 0xF000) == 0x8000) {
    flash_data = spi_user_calib_data;
    flash_data_size = sizeof(spi_user_calib_data);
    copy_len = spi_len;
  } else {
    // Other ranges
    flash_data = spi_factory_calib_data;
    offset = 0;
    flash_data_size = 0;
  }

  // Check area overflow
  if (offset >= flash_data_size) {
    // no copy .. reply 0xffs
    offset = 0;
    copy_len = 0;
  } else if ((offset + spi_len) >= flash_data_size) {
    copy_len = flash_data_size - offset;
  }

  //
  memcpy(spi_data, flash_data + offset, copy_len);
  build_spi_report(report, spi_addr, spi_data, spi_len);
}

void handle_subcommand(SW_REPORT_t *report, const uint8_t *host_data,
                       const uint16_t host_data_size) {
  if (host_data_size <= 16) {
    return;
  }

  uint8_t sub = host_data[10];

  switch (sub) {
  case 0x01: { // Bluetooth manual paring
    const uint8_t data[] = {0x03};
    build_uart_report(report, 0x81, sub, data, 1);
  } break;

  case 0x02: { // Request device info
    uint8_t dev_info[] = {0x03, 0x48, 0x03, 0x02, 0xaa, 0xaa,
                          0xaa, 0xaa, 0xaa, 0xaa, 0x03, 0x01};
    memcpy(dev_info + 4, mac_addr, sizeof(mac_addr));
    build_uart_report(report, 0x82, sub, dev_info, sizeof(dev_info));
  } break;

  case 0x03: // Set input report mode
  case 0x08: // Set shipment low power state
  case 0x38: // Set HOME light
  case 0x40: // Enable IMU
  case 0x48: // Enable vibration
    build_uart_report(report, 0x80, sub, NULL, 0);
    break;

  case 0x04: // Trigger buttons elapsed time
    build_uart_report(report, 0x83, sub, NULL, 0);
    break;

  case 0x10: { // SPI flash read
    handle_spi_flash_read(report, host_data, host_data_size);
  } break;

  case 0x11: { // SPI flash write
    const uint8_t ack_reply[] = {0x00};
    build_uart_report(report, 0x80, sub, ack_reply, sizeof(ack_reply));
  } break;

  case 0x12: { // SPI sector erase
    const uint8_t ack_reply[] = {0x00};
    build_uart_report(report, 0x80, sub, ack_reply, sizeof(ack_reply));
  } break;

  case 0x21: { // Set NFC/IR MCU configuration
    const uint8_t nfc_conf[] = {0x01, 0x00, 0xff, 0x00, 0x03, 0x00, 0x05, 0x01};
    build_uart_report(report, 0xa0, sub, nfc_conf, sizeof(nfc_conf));
  } break;

  case 0x30: // Set player light
    gpio_put(25, 1);
    build_uart_report(report, 0x80, sub, NULL, 0);
    break;

  // case 0x33:
  default: {
    const uint8_t ack_reply[] = {0x03};
    build_uart_report(report, 0x80, sub, ack_reply, sizeof(ack_reply));
  } break;
  }
}

void handle_80_command(SW_REPORT_t *report, const uint8_t *host_data,
                       const uint16_t host_data_size) {
  uint8_t sub0 = host_data[1];

  switch (sub0) {
  case 0x01: { // Current connection status
    uint8_t mac_data[] = {0x00, 0x03, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
    memcpy(mac_data + 2, mac_addr, sizeof(mac_addr));
    build_sw_report(report, 0x81, sub0, mac_data, sizeof(mac_data));
  } break;

  case 0x02: // Send handshaking packet over UART
    build_sw_report(report, 0x81, sub0, NULL, 0);
    break;

  case 0x03: // Switch baudrate to 3Mbit
    build_sw_report(report, 0x81, sub0, NULL, 0);
    break;

  case 04: // Only talk over USB HID without timeouts
    g_input_enable = true;
    break;
  }
}

void handle_host_data(SW_REPORT_t *report, const uint8_t *host_data,
                      const uint16_t host_data_size) {
  uint8_t cmd = host_data[0];

  switch (cmd) {
  case 0x80:
    handle_80_command(report, host_data, host_data_size);
    break;

  case 0x01:
    handle_subcommand(report, host_data, host_data_size);
    break;
  }
}
