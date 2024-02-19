#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PIN_MISO 4
#define PIN_CS 5
#define PIN_SCK 2
#define PIN_MOSI 3
#define PIN_ACK 1
#define PIN_MODE 6

#define SPI_PORT spi0
#define READ_BIT 0x80

// Controller ID

#define PSX_CTRLID_INVALID 0x00
#define PSX_CTRLID_DIGITAL 0x41
#define PSX_CTRLID_ANALOG 0x53
#define PSX_CTRLID_DUAL_ANALOG 0x73
#define PSX_CTRLID_DUAL_SHOCK2 0x79

// PSX Button

// PSX report: L D R U  St R3 L3 Se   [] X O ^   R1 L1 R2 L2
//             --------------------   -----------------------
//             report[1] Button1      report[2]  Button2

#define PSX_BUTTON1_LEFT 0x80
#define PSX_BUTTON1_DOWN 0x40
#define PSX_BUTTON1_RIGHT 0x20
#define PSX_BUTTON1_UP 0x10

#define PSX_BUTTON1_START 0x08
#define PSX_BUTTON1_R3 0x04
#define PSX_BUTTON1_L3 0x02
#define PSX_BUTTON1_SELECT 0x01

#define PSX_BUTTON2_RECT 0x80
#define PSX_BUTTON2_CROSS 0x40
#define PSX_BUTTON2_CIRCLE 0x20
#define PSX_BUTTON2_TRIANGLE 0x10
#define PSX_BUTTON2_R1 0x08
#define PSX_BUTTON2_L1 0x04
#define PSX_BUTTON2_R2 0x02
#define PSX_BUTTON2_L2 0x01

// Raw communication
bool comm_psx_pad(uint8_t *send, uint8_t *recv, uint8_t len,
                  bool skip_last_byte_ack);
//
bool get_psx_pad_data(uint8_t *psx_report, uint8_t *pad_id);

#ifdef __cplusplus
}
#endif
