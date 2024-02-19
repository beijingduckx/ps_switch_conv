#include "psx_controller.h"

#include <stdlib.h>

#include "bsp/board.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"

// Command sequence

#define PSX_CTRLER_ADDR 0x01
#define PSX_COMM_POLL 0x42

static inline uint8_t bit_reverse(uint8_t data) {
  uint16_t tmp;

  tmp = data;
  tmp = (((tmp & 0xaaaa) >> 1) | ((tmp & 0x5555) << 1));
  tmp = (((tmp & 0xcccc) >> 2) | ((tmp & 0x3333) << 2));
  tmp = (((tmp & 0xf0f0) >> 4) | ((tmp & 0x0f0f) << 4));
  //    tmp = (((tmp & 0xff00) >> 8) | ((tmp & 0x00ff) << 8));

  return (uint8_t)(tmp & 0xff);
}

static void bit_reverse_array(uint8_t *data, int len) {
  int index;

  for (index = 0; index < len; index++) {
    data[index] = bit_reverse(data[index]);
  }
}

#define ACK_TIMEOUT_US 100

static bool wait_psx_pad_ack() {
  absolute_time_t timeout;

  timeout = make_timeout_time_us(ACK_TIMEOUT_US);

  while (1) {
    if (absolute_time_diff_us(get_absolute_time(), timeout) < 0) {
      return false;
    }
    if (gpio_get(PIN_ACK) == false) {  // false = Low
      return true;
    }
  }
}

bool comm_psx_pad(uint8_t *send, uint8_t *recv, uint8_t len,
                  bool skip_last_byte_ack) {
  int index;

  bit_reverse_array(send, 2);

  for (index = 0; index < len; index++) {
    spi_write_read_blocking(SPI_PORT, &send[index], &recv[index], 1);
    if ((skip_last_byte_ack == false ||
         (skip_last_byte_ack == true && index != (len - 1))) &&
        wait_psx_pad_ack() == false) {
      return false;
    }
  }

  bit_reverse_array(recv, len);

  return true;
}

bool get_psx_pad_data(uint8_t *psx_report, uint8_t *pad_id) {
  uint8_t id;
  uint8_t psx_send[22];

  // Reading PSX pad state

  // Read Controller Type
  gpio_put(PIN_CS, 0);
  sleep_us(5);

  memset(psx_send, 0, sizeof(psx_send));
  psx_send[0] = PSX_CTRLER_ADDR;
  psx_send[1] = PSX_COMM_POLL;

  bool result = comm_psx_pad(psx_send, psx_report, 2, false);
  if (result == true) {
    id = psx_report[1];
  } else {
    id = PSX_CTRLID_INVALID;
  }
  *pad_id = id;

  // Read state according to the controller type
  switch (id) {
    case PSX_CTRLID_DIGITAL:
      result = comm_psx_pad(psx_send, psx_report, 3, true);
      // invert bits for button part
      psx_report[1] = ~psx_report[1];
      psx_report[2] = ~psx_report[2];
      break;

    case PSX_CTRLID_DUAL_ANALOG:
      result = comm_psx_pad(psx_send, psx_report, 7, true);
      // invert bits for button part
      psx_report[1] = ~psx_report[1];
      psx_report[2] = ~psx_report[2];
      break;

    default:
      result = false;
      break;
  }
  // Un-select controller
  gpio_put(PIN_CS, 1);

  return result;
}
