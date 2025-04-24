// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#define NUM_TIMEOUT 20 * 60 * 1000  // 10 min milliseconds

// Layer state preservation for returning to the last-used layer
bool is_screen_saver_active = false;
uint16_t last_pattern = RGB_MATRIX_SOLID_COLOR;
uint16_t base_layer_pattern = RGB_MATRIX_SOLID_COLOR;

// Matrix setting util
void set_matrix(uint16_t pattern) {
    uint16_t rgb_val = rgb_matrix_get_val();
    switch (pattern) {
        case RGB_MATRIX_BAND_SPIRAL_VAL:
        case RGB_MATRIX_CYCLE_LEFT_RIGHT:
        case RGB_MATRIX_CYCLE_UP_DOWN:
        case RGB_MATRIX_CYCLE_OUT_IN:
        case RGB_MATRIX_CYCLE_OUT_IN_DUAL:
        case RGB_MATRIX_RAINBOW_MOVING_CHEVRON:
        case RGB_MATRIX_CYCLE_PINWHEEL:
        case RGB_MATRIX_DUAL_BEACON:
        case RGB_MATRIX_RAINBOW_BEACON:
            rgb_matrix_set_speed_noeeprom(RGB_MATRIX_DEFAULT_SPD);
            rgb_matrix_sethsv_noeeprom(0, 255, rgb_val);
            rgb_matrix_mode_noeeprom(pattern);
            break;
        case RGB_MATRIX_DIGITAL_RAIN:
            rgb_matrix_set_speed_noeeprom(150);
            rgb_matrix_sethsv_noeeprom(0, 255, rgb_val);
            rgb_matrix_mode_noeeprom(pattern);
            break;
        case RGB_MATRIX_GRADIENT_UP_DOWN:
            rgb_matrix_set_speed(100);
            rgb_matrix_sethsv(175, 200, rgb_val);
            rgb_matrix_mode(pattern);
            break;
        case RGB_MATRIX_SOLID_COLOR:
            rgb_matrix_set_speed(RGB_MATRIX_DEFAULT_SPD);
            rgb_matrix_sethsv(30, 200, rgb_val);
            rgb_matrix_mode(pattern);
            break;
        default:
            rgb_matrix_set_speed(RGB_MATRIX_DEFAULT_SPD);
            rgb_matrix_sethsv(30, 200, rgb_val);
            rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR);
            break;
    }
}

void reset_rgb_val(void) {
    uint8_t hue = rgb_matrix_get_hue();
    uint8_t sat = rgb_matrix_get_sat();
    rgb_matrix_sethsv(hue, sat, RGB_MATRIX_DEFAULT_VAL);
}

// Per-tick check, use for screensaver detection
void matrix_scan_user(void) { // matrix screensaver
    if (is_screen_saver_active && last_input_activity_elapsed() < NUM_TIMEOUT) {
        set_matrix(last_pattern);
        is_screen_saver_active = false;
    }
    if (!is_screen_saver_active && last_input_activity_elapsed() > NUM_TIMEOUT) {
        last_pattern = rgb_matrix_get_mode();
        set_matrix(RGB_MATRIX_DIGITAL_RAIN);
        is_screen_saver_active = true;
    }
}

