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

#include "pico/multicore.h"

#include "main.h"

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

void core1_entry();

kb_pressed_keycodes_t core1_kb_status;
auto_init_mutex(my_mutex);

/*------------- MAIN -------------*/
int main(void)
{
  init_kb_matrix();
  board_init();

  // init device stack on configured roothub port
  tud_init(BOARD_TUD_RHPORT);

  if (board_init_after_tusb) {
    board_init_after_tusb();
  }

  multicore_launch_core1(core1_entry);

  while (1)
  {
    tud_task(); // tinyusb device task
    led_blinking_task();

    hid_task();
  }
}


void core1_entry(){
  while(true){
    uint32_t owner_out;
    kb_pressed_keycodes_t lcl_kb_status = get_kb_keycodes();
    if (mutex_try_enter(&my_mutex,&owner_out)){
      core1_kb_status = lcl_kb_status;
      mutex_exit(&my_mutex);
    }
  }
}
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

static void send_hid_report(uint8_t report_id, kb_pressed_keycodes_t kb_status)
{
  // // skip if hid is not ready yet
  // if ( !tud_hid_ready() ) return;

  // kb_report_t report = parse_kb_report(kb_status);

  // switch(report_id)
  // {
  //   case REPORT_ID_KEYBOARD:
  //   {
  //     // use to avoid send multiple consecutive zero report for keyboard
  //     static bool has_keyboard_key = false;

  //     if((kb_status.num_of_keycodes != 0) && (report.media_key == 0))
  //     {
  //       tud_hid_keyboard_report(REPORT_ID_KEYBOARD, report.modifier, report.keycode);
  //       has_keyboard_key = true;
  //     }else
  //     {
  //       // send empty key report if previously has key pressed
  //       if (has_keyboard_key) tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
  //       has_keyboard_key = false;
  //     }
  //   }
  //   break;

  //   case REPORT_ID_CONSUMER_CONTROL:
  //   {
  //     // use to avoid send multiple consecutive zero report
  //     static bool has_consumer_key = false;

  //     if((report.fn_pressed != 0) && (report.media_key != 0))
  //     {
  //       // volume down
  //       uint16_t volume_down = HID_USAGE_CONSUMER_VOLUME_DECREMENT;
  //       tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &volume_down, 2);
  //       has_consumer_key = true;
  //     }else
  //     {
  //       // send empty key report (release key) if previously has key pressed
  //       uint16_t empty_key = 0;
  //       if (has_consumer_key) tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &empty_key, 2);
  //       has_consumer_key = false;
  //     }
  //   }
  //   break;

  //   default: break;
  // }

  static bool prev_media_report_is_not_empty = false;
  static bool prev_kb_report_is_not_empty = false;

  kb_report_t report = parse_kb_report(kb_status);


  // Send media report
  if(((report.fn_pressed != 0) && (report.media_key != 0)) || prev_media_report_is_not_empty){
    tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &(report.media_key), sizeof(report.media_key));
  }
  prev_media_report_is_not_empty = ((report.fn_pressed != 0) && (report.media_key != 0)) ;

  //Send KB report
  if(((kb_status.num_of_keycodes != 0) && (report.media_key == 0)) || prev_kb_report_is_not_empty){
    tud_hid_keyboard_report(REPORT_ID_KEYBOARD, report.modifier, report.keycode);
  }
  prev_kb_report_is_not_empty = ((kb_status.num_of_keycodes != 0) && (report.media_key == 0));
}

// Every 10ms, we will sent 1 report for each HID profile (keyboard, mouse etc ..)
// tud_hid_report_complete_cb() is used to send the next report after previous one is complete
void hid_task(void)
{
  // Poll every 10ms
  const uint32_t interval_ms = 10;
  static uint32_t start_ms = 0;

  if ( board_millis() - start_ms < interval_ms) return; // not enough time
  start_ms += interval_ms;

  //uint32_t const btn = board_button_read();
  kb_pressed_keycodes_t kb_status;
  uint32_t owner_out;
  if (mutex_try_enter(&my_mutex,&owner_out)){
    kb_status = core1_kb_status;
    mutex_exit(&my_mutex);
  }

  // Remote wakeup
  //if ( tud_suspended() && btn )
  if ( tud_suspended() &&  (kb_status.num_of_keycodes != 0))
  {
    // Wake up host if we are in suspend mode
    // and REMOTE_WAKEUP feature is enabled by host
    tud_remote_wakeup();
  }else
  {
    // Send the 1st of report chain, the rest will be sent by tud_hid_report_complete_cb()
    send_hid_report(REPORT_ID_KEYBOARD, kb_status);
  }
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint16_t len)
{
  (void) instance;
  (void) len;

  uint8_t next_report_id = report[0] + 1u;

  // if (next_report_id < REPORT_ID_COUNT)
  // {
  //   kb_pressed_keycodes_t kb_status;
  //   uint32_t owner_out;
  //   if (mutex_try_enter(&my_mutex,&owner_out)){
  //     kb_status = core1_kb_status;
  //     mutex_exit(&my_mutex);
  //   }
  //   send_hid_report(next_report_id, kb_status);
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

  if (report_type == HID_REPORT_TYPE_OUTPUT)
  {
    // Set keyboard LED e.g Capslock, Numlock etc...
    if (report_id == REPORT_ID_KEYBOARD)
    {
      // bufsize should be (at least) 1
      if ( bufsize < 1 ) return;

      uint8_t const kbd_leds = buffer[0];

      if (kbd_leds & KEYBOARD_LED_CAPSLOCK)
      {
        // Capslock On: disable blink, turn led on
        blink_interval_ms = 0;
        board_led_write(true);
      }else
      {
        // Caplocks Off: back to normal blink
        board_led_write(false);
        blink_interval_ms = BLINK_MOUNTED;
      }
    }
  }
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
