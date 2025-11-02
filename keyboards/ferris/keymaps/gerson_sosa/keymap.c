#include QMK_KEYBOARD_H

enum layers {
    _BASE,
    _SYMBOLS,
    _NUMBERS,
    _ARROWS,
    _CONFIG,
};

// Tap Dance declarations
enum {
    SCLN_MINS,
    COMM_UNDS,
    DOT_EXCL,
    DOT_SCR,
    CIRC_HOME,
    DLR_END,
    QUOT_GRV,
};

// from any layer to base layer
enum kc_trns_to_base {
    FALL_BASE = SAFE_RANGE,
    CMD_BASE,
    ALT_BASE,
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
    [SCLN_MINS] = ACTION_TAP_DANCE_DOUBLE(KC_SCLN, KC_MINS),        //
    [COMM_UNDS] = ACTION_TAP_DANCE_DOUBLE(KC_COMM, KC_UNDS),        //
    [DOT_EXCL]  = ACTION_TAP_DANCE_DOUBLE(KC_DOT, KC_EXLM),         //
    [CIRC_HOME] = ACTION_TAP_DANCE_DOUBLE(S(KC_6), KC_HOME),        //
    [DLR_END]   = ACTION_TAP_DANCE_DOUBLE(S(KC_4), KC_END),         //
    [QUOT_GRV]  = ACTION_TAP_DANCE_DOUBLE(KC_QUOT, KC_GRV),         //
    [DOT_SCR]   = ACTION_TAP_DANCE_TAP_HOLD(KC_DOT, C(S(G(KC_4)))), //
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    tap_dance_action_t *action;

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
        case TD(DOT_SCR): // list all tap dance keycodes with tap-hold configurations
            /* NOTE: when one of these keys is released after the OSL/OSM tap and release
             * within **TAPPING_TERM** two chars are outputted for example `..`
             * this is not caused by permissive hold, the same happens without it.
             * my theory is that OSL makes the tap press and release trigger
             * this function twice */
            action = &tap_dance_actions[QK_TAP_DANCE_GET_INDEX(keycode)];
            if (!record->event.pressed && action->state.count && !action->state.finished) {
                tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)action->user_data;
                tap_code16(tap_hold->tap);
            }
    }
    return true;
}

void keyboard_pre_init_user(void) {
    // set microcontroller led off
    setPinOutput(LIATRIS_ON_LED_PIN);
    writePinHigh(LIATRIS_ON_LED_PIN);
}

void keyboard_post_init_user(void) {
    rgblight_enable_noeeprom();
    rgblight_sethsv_noeeprom(HSV_BLACK);
    rgblight_mode_noeeprom(RGBLIGHT_MODE_STATIC_LIGHT);
}

void housekeeping_task_user(void) {
    switch (get_highest_layer(layer_state | default_layer_state)) {
        case 0:
            rgblight_setrgb_at(RGB_BLACK, 0);
            rgblight_setrgb_at(RGB_BLACK, 1);
            break;
        case 1:
            rgblight_setrgb_at(RGB_BLUE, 0);
            rgblight_setrgb_at(RGB_BLUE, 1);
            break;
        case 2:
            rgblight_setrgb_at(RGB_BLACK, 0);
            rgblight_setrgb_at(RGB_GREEN, 1);
            break;
        case 3:
            rgblight_setrgb_at(RGB_PURPLE, 0);
            rgblight_setrgb_at(RGB_PURPLE, 1);
            break;
        case 4:
            rgblight_setrgb_at(RGB_RED, 0);
            rgblight_setrgb_at(RGB_RED, 1);
            break;
    }
}

const key_override_t grv_tilde  = ko_make_basic(MOD_MASK_GUI, KC_GRV, S(KC_GRV));
const key_override_t eur_dollar = ko_make_basic(MOD_MASK_GUI, S(A(KC_2)), S(KC_4));
const key_override_t alt_bspc   = ko_make_basic(MOD_MASK_ALT, KC_L, A(KC_BSPC));

