#include QMK_KEYBOARD_H

// set microcontroller led off
void keyboard_pre_init_user(void) {
    // Set our LED pin as output
    setPinOutput(24);
    // Turn the LED off
    // (Due to technical reasons, high is off and low is on)
    writePinHigh(24);
}

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

// TODO: can this be implemented with ACTION_TAP_DANCE_FN_ADVANCED_* see https://docs.qmk.fm/features/tap_dance#implementation
#define ACTION_TAP_DANCE_TAP_HOLD(tap, hold) \
    { .fn = {NULL, tap_dance_tap_hold_finished, tap_dance_tap_hold_reset}, .user_data = (void *)&((tap_dance_tap_hold_t){tap, hold, 0}), }

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
        case TD(DOT_SCR): // list all tap dance keycodes with tap-hold configurations
            /* when one of these keys is released after the OSL tap and release
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

void oneshot_mods_changed_user(uint8_t mods) {
    if (mods & MOD_MASK_ALT) {
        layer_clear(); // goes back to the base layer after OSM(*ALT)
    }
}

layer_state_t layer_state_set_user(layer_state_t state) {
    uint8_t current_layer = get_highest_layer(state);
    if (current_layer > 0) {
        setPinOutput(24);
        writePinLow(24);
    } else {
        setPinOutput(24);
        writePinHigh(24);
    }
    return state;
}

const key_override_t grv_tilde  = ko_make_basic(MOD_MASK_GUI, KC_GRV, S(KC_GRV));
const key_override_t eur_dollar = ko_make_basic(MOD_MASK_GUI, S(A(KC_2)), S(KC_4));

const key_override_t *key_overrides[] = {&grv_tilde, &eur_dollar};

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
    //                         │L󰘶/ 󱁐  │      │   │  L󰘴   │ OSL(1)│
    //                         └───────┴───────┘   └───────┴───────┘
    //
    [0] = LAYOUT_split_3x5_2(
            TD(SCLN_MINS), TD(COMM_UNDS), TD(DOT_EXCL), KC_P, KC_Y, KC_F, KC_G, KC_C, KC_R, KC_L,
            KC_A, KC_O, KC_E, KC_U, KC_I, KC_D, KC_H, KC_T, KC_N, KC_S,
            TD(QUOT_GRV), KC_Q, KC_J, KC_K, KC_X, KC_B, KC_M, KC_W, KC_V, KC_Z,
            LSFT_T(KC_SPC), OSM(MOD_LGUI), OSM(MOD_LCTL), OSL(1)),
    // Layer 1
    // ┌───────┬───────┬───────┬───────┬───────┐   ┌───────┬───────┬───────┬───────┬───────┐
    // │  ESC  │ TRNS  │ . scr │   @   │   =   │   │   -   │   %   │   |   │   *   │ BSPC  │
    // ├───────┼───────┼───────┼───────┼───────┤   ├───────┼───────┼───────┼───────┼───────┤
    // │  TAB  │   {   │   [   │   (   │ ^ 🏠  │   │ $ 🔚  │   )   │   ]   │   }   │ ENTER │
    // ├───────┼───────┼───────┼───────┼───────┤   ├───────┼───────┼───────┼───────┼───────┤
    // │ `  ~  │ TRNS  │   !   │   &   │   +   │   │   #   │   ?   │   /   │   \   │ TO(3) │
    // └───────┴───────┴───────┴───────┴───────┘   └───────┴───────┴───────┴───────┴───────┘
    //                         ┌───────┬───────┐   ┌───────┬───────┐
    //                         │ TO(0) │      │   │   ⌥   │ TO(2) │
    //                         └───────┴───────┘   └───────┴───────┘
    [1] = LAYOUT_split_3x5_2(
            KC_ESC, KC_COMM, TD(DOT_SCR), KC_AT, KC_EQL, KC_MINS, KC_PERC, KC_PIPE, KC_ASTR, KC_BSPC,
            KC_TAB, KC_LCBR, KC_LBRC, KC_LPRN, TD(CIRC_HOME), TD(DLR_END), KC_RPRN, KC_RBRC, KC_RCBR, KC_ENT,
            KC_GRV, KC_TRNS, KC_EXLM, KC_AMPR, KC_PLUS, KC_HASH, KC_QUES, KC_SLSH, KC_BSLS, TO(3),
            TO(0), OSM(MOD_LGUI), OSM(MOD_LALT), TO(2)),
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
    //                         │L󰘶 / 󱁐 │      │   │  R󰘴   │  BASE │
    //                         └───────┴───────┘   └───────┴───────┘
    [2] = LAYOUT_split_3x5_2(
            KC_ESC, KC_TRNS, KC_TRNS, FALL_BASE, KC_EQL, KC_MINS, KC_1, KC_2, KC_3, KC_BSPC,
            KC_TAB, FALL_BASE, FALL_BASE, FALL_BASE, S(A(KC_2)), FALL_BASE, KC_4, KC_5, KC_6, KC_ENT,
            FALL_BASE, FALL_BASE, FALL_BASE, FALL_BASE, KC_ASTR, KC_PSLS, KC_7, KC_8, KC_9, KC_0,
            SFT_T(KC_SPC), OSM(MOD_LGUI), OSM(MOD_RCTL), FALL_BASE),
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
    //                         │  SPC  │      │   │ RALT  │  BASE │
    //                         └───────┴───────┘   └───────┴───────┘
    [3] = LAYOUT_split_3x5_2(
            KC_ESC, KC_MRWD, KC_MFFD, KC_MPLY, KC_VOLU, LGUI(KC_LBRC), LGUI(KC_RBRC), RCS(KC_TAB), C(KC_TAB), KC_BSPC,
            KC_TAB, OSM(MOD_LCTL), OSM(MOD_LALT), OSM(MOD_LSFT), KC_VOLD, KC_LEFT, KC_DOWN, KC_UP, KC_RGHT, KC_ENT,
            TG(4), KC_Q, FALL_BASE, FALL_BASE, LGUI(KC_C), LGUI(KC_V), KC_PGDN, KC_PGUP, RSG(KC_Z), LGUI(KC_Z),
            KC_SPC, OSM(MOD_LGUI), OSM(MOD_RALT), FALL_BASE),
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
    [4] = LAYOUT_split_3x5_2(
            KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, QK_BOOT, KC_NO, KC_NO, KC_NO, KC_BSPC,
            KC_TRNS, KC_MS_WH_DOWN, KC_BTN2, KC_BTN1, KC_MS_WH_UP, KC_MS_L, KC_MS_D, KC_MS_U, KC_MS_R, KC_ENT,
            TG(4), KC_TRNS, KC_TRNS, KC_TRNS, DB_TOGG, KC_NO, KC_NO, KC_NO, KC_NO, KC_TRNS,
            KC_TRNS, KC_TRNS, KC_TRNS, FALL_BASE)
};

#if defined(ENCODER_ENABLE) && defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {

};
#endif // defined(ENCODER_ENABLE) && defined(ENCODER_MAP_ENABLE)
