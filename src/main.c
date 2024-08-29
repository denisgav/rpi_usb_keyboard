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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bsp/board_api.h"
#include "tusb.h"

#include "usb_descriptors.h"

#include "pico/stdlib.h"


#define KB_NUM_OF_KEYS 68
#define KB_NUM_OF_COLS 15
#define KB_COL_PIN_0 7
#define KB_COL_PIN_1 8
#define KB_COL_PIN_2 9
#define KB_COL_PIN_3 10
#define KB_COL_PIN_4 11
#define KB_COL_PIN_5 12
#define KB_COL_PIN_6 13
#define KB_COL_PIN_7 14
#define KB_COL_PIN_8 15
#define KB_COL_PIN_9 16
#define KB_COL_PIN_10 17
#define KB_COL_PIN_11 18
#define KB_COL_PIN_12 19
#define KB_COL_PIN_13 20
#define KB_COL_PIN_14 21
#define KB_COL_PINS { \
    KB_COL_PIN_0, KB_COL_PIN_1, KB_COL_PIN_2, KB_COL_PIN_3, \
    KB_COL_PIN_4, KB_COL_PIN_5, KB_COL_PIN_6, KB_COL_PIN_7, \
    KB_COL_PIN_8, KB_COL_PIN_9, KB_COL_PIN_10, KB_COL_PIN_11, \
    KB_COL_PIN_12, KB_COL_PIN_13, KB_COL_PIN_14 \
  }\

#define KB_NUM_OF_ROWS 5
#define KB_ROW_PIN_0 2
#define KB_ROW_PIN_1 3
#define KB_ROW_PIN_2 4
#define KB_ROW_PIN_3 5
#define KB_ROW_PIN_4 6
#define KB_ROW_PINS { \
    KB_ROW_PIN_0, KB_ROW_PIN_1, KB_ROW_PIN_2, KB_ROW_PIN_3, \
    KB_ROW_PIN_4}\


#define KB_KEY_CODES { \
                    /* ["¬\n`","!\n1","\"\n2","£\n3","$\n4","%\n5","^\n6","&\n7","*\n8","(\n9",")\n0","_\n-","+\n=",{w:2},"Backspace"], */ \
                    /*0,0*/{HID_KEY_ESCAPE, HID_KEY_1, HID_KEY_2, HID_KEY_3, HID_KEY_4, HID_KEY_5, HID_KEY_6, HID_KEY_7, \
                     HID_KEY_8, HID_KEY_9, HID_KEY_0, HID_KEY_MINUS, HID_KEY_EQUAL, HID_KEY_BACKSPACE, HID_KEY_GRAVE}, /*0,15*/ \
                    \
                    /* [{w:1.5},"Tab","Q","W","E","R","T","Y","U","I","O","P","{\n[","}\n]",{x:0.25,w:1.25,h:2,w2:1.5,h2:1,x2:-0.25},"Enter"], */ \
                    /*1,0*/{HID_KEY_TAB, HID_KEY_Q, HID_KEY_W, HID_KEY_E, HID_KEY_R, HID_KEY_T, HID_KEY_Y, HID_KEY_U, \
                     HID_KEY_I, HID_KEY_O, HID_KEY_P, HID_KEY_BRACKET_LEFT, HID_KEY_BRACKET_RIGHT, HID_KEY_BACKSLASH, HID_KEY_PAGE_UP}, /*1,15*/ \
                    \
                    /* [{w:1.75},"Caps Lock","A","S","D","F","G","H","J","K","L",":\n;","@\n'","~\n#"],*/ \
                    /*2,0*/{HID_KEY_CAPS_LOCK, HID_KEY_A, HID_KEY_S, HID_KEY_D, HID_KEY_F, HID_KEY_G, HID_KEY_H, HID_KEY_J, \
                     HID_KEY_K, HID_KEY_L, HID_KEY_SEMICOLON, HID_KEY_APOSTROPHE, HID_KEY_NONE, HID_KEY_ENTER, HID_KEY_PAGE_DOWN}, /*2,15*/ \
                    \
                    /* [{w:1.25},"Shift","|\n\\","Z","X","C","V","B","N","M","<\n,",">\n.","?\n/",{w:2.75},"Shift"],*/ \
                    /*3,0*/{HID_KEY_SHIFT_LEFT, HID_KEY_Z, HID_KEY_X, HID_KEY_C, HID_KEY_V, HID_KEY_B, HID_KEY_N, HID_KEY_M, \
                     HID_KEY_COMMA, HID_KEY_PERIOD, HID_KEY_SLASH, HID_KEY_NONE, HID_KEY_SHIFT_RIGHT, HID_KEY_ARROW_UP, HID_KEY_DELETE}, /*3,15*/ \
                    \
                    /* [{w:1.25},"Ctrl",{w:1.25},"Win",{w:1.25},"Alt",{a:7,w:6.25},"",{a:4,w:1.25},"AltGr",{w:1.25},"Win",{w:1.25},"Menu",{w:1.25},"Ctrl"] */ \
                    /*4,0*/{HID_KEY_CONTROL_LEFT, HID_KEY_GUI_LEFT, HID_KEY_ALT_LEFT, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_SPACE, HID_KEY_NONE, HID_KEY_NONE, \
                     HID_KEY_NONE, HID_KEY_ALT_RIGHT, HID_KEY_GUI_RIGHT, HID_KEY_CONTROL_RIGHT, HID_KEY_ARROW_LEFT, HID_KEY_ARROW_DOWN, HID_KEY_ARROW_RIGHT} /*4,15*/ \
                  }

