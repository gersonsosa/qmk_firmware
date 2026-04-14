#include QMK_KEYBOARD_H

enum layers {
    _BASE,
    _SYMBOLS,
    _NUMBERS,
    _ARROWS,
    _CONFIG,
    _BASE_CO,
};

// Tap Dance declarations
enum {
    CIRC_HOME,
    DLR_END,
};

// from any layer to base layer
enum kc_trns_to_base {
    FALL_BASE = SAFE_RANGE,
    CMD_BASE,
    ALT_BASE,
    ALT_FALL_BASE,
};

typedef struct {
    uint16_t tap;
    uint16_t hold;
    uint16_t held;
} tap_dance_tap_hold_t;

void tap_dance_tap_hold_finished(tap_dance_state_t *state, void *user_data) {
    tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)user_data;

    if (state->pressed) {
        if (state->count == 1
#ifndef PERMISSIVE_HOLD
            && !state->interrupted
#endif
        ) {
            register_code16(tap_hold->hold);
            tap_hold->held = tap_hold->hold;
        } else {
            register_code16(tap_hold->tap);
            tap_hold->held = tap_hold->tap;
        }
    }
}

void tap_dance_tap_hold_reset(tap_dance_state_t *state, void *user_data) {
    tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)user_data;

    if (tap_hold->held) {
        unregister_code16(tap_hold->held);
        tap_hold->held = 0;
    }
}

#define ACTION_TAP_DANCE_TAP_HOLD(tap, hold)                                        \
    {                                                                               \
        .fn        = {NULL, tap_dance_tap_hold_finished, tap_dance_tap_hold_reset}, \
        .user_data = (void *)&((tap_dance_tap_hold_t){tap, hold, 0}),               \
    }

// Tap Dance definitions
tap_dance_action_t tap_dance_actions[] = {
    [CIRC_HOME] = ACTION_TAP_DANCE_DOUBLE(S(KC_6), KC_HOME), //
    [DLR_END]   = ACTION_TAP_DANCE_DOUBLE(S(KC_4), KC_END),  //
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case FALL_BASE:
            if (record->event.pressed) {
                layer_clear();
            }
            break;
        case CMD_BASE:
            if (record->event.pressed) {
                register_mods(MOD_BIT(KC_LGUI));
            } else {
                unregister_mods(MOD_BIT(KC_LGUI));
                layer_clear();
            }
            return false;
        case ALT_BASE:
            if (record->event.pressed) {
                register_mods(MOD_BIT(KC_LALT));
            } else {
                unregister_mods(MOD_BIT(KC_LALT));
                layer_clear();
            }
            return false;
        case ALT_FALL_BASE:
            if (record->event.pressed) {
                register_mods(MOD_BIT(KC_LALT));
                layer_clear();
            } else {
                unregister_mods(MOD_BIT(KC_LALT));
            }
            return false;
            // TODO: add a default case to return to the base layer from the first unless the keypress is backspace
    }
    return true;
}

void keyboard_pre_init_user(void) {
    // set microcontroller led off
    gpio_set_pin_output(LIATRIS_ON_LED_PIN);
    gpio_write_pin_high(LIATRIS_ON_LED_PIN);
}

void keyboard_post_init_user(void) {
    rgblight_enable_noeeprom();
    rgblight_sethsv_noeeprom(HSV_BLACK);
    rgblight_mode_noeeprom(RGBLIGHT_MODE_STATIC_LIGHT);
}

