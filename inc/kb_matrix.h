#ifndef KB_MATRIX__H
#define KB_MATRIX__H

#include <stdlib.h>
#include <stdio.h>
#include "tusb.h"

#include "usb_descriptors.h"

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

#define KB_NUM_OF_KEY_ALTERNATE_KEY_CODE 15
#define KB_ALTERNATE_KEY_CODE {\
                  {HID_KEY_1, HID_KEY_F1}, {HID_KEY_2, HID_KEY_F2}, {HID_KEY_3, HID_KEY_F3}, {HID_KEY_4, HID_KEY_F4}, \
                  {HID_KEY_5, HID_KEY_F5}, {HID_KEY_6, HID_KEY_F6}, {HID_KEY_7, HID_KEY_F7}, {HID_KEY_8, HID_KEY_F8}, \
                  {HID_KEY_9, HID_KEY_F9}, {HID_KEY_0, HID_KEY_F10}, {HID_KEY_MINUS, HID_KEY_F11}, {HID_KEY_EQUAL, HID_KEY_F12}, \
                  {HID_KEY_PAGE_UP, HID_KEY_HOME}, {HID_KEY_PAGE_DOWN, HID_KEY_END}, \
                  {HID_KEY_DELETE, HID_KEY_INSERT} \
                 }

#define KB_NUM_OF_MEDIA_KEY_CODE 4
#define KB_MEDIA_KEY_CODE {\
                  {HID_KEY_ARROW_LEFT, USB_HID_SCAN_PREV}, {HID_KEY_ARROW_RIGHT, USB_HID_SCAN_NEXT}, \
                  {HID_KEY_ARROW_UP, USB_HID_VOL_UP}, {HID_KEY_ARROW_DOWN, USB_HID_VOL_DEC} \
                 }

#define KB_MAX_NUM_OF_KEYCODES (6+8)

typedef struct TU_ATTR_PACKED
{
  uint8_t num_of_keycodes;
  uint8_t keycode[KB_MAX_NUM_OF_KEYCODES]; /**< Key codes of the currently pressed keys. */
} kb_pressed_keycodes_t;

typedef struct TU_ATTR_PACKED
{
  uint8_t modifier;                /**< Keyboard modifier (KEYBOARD_MODIFIER_* masks). */
  uint8_t keycode[6];              /**< Key codes of the currently pressed keys. */
  uint8_t media_key;
  uint8_t fn_pressed;
} kb_report_t;

void init_kb_matrix(void);
kb_pressed_keycodes_t get_kb_keycodes(void); 
kb_report_t parse_kb_report(kb_pressed_keycodes_t kb_status);

#endif //KB_MATRIX__H