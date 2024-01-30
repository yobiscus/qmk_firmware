// Copyright 2022 beekeeb
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

enum {
    CT_LSFT,
    CT_RSFT,
    CT_GUI,
};

typedef struct {
    uint16_t tap;
    uint16_t hold;
    uint16_t held;
} tap_dance_tap_hold_t;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    tap_dance_action_t *action;

    switch (keycode) {
        case TD(CT_LSFT):
        case TD(CT_RSFT):
        case TD(CT_GUI):
            action = &tap_dance_actions[TD_INDEX(keycode)];
            if (!record->event.pressed && action->state.count && !action->state.finished) {
                tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)action->user_data;
                tap_code16(tap_hold->tap);
            }
    }
    return true;
}

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

#define ACTION_TAP_DANCE_TAP_HOLD(tap, hold) \
    { .fn = {NULL, tap_dance_tap_hold_finished, tap_dance_tap_hold_reset}, .user_data = (void *)&((tap_dance_tap_hold_t){tap, hold, 0}), }

tap_dance_action_t tap_dance_actions[] = {
    [CT_LSFT] = ACTION_TAP_DANCE_TAP_HOLD(KC_ENT, KC_LSFT),
    [CT_RSFT] = ACTION_TAP_DANCE_TAP_HOLD(KC_SPC, KC_RSFT),
    // CT_GUI doesn't work because of DF(1)
    // [CT_GUI] = ACTION_TAP_DANCE_TAP_HOLD(DF(1), KC_LGUI),
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
     /* Layer 0 - Typing
      * ┌───┬───┬───┬───┬───┬───┐       ┌───┬───┬───┬───┬───┬───┐
      * │Tab│ Q │ W │ F │ P │ B │       │ J │ L │ U │ Y │ ; │GUI│
      * ├───┼───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┼───┤
      * │Bsp│ A │ R │ S │ T │ G │       │ M │ N │ E │ I │ O │ ' │
      * ├───┼───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┼───┤
      * │Alt│ Z │ X │ C │ D │ V │       │ K │ H │ , │ . │ / │DF1│
      * └───┴───┴───┴───┴───┴───┘       └───┴───┴───┴───┴───┴───┘
      *               ┌───┐                   ┌───┐
      *               │CTL├───┐           ┌───┤ L2│
      *               └───┤ENT├───┐   ┌───┤ L1├───┘
      *                   └───┤SFT│   │SPC├───┘
      *                       └───┘   └───┘
      */
    [0] = LAYOUT_split_3x6_3(
        KC_TAB,  KC_Q,    KC_W,    KC_F,    KC_P,    KC_B,                            KC_J,    KC_L,    KC_U,    KC_Y,    KC_SCLN, KC_LGUI,
        KC_BSPC, KC_A,    KC_R,    KC_S,    KC_T,    KC_G,                            KC_M,    KC_N,    KC_E,    KC_I,    KC_O,    KC_QUOT,
        KC_LALT, KC_Z,    KC_X,    KC_C,    KC_D,    KC_V,                            KC_K,    KC_H,    KC_COMM, KC_DOT,  KC_SLSH, DF(1),
                                            KC_LCTL, KC_ENT, KC_LSFT,        KC_SPC,  OSL(1),  OSL(2)
    ),
     /* Layer 1 - Numbers & Symbols
      * ┌───┬───┬───┬───┬───┬───┐       ┌───┬───┬───┬───┬───┬───┐
      * │ ▼ │ ! │ @ │ # │ $ │ % │       │ ^ │ & │ * │ ( │ ) │ ▼ │
      * ├───┼───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┼───┤
      * │ ▼ │ 1 │ 2 │ 3 │ 4 │ 5 │       │ - │ = │ ` │ [ │ ] │   │
      * ├───┼───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┼───┤
      * │ ▼ │ 6 │ 7 │ 8 │ 9 │ 0 │       │ _ │ + │ ~ │ { │ } │DF2│
      * └───┴───┴───┴───┴───┴───┘       └───┴───┴───┴───┴───┴───┘
      *               ┌───┐                   ┌───┐
      *               │ ▼ ├───┐           ┌───┤ ▼ │
      *               └───┤ ▼ ├───┐   ┌───┤ ▼ ├───┘
      *                   └───┤ ▼ │   │ ▼ ├───┘
      *                       └───┘   └───┘
      */
    [1] = LAYOUT_split_3x6_3(
        KC_TRNS, S(KC_1), S(KC_2), S(KC_3), S(KC_4), S(KC_5),                     S(KC_6),    S(KC_7),   S(KC_8),     S(KC_9),    S(KC_0),    KC_TRNS,
        KC_TRNS, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                        KC_MINS,    KC_EQL,    KC_GRAVE,    KC_LBRC,    KC_RBRC,    KC_NO,
        KC_TRNS, KC_6,    KC_7,    KC_8,    KC_9,    KC_0,                        S(KC_MINS), S(KC_EQL), S(KC_GRAVE), S(KC_LBRC), S(KC_RBRC), DF(2),
                                            KC_TRNS, KC_TRNS, KC_TRNS,   KC_TRNS, KC_TRNS,    KC_TRNS
    ),
     /* Layer 2 - Function
      * ┌───┬───┬───┬───┬───┬───┐       ┌───┬───┬───┬───┬───┬───┐
      * │Esc│F1 │F2 │F3 │F4 │F5 │       │Mut│Vl-│Vl+│ ▼ │Up │PgU│
      * ├───┼───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┼───┤
      * │ ▼ │F6 │F7 │F8 │F9 │F10│       │P/P│Prv│Nxt│PgD│Dwn│Rgt│
      * ├───┼───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┼───┤
      * │ ▼ │F11│F12│ ▼ │ ▼ │ ▼ │       │ ▼ │ ▼ │ ▼ │Lft│ ▼ │DF3│
      * └───┴───┴───┴───┴───┴───┘       └───┴───┴───┴───┴───┴───┘
      *               ┌───┐                   ┌───┐
      *               │ ▼ ├───┐           ┌───┤ ▼ │
      *               └───┤ ▼ ├───┐   ┌───┤ ▼ ├───┘
      *                   └───┤ ▼ │   │ ▼ ├───┘
      *                       └───┘   └───┘
      */
    [2] = LAYOUT_split_3x6_3(
        KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                              KC_MUTE, KC_VOLD, KC_VOLU, KC_TRNS, KC_UP,   KC_PGUP,
        KC_TRNS, KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F6,                              KC_MPLY, KC_MPRV, KC_MNXT, KC_PGDN, KC_DOWN, KC_RGHT,
        KC_TRNS, KC_F11,  KC_F12,  KC_TRNS, KC_TRNS, KC_TRNS,                            KC_TRNS, KC_TRNS, KC_TRNS, KC_LEFT, KC_TRNS, DF(3),
                                            KC_TRNS, KC_TRNS, KC_TRNS,          KC_TRNS, KC_TRNS, KC_TRNS
    ),
     /* Layer 3 - Gaming
      * ┌───┬───┬───┬───┬───┬───┐       ┌───┬───┬───┬───┬───┬───┐
      * │ESC│ ▼ │ Q │ W │ E │ R │       │ ▼ │ ▼ │ ▼ │ ▼ │ ▼ │ ▼ │
      * ├───┼───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┼───┤
      * │ ▼ │ ▼ │ A │ S │ D │ F │       │ ▼ │ ▼ │ ▼ │ ▼ │ ▼ │ ▼ │
      * ├───┼───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┼───┤
      * │ ▼ │ ▼ │ Z │ X │ C │ V │       │ ▼ │ ▼ │ ▼ │ ▼ │ ▼ │DF0│
      * └───┴───┴───┴───┴───┴───┘       └───┴───┴───┴───┴───┴───┘
      *               ┌───┐                   ┌───┐
      *               │ ▼ ├───┐           ┌───┤ ▼ │
      *               └───┤ ▼ ├───┐   ┌───┤ ▼ ├───┘
      *                   └───┤ ▼ │   │ ▼ ├───┘
      *                       └───┘   └───┘
      */
    [3] = LAYOUT_split_3x6_3(
        KC_ESC,  KC_TRNS, KC_Q,    KC_W,    KC_E,    KC_R,                               KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_A,    KC_S,    KC_D,    KC_F,                               KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_Z,    KC_X,    KC_C,    KC_V,                               KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, DF(0),
                                            KC_TRNS, KC_TRNS, KC_TRNS,          KC_TRNS, KC_TRNS, KC_TRNS
    ),
     /* Layer 4 - Unused
      * ┌───┬───┬───┬───┬───┬───┐       ┌───┬───┬───┬───┬───┬───┐
      * │ ▼ │ ▼ │ ▼ │ ▼ │ ▼ │ ▼ │       │ ▼ │ ▼ │ ▼ │ ▼ │ ▼ │ ▼ │
      * ├───┼───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┼───┤
      * │ ▼ │ ▼ │ ▼ │ ▼ │ ▼ │ ▼ │       │ ▼ │ ▼ │ ▼ │ ▼ │ ▼ │ ▼ │
      * ├───┼───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┼───┤
      * │ ▼ │ ▼ │ ▼ │ ▼ │ ▼ │ ▼ │       │ ▼ │ ▼ │ ▼ │ ▼ │ ▼ │ ▼ │
      * └───┴───┴───┴───┴───┴───┘       └───┴───┴───┴───┴───┴───┘
      *               ┌───┐                   ┌───┐
      *               │ ▼ ├───┐           ┌───┤ ▼ │
      *               └───┤ ▼ ├───┐   ┌───┤ ▼ ├───┘
      *                   └───┤ ▼ │   │ ▼ ├───┘
      *                       └───┘   └───┘
      */
    [4] = LAYOUT_split_3x6_3(
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                            KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                            KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                            KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                                            KC_TRNS, KC_TRNS, KC_TRNS,          KC_TRNS, KC_TRNS, KC_TRNS
    ),
};