// RGB favorite patterns, for use in randomly selecting a pattern
int rgb_patterns[] = {
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

enum layers {
    BASE,  // default layer
    GAME,  // gaming-compatible layer
    SYM,   // symbols and function keys
    EXT,  // extension layer for arrow keys and numpad
};

// Keypress intercept
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (is_screen_saver_active) {
        return false;
    }  
    switch (keycode) {
        case TO(GAME):
            if (record->event.pressed) {
                if (IS_LAYER_ON(GAME)) {
                    set_matrix(base_layer_pattern);
                    layer_off(GAME);
                } else {
                    int pattern_length = sizeof(rgb_patterns) / sizeof(rgb_patterns[0]);
                    int pattern = rgb_patterns[random() % pattern_length];
                    set_matrix(pattern);
                    layer_move(GAME);
                }
            }
            return false;
        case PB_1:
            if (record->event.pressed) {
                base_layer_pattern = RGB_MATRIX_SOLID_COLOR;
                set_matrix(RGB_MATRIX_SOLID_COLOR);
            }
            return false;
        case PB_2:
            if (record->event.pressed) {
                base_layer_pattern = RGB_MATRIX_GRADIENT_UP_DOWN;
                set_matrix(RGB_MATRIX_GRADIENT_UP_DOWN);
            }
            return false;
        case PB_3:
            if(record->event.pressed) {
                reset_rgb_val();
            }
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
 * `-----------------------------------------/       /    \       \-----------------------------------------'
 *            | LCTL | LALT | LCMD | MO   | /RShift /      \ Space \  | Enter| MO   | Down |  Up  |
 *            |      |      |      | SYM  |/       /        \       \ |      | EXT  |      |      |
 *            '-----------------------------------'          '-------''---------------------------'
 */
[BASE] = LAYOUT_split_4x6_5(
    KC_ESC,   KC_1,   KC_2,    KC_3,    KC_4,    KC_5,                         KC_6,     KC_7,     KC_8,    KC_9,    KC_0,     KC_MINUS,
    KC_GRAVE, KC_Q,   KC_W,    KC_E,    KC_R,    KC_T,                         KC_Y,     KC_U,     KC_I,    KC_O,    KC_SCLN,  KC_BSPC,
    KC_TAB,   KC_A,   KC_S,    KC_D,    KC_F,    KC_G,                         KC_H,     KC_J,     KC_K,    KC_L,    KC_P,     KC_QUOT,
    KC_LSFT,  KC_Z,   KC_X,    KC_C,    KC_V,    KC_B,    KC_CAPS,   KC_MUTE,  KC_N,     KC_M,     KC_COMM, KC_DOT,  KC_SLSH,  KC_BSLS,
                      KC_LCTL, KC_LALT, KC_LGUI, MO(SYM), KC_RSFT,   KC_SPC,   KC_ENTER, MO(EXT),    KC_DOWN, KC_UP
),

/* 
 * GAME -- gaming-compatible layer
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * | Esc  |   1! |   2@ |   3# |   4$ |   5% |                    |   6^ |   7& |   8* |  9(  |  0)  |  -_  |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * | `~   |   Q  |   W  |   E  |   R  |   T  |                    |   Y  |   U  |   I  |   O  |  ;:  | Bspc |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * | Tab  |   A  |   S  |   D  |   F  |   G  |-------.    ,-------|   H  |   J  |   K  |   L  |   P  |  '"  |
 * |------+------+------+------+------+------|  Caps |    | Mute  |------+------+------+------+------+------|
 * |LShift|   Z  |   X  |   C  |   V  |   B  |-------|    |-------|   N  |   M  |  ,<  |  .>  |  /?  |  \|  |
 * `-----------------------------------------/       /    \       \-----------------------------------------'
 *            | LCMD | LALT | LCTL | MO   | / Space /      \ Space \  | Enter| MO   | Down |  Up  |
 *            |      |      |      | SYM  |/       /        \       \ |      | EXT  |      |      |
 *            '-----------------------------------'          '-------''---------------------------'
 */
 [GAME] = LAYOUT_split_4x6_5(
    KC_ESC,   KC_1,   KC_2,    KC_3,    KC_4,    KC_5,                       KC_6,     KC_7,    KC_8,    KC_9,    KC_0,     KC_MINUS,
    KC_GRAVE, KC_Q,   KC_W,    KC_E,    KC_R,    KC_T,                       KC_Y,     KC_U,    KC_I,    KC_O,    KC_SCLN,  KC_BSPC,
    KC_TAB,   KC_A,   KC_S,    KC_D,    KC_F,    KC_G,                       KC_H,     KC_J,    KC_K,    KC_L,    KC_P,     KC_QUOT,
    KC_LSFT,  KC_Z,   KC_X,    KC_C,    KC_V,    KC_B,   KC_CAPS,  KC_MUTE,  KC_N,     KC_M,    KC_COMM, KC_DOT,  KC_SLSH,  KC_BSLS,
                      KC_LGUI, KC_LALT, KC_LCTL, KC_SPC, MO(SYM),  KC_SPC,   KC_ENTER, TO(GAME), KC_DOWN, KC_UP
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
 * `-----------------------------------------/       /    \       \-----------------------------------------'
 *            |      |      |      |      | /       /      \       \  |      | TO   | Left | Right|
 *            |      |      |      |      |/       /        \       \ |      | GAME |      |      |
 *            '-----------------------------------'          '-------''---------------------------'
 */
[SYM] = LAYOUT_split_4x6_5(
    _______,       KC_F1,      KC_F2,         KC_F3,      KC_F4,      KC_F5,                           KC_F6,         KC_F7,      KC_F8,      KC_F9,         KC_F10,        KC_EQL,
    _______,       LSFT(KC_1), LSFT(KC_2),    LSFT(KC_3), LSFT(KC_4), LSFT(KC_5),                      LSFT(KC_6),    LSFT(KC_7), LSFT(KC_8), KC_F11,        KC_F12,        KC_DEL,
    LSFT(KC_MINS), KC_MINS,    LSFT(KC_BSLS), KC_LBRC,    LSFT(KC_9), LSFT(KC_LBRC),                   LSFT(KC_RBRC), LSFT(KC_0), KC_RBRC,    KC_EQL,        LSFT(KC_EQL),  _______,
    _______,       _______,    _______,       _______,    _______,    _______,     _______, _______,   _______,       _______,    KC_SCLN,    LSFT(KC_SCLN), LSFT(KC_SLSH), LSFT(KC_BSLS),
                               _______,       _______,    _______,    _______,     _______, _______,   _______,       TO(GAME),    KC_LEFT,    KC_RGHT
),

/*
 * Extension -- arrow keys and numpad numbers
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |      | Solid| RGB  |      |      |      |                    |      |      |      |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |      | Up   |      |      |      |                    |      |      |      |  7   |  8   |  9   |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      | Left | Down | Right|      |      |-------.    ,-------|      |      |      |  4   |  5   |  6   |
 * |------+------+------+------+------+------|       |    |       |------+------+------+------+------+------|
 * |      |      |      |      |      |      |-------|    |-------|      |      |      |  1   |  2   |  3   |
 * `-----------------------------------------/       /    \       \-----------------------------------------'
 *            |      |      |      | GAME | /       /      \       \  |      |      |      |  0   |
 *            |      |      |      |      |/       /        \       \ |      |      |      |      |
 *            '-----------------------------------'          '-------''---------------------------'
 */
 [EXT] = LAYOUT_split_4x6_5(
    _______,PB_1,   PB_2,   _______,_______,_______,                        _______,_______,_______,_______,_______,_______,
    _______,_______,_______,KC_UP,  _______,_______,                        _______,_______,_______,KC_KP_7,KC_KP_8,KC_KP_9,
    _______,_______,KC_LEFT,KC_DOWN,KC_RGHT,_______,                        _______,_______,_______,KC_KP_4,KC_KP_5,KC_KP_6,
    _______,_______,_______,_______,_______,_______,   PB_3,     _______,   _______,_______,_______,KC_KP_1,KC_KP_2,KC_KP_3,
                    _______,_______,_______,TO(GAME),  _______,  _______,   _______,_______,_______,KC_KP_0
),

};

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [BASE] = { ENCODER_CCW_CW(KC_WH_D, KC_WH_U), ENCODER_CCW_CW(KC_VOLD, KC_VOLU) },
    [GAME] = { ENCODER_CCW_CW(_______, _______), ENCODER_CCW_CW(_______, _______) },
    [SYM] = { ENCODER_CCW_CW(_______, _______), ENCODER_CCW_CW(_______, _______) },
    [EXT] = { ENCODER_CCW_CW(RM_VALD, RM_VALU), ENCODER_CCW_CW(_______, _______) },
};
#endif


