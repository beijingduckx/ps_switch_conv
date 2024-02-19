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

#include "tusb.h"

/* A combination of interfaces must have a unique product id, since PC will save
 * device driver after the first plug. Same VID/PID with different interface e.g
 * MSC (first), then CDC (later) will possibly cause system error on PC.
 *
 * Auto ProductID layout's Bitmap:
 *   [MSB]         HID | MSC | CDC          [LSB]
 */
#define _PID_MAP(itf, n) ((CFG_TUD_##itf) << (n))
#define USB_PID                                                      \
  (0x4000 | _PID_MAP(CDC, 0) | _PID_MAP(MSC, 1) | _PID_MAP(HID, 2) | \
   _PID_MAP(MIDI, 3) | _PID_MAP(VENDOR, 4))

//--------------------------------------------------------------------+
// Device Descriptors
//--------------------------------------------------------------------+
tusb_desc_device_t const desc_device = {
    .bLength = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = 0x00,
    .bDeviceSubClass = 0x00,
    .bDeviceProtocol = 0x00,
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor = 0x057e,
    .idProduct = 0x2009,
    .bcdDevice = 0x0200,

    .iManufacturer = 0x01,
    .iProduct = 0x02,
    .iSerialNumber = 0x00,

    .bNumConfigurations = 0x01};

// Invoked when received GET DEVICE DESCRIPTOR
// Application return pointer to descriptor
uint8_t const* tud_descriptor_device_cb(void) {
  return (uint8_t const*)&desc_device;
}

//--------------------------------------------------------------------+
// HID Report Descriptor
//--------------------------------------------------------------------+

// HID Descriptor from XE1AJ-USB
uint8_t const desc_hid_report[] = {
    0x05, 0x01,  // Usage Page (Generic Desktop Ctrls)
    0x15, 0x00,  // Logical Minimum (0)
    0x09, 0x04,  // Usage (Joystick)
    0xA1, 0x01,  // Collection (Application)
    0x85, 0x30,  //   Report ID (48)
    0x05, 0x01,  //   Usage Page (Generic Desktop Ctrls)
    0x05, 0x09,  //   Usage Page (Button)
    0x19, 0x01,  //   Usage Minimum (0x01)
    0x29, 0x0A,  //   Usage Maximum (0x0A)
    0x15, 0x00,  //   Logical Minimum (0)
    0x25, 0x01,  //   Logical Maximum (1)
    0x75, 0x01,  //   Report Size (1)
    0x95, 0x0A,  //   Report Count (10)
    0x55, 0x00,  //   Unit Exponent (0)
    0x65, 0x00,  //   Unit (None)
    0x81, 0x02,  //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null
                 //   Position)
    0x05, 0x09,  //   Usage Page (Button)
    0x19, 0x0B,  //   Usage Minimum (0x0B)
    0x29, 0x0E,  //   Usage Maximum (0x0E)
    0x15, 0x00,  //   Logical Minimum (0)
    0x25, 0x01,  //   Logical Maximum (1)
    0x75, 0x01,  //   Report Size (1)
    0x95, 0x04,  //   Report Count (4)
    0x81, 0x02,  //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null
                 //   Position)
    0x75, 0x01,  //   Report Size (1)
    0x95, 0x02,  //   Report Count (2)
    0x81, 0x03,  //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No
                 //   Null Position)
    0x0B, 0x01, 0x00, 0x01, 0x00,  //   Usage (0x010001)
    0xA1, 0x00,                    //   Collection (Physical)
    0x0B, 0x30, 0x00, 0x01, 0x00,  //     Usage (0x010030)
    0x0B, 0x31, 0x00, 0x01, 0x00,  //     Usage (0x010031)
    0x0B, 0x32, 0x00, 0x01, 0x00,  //     Usage (0x010032)
    0x0B, 0x35, 0x00, 0x01, 0x00,  //     Usage (0x010035)
    0x15, 0x00,                    //     Logical Minimum (0)
    0x27, 0xFF, 0xFF, 0x00, 0x00,  //     Logical Maximum (65534)
    0x75, 0x10,                    //     Report Size (16)
    0x95, 0x04,                    //     Report Count (4)
    0x81, 0x02,  //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No
                 //     Null Position)
    0xC0,        //   End Collection
    0x0B, 0x39, 0x00, 0x01, 0x00,  //   Usage (0x010039)
    0x15, 0x00,                    //   Logical Minimum (0)
    0x25, 0x07,                    //   Logical Maximum (7)
    0x35, 0x00,                    //   Physical Minimum (0)
    0x46, 0x3B, 0x01,              //   Physical Maximum (315)
    0x65, 0x14,  //   Unit (System: English Rotation, Length: Centimeter)
    0x75, 0x04,  //   Report Size (4)
    0x95, 0x01,  //   Report Count (1)
    0x81, 0x02,  //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null
                 //   Position)
    0x05, 0x09,  //   Usage Page (Button)
    0x19, 0x0F,  //   Usage Minimum (0x0F)
    0x29, 0x12,  //   Usage Maximum (0x12)
    0x15, 0x00,  //   Logical Minimum (0)
    0x25, 0x01,  //   Logical Maximum (1)
    0x75, 0x01,  //   Report Size (1)
    0x95, 0x04,  //   Report Count (4)
    0x81, 0x02,  //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null
                 //   Position)
    0x75, 0x08,  //   Report Size (8)
    0x95, 0x34,  //   Report Count (52)
    0x81, 0x03,  //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No
                 //   Null Position)
    0x06, 0x00, 0xFF,  //   Usage Page (Vendor Defined 0xFF00)
    0x85, 0x21,        //   Report ID (33)
    0x09, 0x01,        //   Usage (0x01)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x3F,        //   Report Count (63)
    0x81, 0x03,  //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No
                 //   Null Position)
    0x85, 0x81,  //   Report ID (-127)
    0x09, 0x02,  //   Usage (0x02)
    0x75, 0x08,  //   Report Size (8)
    0x95, 0x3F,  //   Report Count (63)
    0x81, 0x03,  //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No
                 //   Null Position)
    0x85, 0x01,  //   Report ID (1)
    0x09, 0x03,  //   Usage (0x03)
    0x75, 0x08,  //   Report Size (8)
    0x95, 0x3F,  //   Report Count (63)
    0x91, 0x83,  //   Output (Const,Var,Abs,No Wrap,Linear,Preferred State,No
                 //   Null Position,Volatile)
    0x85, 0x10,  //   Report ID (16)
    0x09, 0x04,  //   Usage (0x04)
    0x75, 0x08,  //   Report Size (8)
    0x95, 0x3F,  //   Report Count (63)
    0x91, 0x83,  //   Output (Const,Var,Abs,No Wrap,Linear,Preferred State,No
                 //   Null Position,Volatile)
    0x85, 0x80,  //   Report ID (-128)
    0x09, 0x05,  //   Usage (0x05)
    0x75, 0x08,  //   Report Size (8)
    0x95, 0x3F,  //   Report Count (63)
    0x91, 0x83,  //   Output (Const,Var,Abs,No Wrap,Linear,Preferred State,No
                 //   Null Position,Volatile)
    0x85, 0x82,  //   Report ID (-126)
    0x09, 0x06,  //   Usage (0x06)
    0x75, 0x08,  //   Report Size (8)
    0x95, 0x3F,  //   Report Count (63)
    0x91, 0x83,  //   Output (Const,Var,Abs,No Wrap,Linear,Preferred State,No
                 //   Null Position,Volatile)
    0xC0,        // End Collection

    // 203 bytes
};
// TUD_HID_REPORT_DESC_GENERIC_INOUT(CFG_TUD_HID_EP_BUFSIZE)