const uint kb_columns[KB_NUM_OF_COLS] = KB_COL_PINS;
const uint kb_rows[KB_NUM_OF_ROWS] = KB_ROW_PINS;
const uint kb_key_codes[KB_NUM_OF_ROWS][KB_NUM_OF_COLS] = KB_KEY_CODES;

#define KB_MAX_NUM_OF_KEYCODES (6+8)

typedef struct TU_ATTR_PACKED
{
  uint8_t num_of_keycodes;
  uint8_t keycode[KB_MAX_NUM_OF_KEYCODES]; /**< Key codes of the currently pressed keys. */
} kb_pressed_keycodes_t;


//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+

/* Blink pattern
 * - 250 ms  : device not mounted
 * - 1000 ms : device mounted
 * - 2500 ms : device is suspended
 */
enum  {
  BLINK_NOT_MOUNTED = 250,
  BLINK_MOUNTED = 1000,
  BLINK_SUSPENDED = 2500,
};

static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;

void led_blinking_task(void);
void hid_task(void);

void init_kb_matrix(void);
kb_pressed_keycodes_t get_kb_keycodes(void); 
void send_hid_report(kb_pressed_keycodes_t kb_status);

/*------------- MAIN -------------*/
int main(void)
{
  board_init();

  // init device stack on configured roothub port
  tud_init(BOARD_TUD_RHPORT);

  init_kb_matrix();

  if (board_init_after_tusb) {
    board_init_after_tusb();
  }

  while (1)
  {
    tud_task(); // tinyusb device task
    led_blinking_task();

    hid_task();
  }
}

//--------------------------------------------------------------------+
// Serving keyboard matrix
//--------------------------------------------------------------------+

void init_kb_matrix(void){
  for (int col_idx = 0; col_idx < KB_NUM_OF_COLS; col_idx++) {
    gpio_init(kb_columns[col_idx]);
    gpio_set_dir(kb_columns[col_idx], GPIO_OUT);

    gpio_put(kb_columns[col_idx], 0);
  }

  for (int row_idx = 0; row_idx < KB_NUM_OF_ROWS; row_idx++) {
    gpio_init(kb_rows[row_idx]);
    gpio_set_dir(kb_rows[row_idx], GPIO_IN);
  }
}

kb_pressed_keycodes_t get_kb_keycodes(void){
  kb_pressed_keycodes_t res;
  // Clear structure
  memset(&res, 0, sizeof(res));

  for (int col_idx = 0; col_idx < KB_NUM_OF_COLS; col_idx++) {
    gpio_put(kb_columns[col_idx], 1);
    sleep_ms(1);
    for (int row_idx = 0; row_idx < KB_NUM_OF_ROWS; row_idx++) {
      if(gpio_get(kb_rows[row_idx])){
        if(res.num_of_keycodes < KB_MAX_NUM_OF_KEYCODES){
          uint8_t keycode = kb_key_codes[row_idx][col_idx];
          if(keycode != HID_KEY_NONE){
            res.keycode[res.num_of_keycodes] = keycode;
            res.num_of_keycodes ++;
          }
        }
      }
    }
    gpio_put(kb_columns[col_idx], 0);
    sleep_ms(1);
  }

  return res;
}