void housekeeping_task_user(void) {
    switch (get_highest_layer(layer_state | default_layer_state)) {
        case _BASE:
            rgblight_setrgb_at(RGB_BLACK, 0);
            rgblight_setrgb_at(RGB_BLACK, 1);
            break;
        case _SYMBOLS:
            rgblight_setrgb_at(RGB_BLUE, 0);
            rgblight_setrgb_at(RGB_BLUE, 1);
            break;
        case _NUMBERS:
            rgblight_setrgb_at(RGB_GREEN, 0);
            rgblight_setrgb_at(RGB_BLACK, 1);
            break;
        case _ARROWS:
            rgblight_setrgb_at(RGB_PURPLE, 0);
            rgblight_setrgb_at(RGB_PURPLE, 1);
            break;
        case _CONFIG:
            rgblight_setrgb_at(RGB_RED, 0);
            rgblight_setrgb_at(RGB_RED, 1);
            break;
        case _BASE_CO:
            rgblight_setrgb_at(RGB_PINK, 0);
            rgblight_setrgb_at(RGB_PINK, 1);
            break;
    }
}

const key_override_t grv_tilde  = ko_make_basic(MOD_MASK_GUI, KC_GRV, S(KC_GRV));
const key_override_t eur_dollar = ko_make_basic(MOD_MASK_GUI, S(A(KC_2)), S(KC_4));
const key_override_t alt_bspc   = ko_make_basic(MOD_MASK_ALT, KC_L, A(KC_BSPC));