// Invoked when received GET HID REPORT DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const* tud_hid_descriptor_report_cb(uint8_t itf) {
  (void)itf;
  return desc_hid_report;
}

//--------------------------------------------------------------------+
// Configuration Descriptor
//--------------------------------------------------------------------+

enum { ITF_NUM_HID, ITF_NUM_TOTAL };

#define CONFIG_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_HID_INOUT_DESC_LEN)

#define EPNUM_HID 0x01

uint8_t const desc_configuration[] = {
    // Config number, interface count, string index, total length, attribute,
    // power in mA
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, 0xa0, 500),

    // Interface number, string index, protocol, report descriptor len, EP In &
    // Out address, size & polling interval
    TUD_HID_INOUT_DESCRIPTOR(ITF_NUM_HID, 0, HID_ITF_PROTOCOL_NONE,
                             sizeof(desc_hid_report), EPNUM_HID,
                             0x80 | EPNUM_HID, CFG_TUD_HID_EP_BUFSIZE, 1)};

// Invoked when received GET CONFIGURATION DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const* tud_descriptor_configuration_cb(uint8_t index) {
  (void)index;  // for multiple configurations
  return desc_configuration;
}

//--------------------------------------------------------------------+
// String Descriptors
//--------------------------------------------------------------------+

// array of pointer to string descriptors
char const* string_desc_arr[] = {
    (const char[]){0x09, 0x04},  // 0: is supported language is English (0x0409)
    "Nintendo Co., Ltd.",        // 1: Manufacturer
    "Pro Controller",            // 2: Product
    "000000000001",              // 3: Serials, should use chip ID
};

static uint16_t _desc_str[32];

// Invoked when received GET STRING DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long
// enough for transfer to complete
uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
  (void)langid;

  uint8_t chr_count;

  if (index == 0) {
    memcpy(&_desc_str[1], string_desc_arr[0], 2);
    chr_count = 1;
  } else {
    // Note: the 0xEE index string is a Microsoft OS 1.0 Descriptors.
    // https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/microsoft-defined-usb-descriptors

    if (!(index < sizeof(string_desc_arr) / sizeof(string_desc_arr[0])))
      return NULL;

    const char* str = string_desc_arr[index];

    // Cap at max char
    chr_count = strlen(str);
    if (chr_count > 31) chr_count = 31;

    // Convert ASCII string into UTF-16
    for (uint8_t i = 0; i < chr_count; i++) {
      _desc_str[1 + i] = str[i];
    }
  }

  // first byte is length (including header), second byte is string type
  _desc_str[0] = (TUSB_DESC_STRING << 8) | (2 * chr_count + 2);

  return _desc_str;
}
