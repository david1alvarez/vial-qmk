// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#define NUM_TIMEOUT 300000  // screensaver timeout in milliseconds

void keyboard_post_init_user(void) {
    rgb_matrix_enable();
    rgb_matrix_sethsv(30, 168, 255); // Set initial color to cyan
    rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR);
}

bool is_screen_saver_active = false;

void matrix_scan_user(void) { // matrix screensaver
    if (is_screen_saver_active && last_input_activity_elapsed() < NUM_TIMEOUT) {
        rgb_matrix_sethsv(30, 168, 255);
        rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR);
        is_screen_saver_active = false;
    }
    if (!is_screen_saver_active && last_input_activity_elapsed() > NUM_TIMEOUT) {
        rgb_matrix_sethsv(0, 255, 255);
        rgb_matrix_mode(RGB_MATRIX_DIGITAL_RAIN);
        is_screen_saver_active = true;
    }
}

enum layers {
    BASE,  // default layer
    SYM,   // symbols and function keys
    EXT,  // extension layer for arrow keys and numpad
    GAME,  // gaming-compatible layer
};

// Each layer gets a name for readability, which is then used in the keymap matrix below.
// The underscores don't mean anything - you can have a layer called STUFF or any other name.
enum my_keycodes {
    RGB_R = QK_USER,
    RGB_G,
    RGB_B,
    RGB_W,
    SW_cy,
    DF_GAME,
    DF_BASE,
    CLR_FUN,
    CLR_NRM,
    CLR_GAME,
};

int rgb_patterns[9] = {
    RGB_MATRIX_BAND_SPIRAL_VAL,
    RGB_MATRIX_CYCLE_LEFT_RIGHT,
    RGB_MATRIX_CYCLE_UP_DOWN,
    RGB_MATRIX_CYCLE_OUT_IN,
    RGB_MATRIX_CYCLE_OUT_IN_DUAL,
    RGB_MATRIX_RAINBOW_MOVING_CHEVRON,
    RGB_MATRIX_CYCLE_PINWHEEL,
    RGB_MATRIX_DUAL_BEACON,
    RGB_MATRIX_RAINBOW_BEACON
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case DF_GAME:
            if (record->event.pressed) {
                rgb_matrix_sethsv(0, 255, 255);
                rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR);
                layer_move(GAME);
            }
            return false; /* Skip all further processing of this key */
        case DF_BASE:
            if (record->event.pressed) {
                rgb_matrix_sethsv(170, 255, 255);
                rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR);
                layer_move(BASE);
            }
            return false; /* Skip all further processing of this key */
        case CLR_FUN:
            if (record->event.pressed) {
                rgb_matrix_sethsv(157, 168, 255);
                rgb_matrix_mode(RGB_MATRIX_GRADIENT_UP_DOWN);
            }
            return false; /* Skip all further processing of this key */
        case CLR_NRM:
        if (record->event.pressed) {
            rgb_matrix_sethsv(30, 168, 255);
            rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR);
        }
            return false; /* Skip all further processing of this key */
        case CLR_GAME:
            if (record->event.pressed) {
                int pattern = rgb_patterns[random() % 9];
                rgb_matrix_sethsv(0, 255, 255);
                rgb_matrix_mode(pattern);
            }
            return false; /* Skip all further processing of this key */
        default:
            return true; /* Process all other keycodes normally */
    }
};


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/*
 * BASE -- QWERTY layer
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * | Esc  |   1! |   2@ |   3# |   4$ |   5% |                    |   6^ |   7& |   8* |  9(  |  0)  |  -_  |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * | `~   |   Q  |   W  |   E  |   R  |   T  |                    |   Y  |   U  |   I  |   O  |  ;:  | Bspc |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * | Tab  |   A  |   S  |   D  |   F  |   G  |-------.    ,-------|   H  |   J  |   K  |   L  |   P  |  '"  |
 * |------+------+------+------+------+------|  Caps |    | Mute  |------+------+------+------+------+------|
 * |LShift|   Z  |   X  |   C  |   V  |   B  |-------|    |-------|   N  |   M  |  ,<  |  .>  |  /?  |  \|  |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *            | LCTL | LALT | LCMD | MO(1)| /RShift /       \Space \  | Enter| MO(2)| Down |  Up  |
 *            |      |      |      |      |/       /         \      \ |      |      |      |      |
 *            '-----------------------------------'           '------''---------------------------'
 */