//--------------------------------------------------------------------+
//--------------------------------------------------------------------+

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
  blink_interval_ms = BLINK_MOUNTED;
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
  blink_interval_ms = BLINK_NOT_MOUNTED;
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
  (void) remote_wakeup_en;
  blink_interval_ms = BLINK_SUSPENDED;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
  blink_interval_ms = tud_mounted() ? BLINK_MOUNTED : BLINK_NOT_MOUNTED;
}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+
void send_hid_report(kb_pressed_keycodes_t kb_status){
  // skip if hid is not ready yet
  if ( !tud_hid_ready() ) return;

  uint8_t modifier = 0;                /**< Keyboard modifier (KEYBOARD_MODIFIER_* masks). */
  uint8_t keycode[6] = { 0,0,0, 0,0,0 }; /**< Key codes of the currently pressed keys. */
  uint cur_keycode_idx = 0;

  for(uint status_keycode_idx = 0; status_keycode_idx < kb_status.num_of_keycodes; status_keycode_idx++){
    switch(kb_status.keycode[status_keycode_idx]){
      case HID_KEY_CONTROL_LEFT:{
        modifier = modifier | KEYBOARD_MODIFIER_LEFTCTRL;
        break;
      }
      case HID_KEY_SHIFT_LEFT:{
        modifier = modifier | KEYBOARD_MODIFIER_LEFTSHIFT;
        break;
      }
      case HID_KEY_ALT_LEFT:{
        modifier = modifier | KEYBOARD_MODIFIER_LEFTALT;
        break;
      }
      case HID_KEY_GUI_LEFT:{
        modifier = modifier | KEYBOARD_MODIFIER_LEFTGUI;
        break;
      }
      case HID_KEY_CONTROL_RIGHT:{
        modifier = modifier | KEYBOARD_MODIFIER_RIGHTCTRL;
        break;
      }
      case HID_KEY_SHIFT_RIGHT:{
        modifier = modifier | KEYBOARD_MODIFIER_RIGHTSHIFT;
        break;
      }
      case HID_KEY_ALT_RIGHT:{
        modifier = modifier | KEYBOARD_MODIFIER_RIGHTALT;
        break;
      }
      case HID_KEY_GUI_RIGHT:{
        modifier = modifier | KEYBOARD_MODIFIER_RIGHTGUI;
        break;
      }
      default:{
        keycode[cur_keycode_idx] = kb_status.keycode[status_keycode_idx];
        cur_keycode_idx ++;
        break;
      }
    }
  }

  static bool prev_report_is_not_empty = false;

  if((kb_status.num_of_keycodes != 0) || prev_report_is_not_empty){
    tud_hid_keyboard_report(REPORT_ID_KEYBOARD, modifier, keycode);
  }
  prev_report_is_not_empty = (kb_status.num_of_keycodes != 0);
}

// static void send_hid_report(uint8_t report_id, uint32_t btn)
// {
//   // skip if hid is not ready yet
//   if ( !tud_hid_ready() ) return;

//   switch(report_id)
//   {
//     case REPORT_ID_KEYBOARD:
//     {
//       // use to avoid send multiple consecutive zero report for keyboard
//       static bool has_keyboard_key = false;

//       if ( btn )
//       {
//         uint8_t keycode[6] = { 0 };
//         keycode[0] = HID_KEY_A;

//         tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keycode);
//         has_keyboard_key = true;
//       }else
//       {
//         // send empty key report if previously has key pressed
//         if (has_keyboard_key) tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
//         has_keyboard_key = false;
//       }
//     }
//     break;

//     case REPORT_ID_MOUSE:
//     {
//       int8_t const delta = 5;

//       // no button, right + down, no scroll, no pan
//       tud_hid_mouse_report(REPORT_ID_MOUSE, 0x00, delta, delta, 0, 0);
//     }
//     break;

