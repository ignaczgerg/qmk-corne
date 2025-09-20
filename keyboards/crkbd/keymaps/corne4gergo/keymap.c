#include QMK_KEYBOARD_H
#include "process_tap_dance.h"
// #include "process_unicode_common.h"

/* --- Unicode includes (portable across QMK versions) --- */
#if __has_include("process_unicode_common.h")
#  include "process_unicode_common.h"
#elif __has_include("process_unicode.h")
#  include "process_unicode.h"
#elif __has_include("quantum/process_keycode/process_unicode.h")
#  include "quantum/process_keycode/process_unicode.h"
#endif

/* Combo IDs */
enum combo_events {
    CB_RESET = 0,
};

/* The chord: left thumb Space, right thumb Enter, right pinky Backspace */
const uint16_t PROGMEM cb_reset_keys[] = { KC_SPC, KC_ENT, KC_BSPC, COMBO_END };

/* Combo table: emit QK_BOOT when the chord is pressed */
combo_t key_combos[] = {
    [CB_RESET] = COMBO(cb_reset_keys, QK_BOOT),
};


enum { U_DEG = 0, U_TIMES };

/* Required by UNICODEMAP_ENABLE */
// const uint32_t PROGMEM unicode_map[] = {
//     [U_DEG]   = 0x00B0, /* ° */
//     [U_TIMES] = 0x00D7, /* × */
// };

/* Tap-dance IDs */
enum {
    TD_LSFT_CAPS = 0,
    TD_ASTR_DEG,
};

/* * once → '*', twice → '°' (Opt+Shift+8), thrice → '•' (Opt+8 as an example) */
void td_astr_deg_finished(tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {
        tap_code16(KC_ASTR);
    } else if (state->count == 2) {
        // degree sign via native macOS shortcut
        register_mods(MOD_BIT(KC_LALT) | MOD_BIT(KC_LSFT));  // Option + Shift
        tap_code(KC_8);
        unregister_mods(MOD_BIT(KC_LALT) | MOD_BIT(KC_LSFT));
    } else {
        // example: bullet via Option+8 (avoids Unicode too)
        register_mods(MOD_BIT(KC_LALT));                     // Option
        tap_code(KC_8);
        unregister_mods(MOD_BIT(KC_LALT));
    }
}


void td_astr_deg_reset(tap_dance_state_t *state, void *user_data) {
    /* no held modifier to release; nothing to do */
}

/* Triple-tap LSFT toggles CAPS; single/double/hold act as Shift */
void td_lsft_caps_finished(tap_dance_state_t *state, void *user_data) {
    if (state->count == 3 && !state->pressed) {
        tap_code(KC_CAPS);
    } else {
        register_code(KC_LSFT);
    }
}

void td_lsft_caps_reset(tap_dance_state_t *state, void *user_data) {
    if (state->count != 3 || state->pressed) {
        unregister_code(KC_LSFT);
    }
}

/* Tap-dance action table */
tap_dance_action_t tap_dance_actions[] = {
    [TD_LSFT_CAPS] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_lsft_caps_finished, td_lsft_caps_reset),
    [TD_ASTR_DEG]  = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_astr_deg_finished, td_astr_deg_reset),
};

/* Set OS Unicode input mode (choose one your OS supports) */
static inline void set_unicode_mode_portable(void) {
#ifdef UNICODE_MODE_MACOS
    set_unicode_input_mode(UNICODE_MODE_MACOS);
#elif defined(UNICODE_MODE_WINCOMPOSE)
    set_unicode_input_mode(UNICODE_MODE_WINCOMPOSE);
#elif defined(UNICODE_MODE_LINUX)
    set_unicode_input_mode(UNICODE_MODE_LINUX);
#elif defined(UC_OSX)
    set_unicode_input_mode(UC_OSX);
#elif defined(UC_WINC)
    set_unicode_input_mode(UC_WINC);
#elif defined(UC_LNX)
    set_unicode_input_mode(UC_LNX);
#endif
}