[BASE] = LAYOUT_split_4x6_5(
    KC_ESC,   KC_1,   KC_2,    KC_3,    KC_4,    KC_5,                           KC_6,     KC_7,     KC_8,    KC_9,    KC_0,     KC_MINUS,
    KC_GRAVE, KC_Q,   KC_W,    KC_E,    KC_R,    KC_T,                           KC_Y,     KC_U,     KC_I,    KC_O,    KC_SCLN,  KC_BSPC,
    KC_TAB,   KC_A,   KC_S,    KC_D,    KC_F,    KC_G,                           KC_H,     KC_J,     KC_K,    KC_L,    KC_P,     KC_QUOT,
    KC_LSFT,  KC_Z,   KC_X,    KC_C,    KC_V,    KC_B,    KC_CAPS,   KC_MUTE,    KC_N,     KC_M,     KC_COMM, KC_DOT,  KC_SLSH,  KC_BSLS,
                      KC_LCTL, KC_LALT, KC_LGUI, MO(SYM), KC_RSFT,   KC_SPC,     KC_ENTER, MO(2),    KC_DOWN, KC_UP
),

/*
 * SYMBOLS -- function keys and symbols
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |      |   F1 |   F2 |   F3 |   F4 |   F5 |                    |   F6 |   F7 |   F8 |  F9  |  F10 |  =+  |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |   !  |   @  |   #  |   $  |   %  |                    |   ^  |   &  |   *  |  F11 |  F12 |  Del |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |   _  |   -  |   |  |   [  |   (  |   {  |-------.    ,-------|   }  |   )  |   ]  |   =  |   +  |      |
 * |------+------+------+------+------+------|       |    |       |------+------+------+------+------+------|
 * |      |      |      |      |      |      |-------|    |-------|      |      |   ;  |   :  |   ?  |   |  |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *            | RGB M| RGB M|      |      | /       /       \      \  | TD(0)| TG(3)| Left | Right|
 *            | RNBW | PLAIN|      |      |/       /         \      \ |      |      |      |      |
 *            '-----------------------------------'           '------''---------------------------'
 */
[SYM] = LAYOUT_split_4x6_5(
    _______,       KC_F1,      KC_F2,         KC_F3,      KC_F4,      KC_F5,                           KC_F6,         KC_F7,      KC_F8,      KC_F9,         KC_F10,        KC_EQL,
    _______,       LSFT(KC_1), LSFT(KC_2),    LSFT(KC_3), LSFT(KC_4), LSFT(KC_5),                      LSFT(KC_6),    LSFT(KC_7), LSFT(KC_8), KC_F11,        KC_F12,        KC_DEL,
    LSFT(KC_MINS), KC_MINS,    LSFT(KC_BSLS), KC_LBRC,    LSFT(KC_9), LSFT(KC_LBRC),                   LSFT(KC_RBRC), LSFT(KC_0), KC_RBRC,    KC_EQL,        LSFT(KC_EQL),  _______,
    _______,       _______,    _______,       _______,    _______,    _______,     _______, _______,   _______,       _______,    KC_SCLN,    LSFT(KC_SCLN), LSFT(KC_SLSH), LSFT(KC_BSLS),
                               _______,       _______,    _______,    _______,     _______, _______,   _______,       DF_GAME,    KC_LEFT,    KC_RGHT
),