const key_override_t *key_overrides[] = {&grv_tilde, &eur_dollar, &alt_bspc};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    // QWERTY
    // ┌───────┬───────┬───────┬───────┬───────┐   ┌───────┬───────┬───────┬───────┬───────┐
    // │   Q   │   W   │   E   │   R   │   T   │   │   Y   │   U   │   I   │   O   │   P   │
    // ├───────┼───────┼───────┼───────┼───────┤   ├───────┼───────┼───────┼───────┼───────┤
    // │   A   │   S   │   D   │   F   │   G   │   │   H   │   J   │   K   │   L   │   ;   │
    // ├───────┼───────┼───────┼───────┼───────┤   ├───────┼───────┼───────┼───────┼───────┤
    // │   Z   │   X   │   C   │   V   │   B   │   │   N   │   M   │   ,   │   .   │   /   │
    // └───────┴───────┴───────┴───────┴───────┘   └───────┴───────┴───────┴───────┴───────┘
    //                         ┌───────┬───────┐   ┌───────┬───────┐
    //                         │      │ L󰘶/ 󱁐 │   │  L󰘴   │Symbols│
    //                         └───────┴───────┘   └───────┴───────┘
    //
    [_BASE] = LAYOUT_split_3x5_2(KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_LGUI, SFT_T(KC_SPC), OSM(MOD_LCTL), OSL(_SYMBOLS)),
    // ┌───────┬───────┬───────┬───────┬───────┐   ┌───────┬───────┬───────┬───────┬───────┐
    // │   Q   │   W   │   F   │   P   │   B   │   │   J   │   L   │   U   │   Y   │   ;   │
    // ├───────┼───────┼───────┼───────┼───────┤   ├───────┼───────┼───────┼───────┼───────┤
    // │   A   │   R   │   S   │   T   │   G   │   │   M   │   N   │   E   │   I   │   O   │
    // ├───────┼───────┼───────┼───────┼───────┤   ├───────┼───────┼───────┼───────┼───────┤
    // │   Z   │   X   │   C   │   D   │   V   │   │   K   │   H   │   ,   │   .   │   /   │
    // └───────┴───────┴───────┴───────┴───────┘   └───────┴───────┴───────┴───────┴───────┘
    //                         ┌───────┬───────┐   ┌───────┬───────┐
    //                         │      │ L󰘶/ 󱁐 │   │  L󰘴   │ OSL(1)│
    //                         └───────┴───────┘   └───────┴───────┘
    //
    [_BASE_CO] = LAYOUT_split_3x5_2(KC_Q, KC_W, KC_F, KC_P, KC_B, KC_J, KC_L, KC_U, KC_Y, KC_SCLN, KC_A, KC_R, KC_S, KC_T, KC_G, KC_M, KC_N, KC_E, KC_I, KC_O, KC_Z, KC_X, KC_C, KC_D, KC_V, KC_K, KC_H, KC_COMM, KC_DOT, KC_SLSH, KC_LGUI, SFT_T(KC_SPC), OSM(MOD_LCTL), OSL(_SYMBOLS)),
    // ┌───────┬───────┬───────┬───────┬───────┐   ┌───────┬───────┬───────┬───────┬───────┐
    // │  ESC  │   !   │   @   │   #   │   =   │   │   -   │   %   │   |   │   \   │ BSPC  │
    // ├───────┼───────┼───────┼───────┼───────┤   ├───────┼───────┼───────┼───────┼───────┤
    // │  TAB  │   {   │   [   │   (   │ ^ 🏠  │   │ $ 🔚  │   )   │   ]   │   }   │ ENTER │
    // ├───────┼───────┼───────┼───────┼───────┤   ├───────┼───────┼───────┼───────┼───────┤
    // │ `  ~  │   '   │   "   │   &   │   *   │   │       │       │       │       │ TO(3) │
    // └───────┴───────┴───────┴───────┴───────┘   └───────┴───────┴───────┴───────┴───────┘
    //                         ┌───────┬───────┐   ┌───────┬───────┐
    //                         │      │ L󰘶/ 󱁐 │   │  ⌥->B │Numbers│
    //                         └───────┴───────┘   └───────┴───────┘
    [_SYMBOLS] = LAYOUT_split_3x5_2(KC_ESC, KC_EXLM, KC_AT, KC_HASH, KC_EQL, KC_MINS, KC_PERC, KC_PIPE, KC_BSLS, KC_BSPC, KC_TAB, KC_LCBR, KC_LBRC, KC_LPRN, TD(CIRC_HOME), TD(DLR_END), KC_RPRN, KC_RBRC, KC_RCBR, KC_ENT, KC_GRV, KC_QUOT, KC_DQT, KC_AMPR, KC_ASTR, KC_NO, KC_NO, KC_NO, KC_NO, TO(_ARROWS), CMD_BASE, KC_LSFT, ALT_FALL_BASE, TO(_NUMBERS)),
    //
    // ┌───────┬───────┬───────┬───────┬───────┐   ┌───────┬───────┬───────┬───────┬───────┐
    // │  ESC  │   7   │   8   │   9   │   =   │   │   -   │  BASE │  BASE │  BASE │ BSPC  │
    // ├───────┼───────┼───────┼───────┼───────┤   ├───────┼───────┼───────┼───────┼───────┤
    // │  TAB  │   4   │   5   │   6   │  €/$  │   │  BASE │  BASE │  BASE │  BASE │ ENTER │
    // ├───────┼───────┼───────┼───────┼───────┤   ├───────┼───────┼───────┼───────┼───────┤
    // │   0   │   1   │   2   │   3   │   *   │   │   /   │  BASE │   ,   │   .   │  L^   │
    // └───────┴───────┴───────┴───────┴───────┘   └───────┴───────┴───────┴───────┴───────┘
    //                         ┌───────┬───────┐   ┌───────┬───────┐
    //                         │  0/  │ L󰘶/ 󱁐 │   │   ⌥   │  BASE │
    //                         └───────┴───────┘   └───────┴───────┘
    [_NUMBERS] = LAYOUT_split_3x5_2(KC_ESC, KC_7, KC_8, KC_9, KC_EQL, KC_MINS, KC_TRNS, KC_TRNS, KC_TRNS, KC_BSPC, KC_TAB, KC_4, KC_5, KC_6, KC_0, FALL_BASE, FALL_BASE, FALL_BASE, FALL_BASE, KC_ENT, KC_0, KC_1, KC_2, KC_3, KC_ASTR, KC_PSLS, KC_TRNS, KC_COMM, KC_DOT, OSM(MOD_LCTL), CMD_BASE, SFT_T(KC_SPC), ALT_BASE, FALL_BASE),
    //
    // ┌───────┬───────┬───────┬───────┬───────┐   ┌───────┬───────┬───────┬───────┬───────┐
    // │  ESC  │ MRWD  │ MFFD  │ MPLY  │ VOLU  │   │ G([)  │ G(])  │C(TB)  │RC(TAB)│ BSPC  │
    // ├───────┼───────┼───────┼───────┼───────┤   ├───────┼───────┼───────┼───────┼───────┤
    // │  TAB  │ LCTRL │ LALT  │ LSHFT │ VOLD  │   │ LEFT  │ DOWN  │  UP   │ RIGHT │ ENTER │
    // ├───────┼───────┼───────┼───────┼───────┤   ├───────┼───────┼───────┼───────┼───────┤
    // │ TG(4) │ BASE  │  BASE │  BASE │ G(C)  │   │ G(V)  │ PGDN  │ PGUP  │RSG(Z) │ G(Z)  │
    // └───────┴───────┴───────┴───────┴───────┘   └───────┴───────┴───────┴───────┴───────┘
    //                         ┌───────┬───────┐   ┌───────┬───────┐
    //                         │  0/  │ L󰘶/ 󱁐 │   │ RALT  │  BASE │
    //                         └───────┴───────┘   └───────┴───────┘
    [_ARROWS] = LAYOUT_split_3x5_2(KC_ESC, KC_MRWD, KC_MFFD, KC_MPLY, KC_VOLU, LGUI(KC_LBRC), LGUI(KC_RBRC), RCS(KC_TAB), C(KC_TAB), KC_BSPC, KC_TAB, OSM(MOD_LCTL), OSM(MOD_LALT), OSM(MOD_LSFT), KC_VOLD, KC_LEFT, KC_DOWN, KC_UP, KC_RGHT, KC_ENT, TG(_CONFIG), FALL_BASE, FALL_BASE, FALL_BASE, LGUI(KC_C), LGUI(KC_V), KC_PGDN, KC_PGUP, RSG(KC_Z), LGUI(KC_Z), CMD_BASE, SFT_T(KC_SPC), KC_RALT, FALL_BASE),
    // ┌───────┬───────┬───────┬───────┬───────┐   ┌───────┬───────┬───────┬───────┬───────┐
    // │ TRNS  │ TRNS  │ TRNS  │ TRNS  │ TRNS  │   │ BOOTL │  NO   │  NO   │  NO   │ BSPC  │
    // ├───────┼───────┼───────┼───────┼───────┤   ├───────┼───────┼───────┼───────┼───────┤
    // │ TRNS  │MS_WH_D│ BTN2  │ BTN1  │MS_WH_U│   │ MS_L  │ MS_D  │ MS_U  │ MS_R  │ ENTER │
    // ├───────┼───────┼───────┼───────┼───────┤   ├───────┼───────┼───────┼───────┼───────┤
    // │ TG(4) │  NO   │  NO   │  NO   │DB_TOGG│   │  NO   │  NO   │  NO   │  NO   │ TRNS  │
    // └───────┴───────┴───────┴───────┴───────┘   └───────┴───────┴───────┴───────┴───────┘
    //                         ┌───────┬───────┐   ┌───────┬───────┐
    //                         │ TRNS  │ TO(5) │   │FALL_B │FALL_B │
    //                         └───────┴───────┘   └───────┴───────┘
    //
    [_CONFIG] = LAYOUT_split_3x5_2(KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, QK_BOOT, KC_NO, KC_NO, KC_NO, KC_BSPC, KC_TRNS, MS_WHLD, MS_BTN2, MS_BTN1, MS_WHLU, MS_LEFT, MS_DOWN, MS_UP, MS_RGHT, KC_ENT, TG(4), KC_TRNS, KC_TRNS, KC_TRNS, DB_TOGG, KC_NO, KC_NO, KC_NO, KC_NO, KC_TRNS, KC_TRNS, DF(_BASE_CO), FALL_BASE, FALL_BASE)};

#if defined(ENCODER_ENABLE) && defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {

};
#endif // defined(ENCODER_ENABLE) && defined(ENCODER_MAP_ENABLE)
