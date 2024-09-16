#include <stdlib.h>
#include "pico/stdlib.h"
#include "kb_matrix.h"
#include "usb_descriptors.h"

const uint kb_columns[KB_NUM_OF_COLS] = KB_COL_PINS;
const uint kb_rows[KB_NUM_OF_ROWS] = KB_ROW_PINS;
const uint kb_key_codes[KB_NUM_OF_ROWS][KB_NUM_OF_COLS] = KB_KEY_CODES;
const uint kb_alternate_key_codes[KB_NUM_OF_KEY_ALTERNATE_KEY_CODE][2] = KB_ALTERNATE_KEY_CODE;
const uint kb_media_key_codes[KB_NUM_OF_MEDIA_KEY_CODE][2] = KB_MEDIA_KEY_CODE;


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

kb_report_t parse_kb_report(kb_pressed_keycodes_t kb_status){
  kb_report_t report;
  memset(&report, 0, sizeof(report));
  uint cur_keycode_idx = 0;
  uint num_of_parsed_keycodes = 0;

  for(uint status_keycode_idx = 0; status_keycode_idx < kb_status.num_of_keycodes; status_keycode_idx++){
    switch(kb_status.keycode[status_keycode_idx]){
      case HID_KEY_CONTROL_LEFT:{
        report.modifier = report.modifier | KEYBOARD_MODIFIER_LEFTCTRL;
        break;
      }
      case HID_KEY_SHIFT_LEFT:{
        report.modifier = report.modifier | KEYBOARD_MODIFIER_LEFTSHIFT;
        break;
      }
      case HID_KEY_ALT_LEFT:{
        report.modifier = report.modifier | KEYBOARD_MODIFIER_LEFTALT;
        break;
      }
      case HID_KEY_GUI_LEFT:{
        report.modifier = report.modifier | KEYBOARD_MODIFIER_LEFTGUI;
        break;
      }
      case HID_KEY_CONTROL_RIGHT:{
        report.modifier = report.modifier | KEYBOARD_MODIFIER_RIGHTCTRL;
        break;
      }
      case HID_KEY_SHIFT_RIGHT:{
        report.modifier = report.modifier | KEYBOARD_MODIFIER_RIGHTSHIFT;
        break;
      }
      case HID_KEY_ALT_RIGHT:{
        report.modifier = report.modifier | KEYBOARD_MODIFIER_RIGHTALT;
        break;
      }
      case HID_KEY_GUI_RIGHT:{
        //report.modifier = report.modifier | KEYBOARD_MODIFIER_RIGHTGUI;
        report.fn_pressed = 1;
        break;
      }
      default:{
        report.keycode[cur_keycode_idx] = kb_status.keycode[status_keycode_idx];
        cur_keycode_idx ++;
        break;
      }
    }
  }

  num_of_parsed_keycodes = cur_keycode_idx;

  // Parse alternate keycodes
  if(report.fn_pressed != 0){
    for(uint keycode_idx=0; keycode_idx<num_of_parsed_keycodes; keycode_idx++){
      for(uint i=0; i<KB_NUM_OF_KEY_ALTERNATE_KEY_CODE; i++){
        if(report.keycode[keycode_idx] == kb_alternate_key_codes[i][0]){
          report.keycode[keycode_idx] = kb_alternate_key_codes[i][1];
          break;
        }
      }
    }
  }

  // Parse media keycodes
  if(report.fn_pressed != 0){
    for(uint keycode_idx=0; keycode_idx<num_of_parsed_keycodes; keycode_idx++){
      for(uint i=0; i<KB_NUM_OF_MEDIA_KEY_CODE; i++){
        if(report.keycode[keycode_idx] == kb_media_key_codes[i][0]){
          report.media_key |= kb_media_key_codes[i][1];
          break;
        }
      }
    }
  }

  return report;
}