/*
 * Extension -- arrow keys and numpad numbers
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |      | Solid| RGB  |      |      |      |                    |      |      |      |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |      |      |  Up  |      |      |                    |      |      |      |  7   |  8   |  9   |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |      | Left | Down | Right|      |-------.    ,-------|      |      |      |  4   |  5   |  6   |
 * |------+------+------+------+------+------|       |    |       |------+------+------+------+------+------|
 * |      |      |      |      |      |      |-------|    |-------|      |      |      |  1   |  2   |  3   |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *            |      |      |      | GAME |/       /       \      \  |      |      |      |  0   |
 *            |      |      |      |      |/       /         \      \ |      |      |      |      |
 *            '-----------------------------------'           '------''---------------------------'
 */
 [EXT] = LAYOUT_split_4x6_5(
    _______,CLR_NRM,CLR_FUN, _______,_______,_______,                           _______,_______,_______,_______,_______,_______,
    _______,_______,_______,KC_UP,  _______,_______,                           _______,_______,_______,KC_KP_7,KC_KP_8,KC_KP_9,
    _______,_______,KC_LEFT,KC_DOWN,KC_RGHT,_______,                           _______,_______,_______,KC_KP_4,KC_KP_5,KC_KP_6,
    _______,_______,_______,_______,_______,_______,   _______,     _______,   _______,_______,_______,KC_KP_1,KC_KP_2,KC_KP_3,
                    _______,_______,_______,DF_GAME,   _______,     _______,   _______,_______,_______,KC_KP_0
),

/*
 * GAME -- gaming-compatible layer
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |      |      |      |      |      |      |                    |      |      |      |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |      |      |      |      |      |                    |      |      |      |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |      |      |      |      |      |-------.    ,-------|      |      |      |      |      |      |
 * |------+------+------+------+------+------|       |    |       |------+------+------+------+------+------|
 * |      |      |      |      |      |      |-------|    |-------|      |      |      |      |      |      |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *            |      |      | LCtrl| Space| / MO(1) /       \      \  |      | TG(3)|      |      |
 *            |      |      |      |      |/       /         \      \ |      |      |      |      |
 *            '-----------------------------------'           '------''---------------------------'
 */
 [GAME] = LAYOUT_split_4x6_5(
    KC_ESC,   KC_1,   KC_2,    KC_3,    KC_4,    KC_5,                         KC_6,     KC_7,    KC_8,    KC_9,    KC_0,     KC_MINUS,
    KC_GRAVE, KC_Q,   KC_W,    KC_E,    KC_R,    KC_T,                         KC_Y,     KC_U,    KC_I,    KC_O,    KC_SCLN,  KC_BSPC,
    KC_TAB,   KC_A,   KC_S,    KC_D,    KC_F,    KC_G,                         KC_H,     KC_J,    KC_K,    KC_L,    KC_P,     KC_QUOT,
    KC_LSFT,  KC_Z,   KC_X,    KC_C,    KC_V,    KC_B,    KC_CAPS,   KC_MUTE,  KC_N,     KC_M,    KC_COMM, KC_DOT,  KC_SLSH,  KC_BSLS,
                      KC_LCTL, KC_LALT, KC_LCTL, KC_SPC,  MO(SYM),   KC_SPC,   KC_ENTER, DF_BASE, KC_DOWN, KC_UP
),
};

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [BASE] = { ENCODER_CCW_CW(KC_WH_D, KC_WH_U), ENCODER_CCW_CW(KC_VOLD, KC_VOLU) },
    [SYM] = { ENCODER_CCW_CW(_______, _______), ENCODER_CCW_CW(_______, _______) },
    [EXT] = { ENCODER_CCW_CW(_______, _______), ENCODER_CCW_CW(_______, _______) },
    [GAME] = { ENCODER_CCW_CW(_______, _______), ENCODER_CCW_CW(_______, _______) }
};
#endif


