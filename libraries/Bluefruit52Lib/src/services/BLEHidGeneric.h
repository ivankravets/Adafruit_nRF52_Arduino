/**************************************************************************/
/*!
    @file     BLEHidGeneric.h
    @author   hathach (tinyusb.org)

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2018, Adafruit Industries (adafruit.com)
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holders nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/**************************************************************************/
#ifndef BLEHIDGENERIC_H_
#define BLEHIDGENERIC_H_

#include "bluefruit_common.h"

#include "BLECharacteristic.h"
#include "BLEService.h"

// for HID definitions
#include "tusb.h"

enum
{
  HID_PROTOCOL_MODE_BOOT   = 0,
  HID_PROTOCOL_MODE_REPORT = 1
};

typedef struct{
  uint8_t shift;
  uint8_t keycode;
}hid_ascii_to_keycode_entry_t;
extern const hid_ascii_to_keycode_entry_t hid_ascii_to_keycode[128];

typedef struct{
  uint8_t ascii;
  uint8_t shifted;
}hid_keycode_to_ascii_t;
extern hid_keycode_to_ascii_t const hid_keycode_to_ascii[128];


/// HID Consumer Control Report
typedef ATTR_PACKED_STRUCT(struct)
{
  uint16_t usage_value; ///< Usage value of the pressed control
} hid_consumer_control_report_t;

/// Gamepad report
typedef ATTR_PACKED_STRUCT(struct)
{
  ATTR_PACKED_STRUCT(struct){
    uint8_t x : 2;
    uint8_t y : 2;
    uint8_t : 4;
  };

  uint8_t buttons;
}hid_gamepad_report_t;


class BLEHidGeneric : public BLEService
{
  public:
    BLEHidGeneric(uint8_t num_input, uint8_t num_output = 0, uint8_t num_feature = 0);

    void enableKeyboard(bool enable);
    void enableMouse(bool enable);

    void setHidInfo(uint16_t bcd, uint8_t country, uint8_t flags);

    void setReportLen(uint16_t input_len[], uint16_t output_len[] = NULL, uint16_t feature_len[] = NULL);
    void setReportMap(const uint8_t* report_map, size_t len);

    void setOutputReportCallback(uint8_t reportID, BLECharacteristic::write_cb_t fp);

    virtual err_t begin(void);

    bool isBootMode(void) { return _protocol_mode == HID_PROTOCOL_MODE_BOOT; }

    // Send Report to default connection
    bool inputReport(uint8_t reportID, void const* data, int len);
    bool bootKeyboardReport(void const* data, int len);
    bool bootMouseReport(void const* data, int len);

    // Send report to specific connection
    bool inputReport(uint16_t conn_hdl, uint8_t reportID, void const* data, int len);
    bool bootKeyboardReport(uint16_t conn_hdl, void const* data, int len);
    bool bootMouseReport(uint16_t conn_hdl, void const* data, int len);

  protected:
    uint8_t _num_input;
    uint8_t _num_output;
    uint8_t _num_feature;

    bool    _has_keyboard;
    bool    _has_mouse;
    bool    _protocol_mode;

    uint8_t _hid_info[4];
    const uint8_t* _report_map;
    size_t _report_map_len;

    uint16_t* _input_len;
    uint16_t* _output_len;
    uint16_t* _feature_len;

    BLECharacteristic* _chr_protocol;

    BLECharacteristic* _chr_inputs;
    BLECharacteristic* _chr_outputs;
    BLECharacteristic* _chr_features;

    BLECharacteristic* _chr_boot_keyboard_input;
    BLECharacteristic* _chr_boot_keyboard_output;
    BLECharacteristic* _chr_boot_mouse_input;

    BLECharacteristic _chr_control;

    static void blehid_generic_protocol_mode_cb(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len);
};

#endif /* BLEHIDGENERIC_H_ */