const key_override_t *key_overrides[] = {&grv_tilde, &eur_dollar, &alt_bspc};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    // Layer 0
    // ┌───────┬───────┬───────┬───────┬───────┐   ┌───────┬───────┬───────┬───────┬───────┐
    // │ ; -   │ , _   │ . !   │   P   │   Y   │   │   F   │   G   │   C   │   R   │   L   │
    // ├───────┼───────┼───────┼───────┼───────┤   ├───────┼───────┼───────┼───────┼───────┤
    // │   A   │   O   │   E   │   U   │   I   │   │   D   │   H   │   T   │   N   │   S   │
    // ├───────┼───────┼───────┼───────┼───────┤   ├───────┼───────┼───────┼───────┼───────┤
    // │ ' `   │   Q   │   J   │   K   │   X   │   │   B   │   M   │   W   │   V   │   Z   │
    // └───────┴───────┴───────┴───────┴───────┘   └───────┴───────┴───────┴───────┴───────┘
    //                         ┌───────┬───────┐   ┌───────┬───────┐
    //                         │      │ L󰘶/ 󱁐 │   │  L󰘴   │ OSL(1)│
    //                         └───────┴───────┘   └───────┴───────┘
    //
    [_BASE] = LAYOUT_split_3x5_2(TD(SCLN_MINS), TD(COMM_UNDS), TD(DOT_EXCL), KC_P, KC_Y, KC_F, KC_G, KC_C, KC_R, KC_L, KC_A, KC_O, KC_E, KC_U, KC_I, KC_D, KC_H, KC_T, KC_N, KC_S, TD(QUOT_GRV), KC_Q, KC_J, KC_K, KC_X, KC_B, KC_M, KC_W, KC_V, KC_Z, KC_LGUI, SFT_T(KC_SPC), OSM(MOD_LCTL), OSL(1)),
    // Layer 1
    // ┌───────┬───────┬───────┬───────┬───────┐   ┌───────┬───────┬───────┬───────┬───────┐
    // │  ESC  │ TRNS  │ . scr │   @   │   =   │   │   -   │   %   │   |   │   *   │ BSPC  │
    // ├───────┼───────┼───────┼───────┼───────┤   ├───────┼───────┼───────┼───────┼───────┤
    // │  TAB  │   {   │   [   │   (   │ ^ 🏠  │   │ $ 🔚  │   )   │   ]   │   }   │ ENTER │
    // ├───────┼───────┼───────┼───────┼───────┤   ├───────┼───────┼───────┼───────┼───────┤
    // │ `  ~  │ TRNS  │   !   │   &   │   +   │   │   #   │   ?   │   /   │   \   │ TO(3) │
    // └───────┴───────┴───────┴───────┴───────┘   └───────┴───────┴───────┴───────┴───────┘
    //                         ┌───────┬───────┐   ┌───────┬───────┐
    //                         │  0/  │ BASE  │   │   ⌥   │ TO(2) │
    //                         └───────┴───────┘   └───────┴───────┘
    [_SYMBOLS] = LAYOUT_split_3x5_2(KC_ESC, KC_COMM, TD(DOT_SCR), KC_AT, KC_EQL, KC_MINS, KC_PERC, KC_PIPE, KC_ASTR, KC_BSPC, KC_TAB, KC_LCBR, KC_LBRC, KC_LPRN, TD(CIRC_HOME), TD(DLR_END), KC_RPRN, KC_RBRC, KC_RCBR, KC_ENT, KC_GRV, KC_TRNS, KC_EXLM, KC_AMPR, KC_PLUS, KC_HASH, KC_QUES, KC_SLSH, KC_BSLS, TO(3), FALL_BASE, SFT_T(KC_SPC), OSM(MOD_LALT), TO(2)),
    //
    // Layer 2
    // ┌───────┬───────┬───────┬───────┬───────┐   ┌───────┬───────┬───────┬───────┬───────┐
    // │  ESC  │ TRNS  │ TRNS  │  BASE │   =   │   │   -   │   1   │   2   │   3   │ BSPC  │
    // ├───────┼───────┼───────┼───────┼───────┤   ├───────┼───────┼───────┼───────┼───────┤
    // │  TAB  │  BASE │  BASE │  BASE │  €/$  │   │  BASE │   4   │   5   │   6   │ ENTER │
    // ├───────┼───────┼───────┼───────┼───────┤   ├───────┼───────┼───────┼───────┼───────┤
    // │  BASE │  BASE │  BASE │  BASE │   *   │   │   /   │   7   │   8   │   9   │   0   │
    // └───────┴───────┴───────┴───────┴───────┘   └───────┴───────┴───────┴───────┴───────┘
    //                         ┌───────┬───────┐   ┌───────┬───────┐
    //                         │  0/  │ L󰘶/ 󱁐 │   │  R󰘴   │  BASE │
    //                         └───────┴───────┘   └───────┴───────┘
    [_NUMBERS] = LAYOUT_split_3x5_2(KC_ESC, KC_TRNS, KC_TRNS, FALL_BASE, KC_EQL, KC_MINS, KC_1, KC_2, KC_3, KC_BSPC, KC_TAB, FALL_BASE, FALL_BASE, FALL_BASE, S(A(KC_2)), FALL_BASE, KC_4, KC_5, KC_6, KC_ENT, FALL_BASE, FALL_BASE, FALL_BASE, FALL_BASE, KC_ASTR, KC_PSLS, KC_7, KC_8, KC_9, KC_0, ALT_BASE, SFT_T(KC_SPC), KC_RCTL, FALL_BASE),
    //
    // Layer 3
    // ┌───────┬───────┬───────┬───────┬───────┐   ┌───────┬───────┬───────┬───────┬───────┐
    // │  ESC  │ MRWD  │ MFFD  │ MPLY  │ VOLU  │   │ G([)  │ G(])  │C(TB)  │RC(TAB)│ BSPC  │
    // ├───────┼───────┼───────┼───────┼───────┤   ├───────┼───────┼───────┼───────┼───────┤
    // │  TAB  │ LCTRL │ LALT  │ LSHFT │ VOLD  │   │ LEFT  │ DOWN  │  UP   │ RIGHT │ ENTER │
    // ├───────┼───────┼───────┼───────┼───────┤   ├───────┼───────┼───────┼───────┼───────┤
    // │ TG(4) │ KC_Q  │  BASE │  BASE │ G(C)  │   │ G(V)  │ PGDN  │ PGUP  │RSG(Z) │ G(Z)  │
    // └───────┴───────┴───────┴───────┴───────┘   └───────┴───────┴───────┴───────┴───────┘
    //                         ┌───────┬───────┐   ┌───────┬───────┐
    //                         │  0/  │ L󰘶/ 󱁐 │   │ RALT  │  BASE │
    //                         └───────┴───────┘   └───────┴───────┘
    [_ARROWS] = LAYOUT_split_3x5_2(KC_ESC, KC_MRWD, KC_MFFD, KC_MPLY, KC_VOLU, LGUI(KC_LBRC), LGUI(KC_RBRC), RCS(KC_TAB), C(KC_TAB), KC_BSPC, KC_TAB, OSM(MOD_LCTL), OSM(MOD_LALT), OSM(MOD_LSFT), KC_VOLD, KC_LEFT, KC_DOWN, KC_UP, KC_RGHT, KC_ENT, TG(4), KC_Q, FALL_BASE, FALL_BASE, LGUI(KC_C), LGUI(KC_V), KC_PGDN, KC_PGUP, RSG(KC_Z), LGUI(KC_Z), CMD_BASE, SFT_T(KC_SPC), KC_RALT, FALL_BASE),
    //
    // Layer 4
    // ┌───────┬───────┬───────┬───────┬───────┐   ┌───────┬───────┬───────┬───────┬───────┐
    // │ TRNS  │ TRNS  │ TRNS  │ TRNS  │ TRNS  │   │ BOOTL │  NO   │  NO   │  NO   │ BSPC  │
    // ├───────┼───────┼───────┼───────┼───────┤   ├───────┼───────┼───────┼───────┼───────┤
    // │ TRNS  │MS_WH_D│ BTN2  │ BTN1  │MS_WH_U│   │ MS_L  │ MS_D  │ MS_U  │ MS_R  │ ENTER │
    // ├───────┼───────┼───────┼───────┼───────┤   ├───────┼───────┼───────┼───────┼───────┤
    // │ TG(4) │  NO   │  NO   │  NO   │DB_TOGG│   │  NO   │  NO   │  NO   │  NO   │ TRNS  │
    // └───────┴───────┴───────┴───────┴───────┘   └───────┴───────┴───────┴───────┴───────┘
    //                         ┌───────┬───────┐   ┌───────┬───────┐
    //                         │ TRNS  │ TRNS  │   │ TRNS  │ TRNS  │
    //                         └───────┴───────┘   └───────┴───────┘
    //
    [_CONFIG] = LAYOUT_split_3x5_2(KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, QK_BOOT, KC_NO, KC_NO, KC_NO, KC_BSPC, KC_TRNS, MS_WHLD, MS_BTN2, MS_BTN1, MS_WHLU, MS_LEFT, MS_DOWN, MS_UP, MS_RGHT, KC_ENT, TG(4), KC_TRNS, KC_TRNS, KC_TRNS, DB_TOGG, KC_NO, KC_NO, KC_NO, KC_NO, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, FALL_BASE)};

#if defined(ENCODER_ENABLE) && defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {

};
#endif // defined(ENCODER_ENABLE) && defined(ENCODER_MAP_ENABLE)