//     case REPORT_ID_CONSUMER_CONTROL:
//     {
//       // use to avoid send multiple consecutive zero report
//       static bool has_consumer_key = false;

//       if ( btn )
//       {
//         // volume down
//         uint16_t volume_down = HID_USAGE_CONSUMER_VOLUME_DECREMENT;
//         tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &volume_down, 2);
//         has_consumer_key = true;
//       }else
//       {
//         // send empty key report (release key) if previously has key pressed
//         uint16_t empty_key = 0;
//         if (has_consumer_key) tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &empty_key, 2);
//         has_consumer_key = false;
//       }
//     }
//     break;

//     case REPORT_ID_GAMEPAD:
//     {
//       // use to avoid send multiple consecutive zero report for keyboard
//       static bool has_gamepad_key = false;

//       hid_gamepad_report_t report =
//       {
//         .x   = 0, .y = 0, .z = 0, .rz = 0, .rx = 0, .ry = 0,
//         .hat = 0, .buttons = 0
//       };

//       if ( btn )
//       {
//         report.hat = GAMEPAD_HAT_UP;
//         report.buttons = GAMEPAD_BUTTON_A;
//         tud_hid_report(REPORT_ID_GAMEPAD, &report, sizeof(report));

//         has_gamepad_key = true;
//       }else
//       {
//         report.hat = GAMEPAD_HAT_CENTERED;
//         report.buttons = 0;
//         if (has_gamepad_key) tud_hid_report(REPORT_ID_GAMEPAD, &report, sizeof(report));
//         has_gamepad_key = false;
//       }
//     }
//     break;

//     default: break;
//   }
// }

// Every 10ms, we will sent 1 report for each HID profile (keyboard, mouse etc ..)
// tud_hid_report_complete_cb() is used to send the next report after previous one is complete
void hid_task(void)
{
  // Poll every 10ms
  const uint32_t interval_ms = 10;
  static uint32_t start_ms = 0;

  if ( board_millis() - start_ms < interval_ms) return; // not enough time
  start_ms += interval_ms;

  kb_pressed_keycodes_t kb_status = get_kb_keycodes();
  send_hid_report(kb_status);

  //uint32_t const btn = board_button_read();
  //
  // // Remote wakeup
  // if ( tud_suspended() && btn )
  // {
  //   // Wake up host if we are in suspend mode
  //   // and REMOTE_WAKEUP feature is enabled by host
  //   tud_remote_wakeup();
  // }else
  // {
  //   // Send the 1st of report chain, the rest will be sent by tud_hid_report_complete_cb()
  //   send_hid_report(REPORT_ID_KEYBOARD, btn);
  // }
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint16_t len)
{
  (void) instance;
  (void) len;

  // uint8_t next_report_id = report[0] + 1u;

  // if (next_report_id < REPORT_ID_COUNT)
  // {
  //   send_hid_report(next_report_id, board_button_read());
  // }
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  // TODO not Implemented
  (void) instance;
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) reqlen;

  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
  (void) instance;

  // if (report_type == HID_REPORT_TYPE_OUTPUT)
  // {
  //   // Set keyboard LED e.g Capslock, Numlock etc...
  //   if (report_id == REPORT_ID_KEYBOARD)
  //   {
  //     // bufsize should be (at least) 1
  //     if ( bufsize < 1 ) return;

  //     uint8_t const kbd_leds = buffer[0];

  //     if (kbd_leds & KEYBOARD_LED_CAPSLOCK)
  //     {
  //       // Capslock On: disable blink, turn led on
  //       blink_interval_ms = 0;
  //       board_led_write(true);
  //     }else
  //     {
  //       // Caplocks Off: back to normal blink
  //       board_led_write(false);
  //       blink_interval_ms = BLINK_MOUNTED;
  //     }
  //   }
  // }
}

//--------------------------------------------------------------------+
// BLINKING TASK
//--------------------------------------------------------------------+
void led_blinking_task(void)
{
  static uint32_t start_ms = 0;
  static bool led_state = false;

  // blink is disabled
  if (!blink_interval_ms) return;

  // Blink every interval ms
  if ( board_millis() - start_ms < blink_interval_ms) return; // not enough time
  start_ms += blink_interval_ms;

  board_led_write(led_state);
  led_state = 1 - led_state; // toggle
}