void keyboard_post_init_user(void) {
    set_unicode_mode_portable();
}


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_split_3x6_3_ex2(
  //|--------------------------------------------------------------|        |--------------|        |-----------------------------------------------------|
      KC_ESC,              KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,        XXXXXXX,XXXXXXX,        KC_Y,    KC_U,    KC_I,    KC_O,   KC_P,  KC_BSPC,
  //|--------+--------+--------+--------+--------+--------+--------|        |--------------|        |--------+--------+--------+--------+--------+--------|
      TD(TD_LSFT_CAPS),    KC_A,    KC_S,    KC_D,    KC_F,    KC_G,        XXXXXXX,XXXXXXX,        KC_H,    KC_J,    KC_K,    KC_L, KC_SCLN, KC_BSLS,
  //|--------+--------+--------+--------+--------+--------+--------|                                |--------+--------+--------+--------+--------+--------|
      KC_TAB,              KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,                                KC_N,    KC_M, KC_COMM,  KC_DOT, KC_SLSH,  KC_QUOT,
  //|--------+--------+--------+--------+--------+--------+--------|                                |--------+--------+--------+--------+--------+--------|
                                       KC_LALT,   MO(1),  GUI_T(KC_SPC),                      CTL_T(KC_ENT),   MO(2), KC_RALT
                                      //`--------------------------|                                |--------------------------|
  ),

    [1] = LAYOUT_split_3x6_3_ex2(
  //,-----------------------------------------------------|                 |-----------------|         |-----------------------------------------------------.
      KC_ESC,      KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                 XXXXXXX, XXXXXXX,         KC_6,    KC_7,    KC_8,    KC_9,    KC_0, KC_BSPC,
  //|--------+--------+--------+--------+--------+--------|                 |-----------------|         |--------+--------+--------+--------+--------+--------|
      KC_LSFT,     TD(TD_ASTR_DEG), XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,       XXXXXXX, XXXXXXX,         XXXXXXX, KC_LEFT,   KC_UP,KC_RIGHT, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------|                                             |--------+--------+--------+--------+--------+--------|
      KC_TAB,     XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                         XXXXXXX, XXXXXXX, KC_DOWN, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------+--------|                                    |--------+--------+--------+--------+--------+--------+--------|
                                          KC_LGUI, _______,  GUI_T(KC_SPC),                                    CTL_T(KC_ENT), KC_RALT,  MO(3)
                                      //|--------------------------|                                    |--------------------------|
  ),

    [2] = LAYOUT_split_3x6_3_ex2(
  //,-----------------------------------------------------.  ,---------.  ,-----------------------------------------------------.
       KC_TAB, KC_EXLM,   KC_AT, KC_HASH,  KC_DLR, KC_PERC,    XXXXXXX, XXXXXXX, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_BSPC,
  //|--------+--------+--------+--------+--------+--------|  |---------|  |--------+--------+--------+--------+--------+--------|
      KC_LCTL,  XXXXXXX, KC_PCMM,  KC_PPLS, KC_PAST, XXXXXXX,    XXXXXXX, XXXXXXX, KC_MINS,  KC_EQL, KC_LBRC, KC_RBRC, KC_BSLS,  KC_GRV,
  //|--------+--------+--------+--------+--------+--------|              |--------+--------+--------+--------+--------+--------|
      KC_LSFT, XXXXXXX, KC_PDOT, KC_PMNS, KC_PSLS, XXXXXXX,                      KC_UNDS, KC_PLUS, KC_LCBR, KC_RCBR, KC_PIPE, KC_TILD,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          KC_LGUI,   MO(3),  GUI_T(KC_SPC),     CTL_T(KC_ENT), _______, KC_RALT
                                      //`--------------------------'  `--------------------------/
  ),

    [3] = LAYOUT_split_3x6_3_ex2(
  //,-----------------------------------------------------.  ,---------.  ,-----------------------------------------------------.
      QK_BOOT, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------|  |---------|  |--------+--------+--------+--------+--------+--------|
      RM_TOGG, RM_HUEU, RM_SATU, RM_VALU, XXXXXXX, XXXXXXX,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------|              |--------+--------+--------+--------+--------+--------|
      RM_NEXT, RM_HUED, RM_SATD, RM_VALD, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          KC_LGUI, _______,  KC_SPC,     KC_ENT, _______, KC_RALT
                                      //`--------------------------'  `--------------------------'
  )
};

#ifdef ENCODER_MAP_ENABLE
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
  [0] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(KC_MPRV, KC_MNXT), ENCODER_CCW_CW(RM_VALD, RM_VALU), ENCODER_CCW_CW(KC_RGHT, KC_LEFT), },
  [1] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(KC_MPRV, KC_MNXT), ENCODER_CCW_CW(RM_VALD, RM_VALU), ENCODER_CCW_CW(KC_RGHT, KC_LEFT), },
  [2] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(KC_MPRV, KC_MNXT), ENCODER_CCW_CW(RM_VALD, RM_VALU), ENCODER_CCW_CW(KC_RGHT, KC_LEFT), },
  [3] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(KC_MPRV, KC_MNXT), ENCODER_CCW_CW(RM_VALD, RM_VALU), ENCODER_CCW_CW(KC_RGHT, KC_LEFT), },
};
#endif

