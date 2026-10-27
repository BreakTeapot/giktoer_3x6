/**
 * Charybdis 4x6 — кеймап, перенесённый из Argos.
 * Собирается в Bastardkb/qmk_userspace с модулем bastardkb/bk_pointing_device
 * (см. keymap.json) — без него не будет PZOOM, DRGSCRL и т.п.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include QMK_KEYBOARD_H
#include <stdlib.h>
#include "bk_pointing_modes.h"
#include "raw_hid.h"
#include "transactions.h"

// ── Настройки ─────────────────────────────────────────────────────────────────
// DPI трекбола по режимам, шаг 100. Обычный: 100…3100, остальные: 100…2100.
#define TB_NORMAL_DPI     800
#define TB_SNIPING_DPI    500
#define TB_DRAGSCROLL_DPI 500
// TB_TABS: сколько прокрутить трекбол по X, чтобы переключить одну вкладку.
#define TB_TABS_THRESHOLD 120
// Сколько ждать, пока хост подтвердит переключение раскладки, прежде чем печатать, мс.
#define HOST_EN_TIMEOUT_MS 300
// Сколько держать комбинацию для входа в бутлоадер, мс.
#define BOOT_HOLD_MS 1000
// RGB: яркость 0…255 (на всех слоях) и скорость радуги на базовом слое.
#define RGB_BRIGHTNESS 125
#define RGB_SPEED      50

enum charybdis_keymap_layers {
    LAYER_BASE = 0,
    LAYER_NUMBERS, // 1
    LAYER_UTILITY, // 2
    LAYER_POINTER, // 3
    LAYER_FN,      // 4 — F1–F12, Numbers+Utility
    LAYER_GAME,    // 5 — игровой, включается TG
};

enum charybdis_keymap_keycodes {
    // Пока зажата: трекбол влево — Ctrl+PgUp, вправо — Ctrl+PgDn.
    TB_TABS = SAFE_RANGE,
    // Win зажимается, тапается Пробел, затем Win отпускается.
    WIN_SPC,
    // Фигурные скобки, Shift инвертирован: без Shift — {, с Shift — [.
    INV_LCB,
    INV_RCB,
};

// ── Большие пальцы ────────────────────────────────────────────────────────────
#define RAI_SPC LT(LAYER_UTILITY, KC_SPC)
#define NUMBERS MO(LAYER_NUMBERS)
#define CTL_ESC LCTL_T(KC_ESC)

// ── Home row mods, база: Shift / Alt / GUI / Ctrl ─────────────────────────────
#define HM_A    LSFT_T(KC_A)
#define HM_S    LALT_T(KC_S)
#define HM_D    LGUI_T(KC_D)
#define PT_F    LT(LAYER_POINTER, KC_F)
#define HM_J    LCTL_T(KC_J)
#define HM_K    LGUI_T(KC_K)
#define HM_L    LALT_T(KC_L)
// Правый Shift справа: оба home row Shift вместе включают Caps Word.
#define HM_SCLN RSFT_T(KC_SCLN)

// ── Home row mods, Numbers и Utility ─────────────────────────────────────────
#define HM_DOT  LSFT_T(KC_DOT)
#define HM_COMM LALT_T(KC_COMM)
#define HM_EQL  LGUI_T(KC_EQL)
// Mod-tap умеет только базовые кейкоды, поэтому тап '?' и '~' подменяется
// на Shift+кейкод в process_record_user.
#define HM_QUES LCTL_T(KC_SLSH)
#define HM_TILD LGUI_T(KC_GRV)
#define HM_4    LCTL_T(KC_4)
#define HM_5    LGUI_T(KC_5)
#define HM_6    LALT_T(KC_6)
#define HM_0    LSFT_T(KC_0)

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [LAYER_BASE] = LAYOUT(
  // ╭──────────────────────────────────────────────────────╮ ╭──────────────────────────────────────────────────────╮
        KC_ESC, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
        KC_GRV,    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,       KC_Y,    KC_U,    KC_I,    KC_O,    KC_P, KC_LBRC,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
        KC_TAB,    HM_A,    HM_S,    HM_D,    PT_F,    KC_G,       KC_H,    HM_J,    HM_K,    HM_L, HM_SCLN, KC_QUOT,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       KC_LCTL,    KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,       KC_N,    KC_M, KC_COMM,  KC_DOT, KC_SLSH, KC_RBRC,
  // ╰──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────╯
                                  CTL_ESC, RAI_SPC, MS_BTN1,      NUMBERS,    KC_R,
                                           WIN_SPC, KC_BSPC,     KC_ENT
  //                            ╰───────────────────────────╯ ╰──────────────────╯
  ),

  [LAYER_NUMBERS] = LAYOUT(
  // ╭──────────────────────────────────────────────────────╮ ╭──────────────────────────────────────────────────────╮
       XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       XXXXXXX, XXXXXXX, KC_AMPR, KC_DQUO,   KC_AT, KC_COLN,    KC_PLUS,    KC_7,    KC_8,    KC_9, XXXXXXX, XXXXXXX,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       KC_PERC,  HM_DOT, HM_COMM,  HM_EQL, HM_QUES, KC_EXLM,    KC_MINS,    HM_4,    HM_5,    HM_6,    HM_0, KC_CIRC,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       XXXXXXX, KC_PIPE,  KC_DLR, KC_QUOT, KC_UNDS, KC_SCLN,    KC_SLSH,    KC_1,    KC_2,    KC_3, KC_ASTR, XXXXXXX,
  // ╰──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────╯
                             _______, MO(LAYER_FN), _______,    XXXXXXX, _______,
                                           _______, _______,    _______
  //                            ╰───────────────────────────╯ ╰──────────────────╯
  ),

  [LAYER_UTILITY] = LAYOUT(
  // ╭──────────────────────────────────────────────────────╮ ╭──────────────────────────────────────────────────────╮
       XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       KC_MNXT, XXXXXXX, XXXXXXX, XXXXXXX,  KC_DEL, KC_COLN,    XXXXXXX, XXXXXXX, KC_DQUO, XXXXXXX, XXXXXXX, XXXXXXX,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       KC_MPLY, KC_LEFT,   KC_UP, KC_DOWN, KC_RGHT, KC_EXLM,    KC_MINS, HM_QUES, HM_TILD, HM_COMM,  HM_DOT, KC_BSLS,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       KC_MPRV, KC_HOME, KC_PGUP, KC_PGDN,  KC_END, KC_SCLN,    KC_SLSH,  KC_GRV, KC_QUOT, KC_HASH, XXXXXXX, XXXXXXX,
  // ╰──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────╯
                                  _______, XXXXXXX, _______,  MO(LAYER_FN), _______,
                                           _______, _______,    _______
  //                            ╰───────────────────────────╯ ╰──────────────────╯
  ),

  [LAYER_POINTER] = LAYOUT(
  // ╭──────────────────────────────────────────────────────╮ ╭──────────────────────────────────────────────────────╮
       XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       KC_MNXT, PVOLUME, XXXXXXX,   PZOOM, XXXXXXX, XXXXXXX,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       KC_MPLY, TB_TABS,   CURSOR, DRGSCRL, XXXXXXX, XXXXXXX,    XXXXXXX, KC_LCTL, KC_LGUI, KC_LALT, KC_LSFT, XXXXXXX,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       KC_MPRV, SNIPING, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  // ╰──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────╯
                                  MS_BTN2, MS_BTN3, MS_BTN1,    _______, _______,
                                           _______, MS_BTN2,    _______
  //                            ╰───────────────────────────╯ ╰──────────────────╯
  ),

  [LAYER_FN] = LAYOUT(
  // ╭──────────────────────────────────────────────────────╮ ╭──────────────────────────────────────────────────────╮
       QK_REBOOT, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, QK_CLEAR_EEPROM,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,      KC_F11,   KC_F7,   KC_F8,   KC_F9, XXXXXXX, TG(LAYER_GAME),
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       XXXXXXX, KC_LSFT, KC_LALT, KC_LGUI, KC_LCTL, XXXXXXX,      KC_F12,   KC_F4,   KC_F5,   KC_F6,   KC_F10, XXXXXXX,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,    XXXXXXX,   KC_F1,   KC_F2,   KC_F3, XXXXXXX, XXXXXXX,
  // ╰──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────╯
                                  XXXXXXX, XXXXXXX, XXXXXXX,    XXXXXXX, XXXXXXX,
                                           XXXXXXX, XXXXXXX,    XXXXXXX
  //                            ╰───────────────────────────╯ ╰──────────────────╯
  ),

  [LAYER_GAME] = LAYOUT(
  // ╭──────────────────────────────────────────────────────╮ ╭──────────────────────────────────────────────────────╮
        KC_ESC,  KC_GRV,    KC_1,    KC_2,    KC_3,    KC_4,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
        KC_ESC,  KC_TAB,    KC_Q,    KC_W,    KC_E,    KC_R,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, TO(LAYER_BASE),
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       KC_LALT, KC_LSFT,    KC_A,    KC_S,    KC_D,    KC_F,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       XXXXXXX, KC_LCTL,    KC_Z,    KC_X,    KC_C,    KC_V,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  // ╰──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────╯
                                  XXXXXXX,  KC_SPC, XXXXXXX,    XXXXXXX, XXXXXXX,
                                           XXXXXXX, XXXXXXX,    XXXXXXX
  //                            ╰───────────────────────────╯ ╰──────────────────╯
  ),
};

// Подсветка слоёв. На базовом слое — радуга слева направо. На остальных слоях
// базовый эффект не просвечивает: клавиша горит цветом из таблицы, C_OFF —
// выключена, C_FX — показывает радугу базового слоя. Слои, которых нет в
// таблице (FN), полностью тёмные.
enum layer_colors {
    C_OFF = 0,
    C_FX,
    C_PURP,
    C_GREEN,
    C_CYAN,
};

static const rgb_t layer_palette[] = {
    [C_OFF]   = {.r = 0,   .g = 0,   .b = 0},
    [C_PURP]  = {.r = 221, .g = 0,   .b = 255},
    [C_GREEN] = {.r = 33,  .g = 255, .b = 0},
    [C_CYAN]  = {.r = 0,   .g = 255, .b = 235},
};

static const uint8_t PROGMEM layer_colors[][MATRIX_ROWS][MATRIX_COLS] = {
  [LAYER_NUMBERS] = LAYOUT(
         C_OFF,  C_PURP,  C_PURP,   C_OFF,   C_OFF,  C_PURP,     C_PURP,   C_OFF,   C_OFF,  C_PURP,  C_PURP,   C_OFF,
         C_OFF,  C_PURP,   C_OFF,   C_OFF,   C_OFF,  C_PURP,     C_PURP,   C_OFF,   C_OFF,   C_OFF,  C_PURP,   C_OFF,
         C_OFF,  C_PURP,   C_OFF,   C_OFF,   C_OFF,  C_PURP,     C_PURP,   C_OFF,   C_OFF,   C_OFF,  C_PURP,   C_OFF,
        C_PURP,  C_PURP,   C_OFF,   C_OFF,   C_OFF,  C_PURP,     C_PURP,   C_OFF,   C_OFF,   C_OFF,  C_PURP,  C_PURP,
                                  C_PURP,  C_PURP,  C_PURP,     C_PURP,  C_PURP,
                                           C_PURP,  C_PURP,     C_PURP
  ),
  [LAYER_UTILITY] = LAYOUT(
         C_OFF, C_GREEN, C_GREEN,   C_OFF,   C_OFF, C_GREEN,    C_GREEN,   C_OFF,   C_OFF, C_GREEN, C_GREEN,   C_OFF,
         C_OFF, C_GREEN,   C_OFF,   C_OFF,   C_OFF, C_GREEN,    C_GREEN,   C_OFF,   C_OFF,   C_OFF, C_GREEN,   C_OFF,
         C_OFF, C_GREEN,   C_OFF,   C_OFF,   C_OFF, C_GREEN,    C_GREEN,   C_OFF,   C_OFF,   C_OFF, C_GREEN,   C_OFF,
       C_GREEN, C_GREEN,   C_OFF,   C_OFF,   C_OFF, C_GREEN,    C_GREEN,   C_OFF,   C_OFF,   C_OFF, C_GREEN, C_GREEN,
                                 C_GREEN, C_GREEN, C_GREEN,    C_GREEN, C_GREEN,
                                          C_GREEN, C_GREEN,    C_GREEN
  ),
  [LAYER_POINTER] = LAYOUT(
         C_OFF,  C_CYAN,  C_CYAN,   C_OFF,   C_OFF,  C_CYAN,     C_CYAN,   C_OFF,   C_OFF,  C_CYAN,  C_CYAN,   C_OFF,
         C_OFF,  C_CYAN,   C_OFF,   C_OFF,   C_OFF,  C_CYAN,     C_CYAN,   C_OFF,   C_OFF,   C_OFF,  C_CYAN,   C_OFF,
         C_OFF,  C_CYAN,   C_OFF,   C_OFF,   C_OFF,  C_CYAN,     C_CYAN,   C_OFF,   C_OFF,   C_OFF,  C_CYAN,   C_OFF,
        C_CYAN,  C_CYAN,   C_OFF,   C_OFF,   C_OFF,  C_CYAN,     C_CYAN,   C_OFF,   C_OFF,   C_OFF,  C_CYAN,  C_CYAN,
                                  C_CYAN,  C_CYAN,  C_CYAN,     C_CYAN,  C_CYAN,
                                           C_CYAN,  C_CYAN,     C_CYAN
  ),
  [LAYER_GAME] = LAYOUT(
         C_OFF,   C_OFF,   C_OFF,   C_OFF,   C_OFF,   C_OFF,      C_OFF,   C_OFF,   C_OFF,   C_OFF,   C_OFF,   C_OFF,
         C_OFF,   C_OFF,   C_OFF,    C_FX,   C_OFF,   C_OFF,      C_OFF,   C_OFF,   C_OFF,   C_OFF,   C_OFF, C_GREEN,
         C_OFF,   C_OFF,    C_FX,    C_FX,    C_FX,   C_OFF,      C_OFF,   C_OFF,   C_OFF,   C_OFF,   C_OFF,   C_OFF,
         C_OFF,   C_OFF,   C_OFF,   C_OFF,   C_OFF,   C_OFF,      C_OFF,   C_OFF,   C_OFF,   C_OFF,   C_OFF,   C_OFF,
                                    C_FX,    C_FX,    C_FX,       C_FX,    C_FX,
                                             C_FX,    C_FX,       C_FX
  ),
};
// clang-format on

// Комбо: одновременное нажатие двух соседних клавиш даёт скобку.
// Слева открывающие, справа зеркально — закрывающие:
//   E+R — {  (с Shift — [)      U+I — }  (с Shift — ])
//   D+F — (                     J+K — )
//   C+V — <                     M+, — >
//   Клик + Backspace (большой палец) — Win+D, свернуть все окна
// Клавиши всегда берутся с базового слоя (COMBO_ONLY_FROM_LAYER в config.h),
// а срабатывают комбо только на слоях Base, Numbers и Utility.
const uint16_t PROGMEM combo_lcb[]  = {KC_E, KC_R, COMBO_END};
const uint16_t PROGMEM combo_rcb[]  = {KC_U, KC_I, COMBO_END};
const uint16_t PROGMEM combo_lprn[] = {HM_D, PT_F, COMBO_END};
const uint16_t PROGMEM combo_rprn[] = {HM_J, HM_K, COMBO_END};
const uint16_t PROGMEM combo_lt[]   = {KC_C, KC_V, COMBO_END};
const uint16_t PROGMEM combo_gt[]   = {KC_M, KC_COMM, COMBO_END};
const uint16_t PROGMEM combo_desk[] = {MS_BTN1, KC_BSPC, COMBO_END};

combo_t key_combos[] = {
    COMBO(combo_lcb, INV_LCB),
    COMBO(combo_rcb, INV_RCB),
    COMBO(combo_lprn, KC_LPRN),
    COMBO(combo_rprn, KC_RPRN),
    COMBO(combo_lt, KC_LT),
    COMBO(combo_gt, KC_GT),
    COMBO(combo_desk, G(KC_D)),
};

// Shift + Backspace — Delete (Shift при этом не отправляется).
const key_override_t shift_bspc_del = ko_make_basic(MOD_MASK_SHIFT, KC_BSPC, KC_DEL);

const key_override_t *key_overrides[] = {
    &shift_bspc_del,
};

// ── Flow Tap и Chordal Hold ──────────────────────────────────────────────────
// Thumb-ряд — пятый ряд каждой половины (4 и 9). Проверяем по ряду, а не по
// стороне, чтобы работало при кабеле в любой половине.
static bool is_thumb_key(keypos_t key) {
    return key.row < MATRIX_ROWS && key.row % (MATRIX_ROWS / 2) == 4;
}

// Flow Tap: при быстром наборе tap-hold клавиши сразу срабатывают как тап.
// Как стандартная версия QMK, но без пробела, чтобы слой Utility включался
// сразу после буквы.
bool is_flow_tap_key(uint16_t keycode) {
    if ((get_mods() & (MOD_MASK_CG | MOD_BIT_LALT)) != 0) {
        return false; // не мешаем горячим клавишам
    }
    switch (get_tap_keycode(keycode)) {
        case KC_A ... KC_Z:
        case KC_DOT:
        case KC_COMM:
        case KC_SCLN:
        case KC_SLSH:
            return true;
    }
    return false;
}

// Chordal Hold: tap-hold клавиша + клавиша той же руки = тап. Исключения:
// F (слой Pointer — трекбол под правой рукой, а кнопки мыши на левой) и
// большие пальцы, как удерживаемые, так и нажатые вместе с home row mod.
bool get_chordal_hold(uint16_t tap_hold_keycode, keyrecord_t *tap_hold_record, uint16_t other_keycode, keyrecord_t *other_record) {
    if (tap_hold_keycode == PT_F || is_thumb_key(tap_hold_record->event.key) || is_thumb_key(other_record->event.key)) {
        return true;
    }
    return get_chordal_hold_default(tap_hold_record, other_record);
}

bool combo_should_trigger(uint16_t combo_index, combo_t *combo, uint16_t keycode, keyrecord_t *record) {
    switch (get_highest_layer(layer_state | default_layer_state)) {
        case LAYER_BASE:
        case LAYER_NUMBERS:
        case LAYER_UTILITY:
            return true;
        default:
            return false;
    }
}

// Внимание: модуль bk_pointing_device сам вызывает process_record_user, и если
// функция вернула true — ядро QMK вызовет её ещё раз для того же события.
// Поэтому побочные эффекты делаем только в ветках, которые возвращают false.
static bool tb_tabs_held = false;

// Бутлоадер: зажать на BOOT_HOLD_MS клавишу большого пальца и две верхние
// клавиши у центра той же половины. Слева — пробел (он же Utility), справа —
// Numbers. Проверяются положения в матрице, а не слои: в прошивке Charybdis
// задан MASTER_RIGHT, поэтому половина с USB всегда считает себя правой
// (ряды 5–9). При кабеле в левой половине её верхние клавиши приходят на
// [5,4]/[5,5], а пробел — на пустое место [9,4], и слой Utility не включается.
static uint8_t  boot_keys_held = 0; // биты 0–2 — ряды 0–4, биты 3–5 — ряды 5–9
static uint16_t boot_timer;

static bool boot_chord_complete(void) {
    return (boot_keys_held & 0x07) == 0x07 || (boot_keys_held & 0x38) == 0x38;
}

static uint8_t boot_key_bit(keypos_t key) {
    if (key.row >= MATRIX_ROWS) {
        return 0; // комбо и прочие виртуальные события
    }
    uint8_t shift = key.row >= MATRIX_ROWS / 2 ? 3 : 0;
    uint8_t row   = key.row % (MATRIX_ROWS / 2);
    if (row == 0 && key.col >= 4) {
        return 1 << (shift + key.col - 4); // верхние клавиши у центра
    }
    // Пробел [4,4]; на правой стороне — Numbers [9,1] или пробел левой
    // половины, подключённой по USB, [9,4].
    if (row == 4 && (key.col == 4 || (shift && key.col == 1))) {
        return 1 << (shift + 2);
    }
    return 0;
}

static void boot_chord_update(keyrecord_t *record) {
    uint8_t bit = boot_key_bit(record->event.key);
    if (!bit) {
        return;
    }
    if (!record->event.pressed) {
        boot_keys_held &= ~bit;
        return;
    }
    // Функция может вызваться дважды на одно событие — таймер запускаем
    // только в момент, когда комбинация впервые стала полной.
    if (!(boot_keys_held & bit)) {
        boot_keys_held |= bit;
        if (boot_chord_complete()) {
            boot_timer = timer_read();
        }
    }
}



// ── Связь с программой land_detect на ПК (Raw HID) ───────────────────────────
// Пакет 32 байта, код команды в data[0]. Хост сообщает текущую раскладку;
// пока он на связи, верхняя правая клавиша левой половины горит фиолетовым
// при английской раскладке, верхняя левая клавиша правой половины — при русской.
// На слоях Numbers и Utility раскладка временно переключается на EN.
enum hid_cmd {
    HID_LAYOUT_EN       = 0x01, // host → kb
    HID_LAYOUT_RU       = 0x02, // host → kb
    HID_REQUEST_EN      = 0x10, // kb → host
    HID_REQUEST_RUSSIAN = 0x11, // kb → host
    HID_RESTORE_LAYOUT  = 0x19, // kb → host
    HID_HOST_READY      = 0x20, // оба направления
};

enum host_layout {
    HOST_LAYOUT_NONE = 0, // хост не на связи
    HOST_LAYOUT_UNKNOWN,  // на связи, раскладку ещё не сообщил
    HOST_LAYOUT_EN,
    HOST_LAYOUT_RU,
};

// На ведущей половине — из Raw HID, на ведомой — из синхронизации.
static uint8_t host_layout = HOST_LAYOUT_NONE;

static bool     en_requested = false; // мы попросили EN и должны вернуть раскладку
static bool     en_pending      = false; // ждём от хоста подтверждения EN
static bool     restore_pending = false; // ждём от хоста раскладку после возврата
static uint16_t host_timer;

static void host_send(uint8_t cmd) {
    uint8_t data[32] = {cmd}; // размер пакета по протоколу
    raw_hid_send(data, sizeof(data));
}

void raw_hid_receive(uint8_t *data, uint8_t length) {
    switch (data[0]) {
        // На HOST_READY не отвечаем: хост сам отвечает на наш HOST_READY,
        // и ответ на ответ зациклил бы обмен.
        case HID_HOST_READY:
            if (host_layout == HOST_LAYOUT_NONE) {
                host_layout = HOST_LAYOUT_UNKNOWN;
            }
            break;
        case HID_LAYOUT_EN:
            host_layout     = HOST_LAYOUT_EN;
            en_pending      = false;
            restore_pending = false;
            break;
        case HID_LAYOUT_RU:
            host_layout     = HOST_LAYOUT_RU;
            restore_pending = false;
            break;
    }
}

// Символы на слоях Numbers и Utility и скобки из комбо рассчитаны на
// английскую раскладку. Пока включён любой из этих слоёв или зажато такое
// комбо, просим хост переключиться на EN, а когда не осталось ни того, ни
// другого — вернуть прежнюю раскладку. Без связи с хостом считаем, что стоит EN.
// Каждое нажатие придерживается, пока хост не подтвердит переключение
// (обычно ~1 мс, бывает до ~20 мс), но не дольше HOST_EN_TIMEOUT_MS; если
// время вышло, считаем, что хоста нет, пока он снова не пришлёт HOST_READY.
// Подтверждения возврата тоже ждём: пока оно не пришло, host_layout ещё
// показывает EN, и следующее комбо или слой не запросили бы EN заново.
static bool layer_wants_en = false; // включён Numbers или Utility
static bool combo_wants_en = false; // зажато комбо со скобкой

static void host_request_en(void) {
    host_send(HID_REQUEST_EN);
    en_pending      = true;
    restore_pending = false;
    host_timer      = timer_read();
}

static void host_restore(void) {
    host_send(HID_RESTORE_LAYOUT);
    en_pending      = false;
    restore_pending = true;
    host_timer      = timer_read();
}

static void host_update_en(void) {
    bool want_en = layer_wants_en || combo_wants_en;
    if (want_en && !en_requested && (host_layout == HOST_LAYOUT_RU || host_layout == HOST_LAYOUT_UNKNOWN)) {
        host_request_en();
        en_requested = true;
    } else if (!want_en && en_requested) {
        host_restore();
        en_requested = false;
    }
}

layer_state_t layer_state_set_user(layer_state_t state) {
    if (is_keyboard_master()) {
        layer_wants_en = layer_state_cmp(state, LAYER_NUMBERS) || layer_state_cmp(state, LAYER_UTILITY);
        host_update_en();
    }
    return state;
}

// Пакеты от хоста обычно разбирает основной цикл, но пока мы ждём внутри
// обработки нажатия, читаем их сами.
void raw_hid_task(void);

static void wait_for_host(void) {
    while ((en_pending || restore_pending) && timer_elapsed(host_timer) < HOST_EN_TIMEOUT_MS) {
        raw_hid_task();
    }
    if (en_pending || restore_pending) {
        // Хост не ответил — считаем, что его нет: печатаем как есть, как при
        // EN, и больше ничего не запрашиваем. Если программа на самом деле
        // жива, она ответит на HOST_READY и связь восстановится.
        host_layout     = HOST_LAYOUT_NONE;
        en_requested    = false;
        en_pending      = false;
        restore_pending = false;
        host_send(HID_HOST_READY);
    }
}

// Комбо со скобкой держит EN, пока зажато, — так же, как слой. Символ
// печатается при нажатии, раскладка возвращается при отпускании.
static void combo_tap_en(uint16_t keycode, keyrecord_t *record) {
    combo_wants_en = record->event.pressed;
    host_update_en();
    if (record->event.pressed) {
        wait_for_host();
        tap_code16(keycode);
    }
}

// Caps Word (оба Shift на home row): в русской раскладке буквы
// ж х ъ э б ю ё стоят на знаках препинания, их тоже набираем заглавными.
bool caps_word_press_user(uint16_t keycode) {
    switch (keycode) {
        case KC_SCLN:
        case KC_QUOT:
        case KC_LBRC:
        case KC_RBRC:
        case KC_COMM:
        case KC_DOT:
        case KC_GRV:
            if (host_layout != HOST_LAYOUT_RU) {
                return false;
            }
            add_weak_mods(MOD_BIT(KC_LSFT));
            return true;
        case KC_A ... KC_Z:
        case KC_MINS:
            add_weak_mods(MOD_BIT(KC_LSFT));
            return true;
        case KC_1 ... KC_0:
        case KC_BSPC:
        case KC_DEL:
        case KC_UNDS:
            return true;
    }
    return false;
}

static void host_layout_sync_handler(uint8_t in_len, const void *in_data, uint8_t out_len, void *out_data) {
    host_layout = *(const uint8_t *)in_data;
}

// Передаём состояние ведомой половине при изменении и раз в секунду —
// на случай, если она перезагрузилась.
static void host_layout_sync(void) {
    static uint8_t  synced = HOST_LAYOUT_NONE;
    static uint16_t timer  = 0;
    if (!is_keyboard_master() || (synced == host_layout && timer_elapsed(timer) < 1000)) {
        return;
    }
    if (transaction_rpc_send(HOST_LAYOUT_SYNC, sizeof(host_layout), &host_layout)) {
        synced = host_layout;
        timer  = timer_read();
    }
}

void housekeeping_task_user(void) {
    if (boot_chord_complete() && timer_elapsed(boot_timer) >= BOOT_HOLD_MS) {
        reset_keyboard();
    }
    host_layout_sync();
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    boot_chord_update(record);
    if (record->event.pressed) {
        wait_for_host();
    }
    switch (keycode) {
        case TB_TABS:
            tb_tabs_held = record->event.pressed;
            return false;
        case WIN_SPC:
            if (record->event.pressed) {
                register_code(KC_LGUI);
                wait_ms(10);
                tap_code_delay(KC_SPC, 10);
                unregister_code(KC_LGUI);
            }
            return false;
        case HM_QUES:
        case HM_TILD:
            if (record->tap.count && record->event.pressed) {
                tap_code16(S(QK_MOD_TAP_GET_TAP_KEYCODE(keycode)));
                return false;
            }
            break;
        case INV_LCB:
        case INV_RCB: {
            uint16_t bracket = keycode == INV_LCB ? KC_LBRC : KC_RBRC;
            uint8_t  shift   = get_mods() & MOD_MASK_SHIFT;
            del_mods(shift);
            combo_tap_en(shift ? bracket : S(bracket), record);
            add_mods(shift);
            return false;
        }
        case KC_LT: // только из комбо
        case KC_GT:
            combo_tap_en(keycode, record);
            return false;
    }
    return true;
}

// DPI трекбола по режимам. У bk_pointing_device DPI свой у каждого режима
// (обычный, снайпер, драг-скролл и т.д.), а без Argos модуль сохраняет в EEPROM
// только 1 байт флагов: после первой загрузки настройки режимов остаются
// нулевыми, и трекбол работает на минимальном DPI. Поэтому при каждом старте
// заново заполняем настройки режимов и выставляем нужные значения.
// Значения — TB_*_DPI в начале файла.

void keyboard_post_init_user(void) {
    bkpd_modes_init();
    bkpd_mode_change_dpi(MODE_SNIPING, TB_SNIPING_DPI);
    bkpd_mode_change_dpi(MODE_DRAGSCROLL, TB_DRAGSCROLL_DPI);
    bkpd_mode_change_dpi(MODE_NORMAL, TB_NORMAL_DPI);

    rgb_matrix_enable_noeeprom();
    rgb_matrix_mode_noeeprom(RGB_MATRIX_CYCLE_LEFT_RIGHT);
    rgb_matrix_sethsv_noeeprom(0, 255, RGB_BRIGHTNESS);
    rgb_matrix_set_speed_noeeprom(RGB_SPEED);

    transaction_register_rpc(HOST_LAYOUT_SYNC, host_layout_sync_handler);
    // Если программа уже запущена, она ответит HOST_READY и раскладкой.
    host_send(HID_HOST_READY);
}

static uint8_t layer_color_at(uint8_t layer, uint8_t row, uint8_t col) {
    return layer < ARRAY_SIZE(layer_colors) ? pgm_read_byte(&layer_colors[layer][row][col]) : C_OFF;
}

static void set_key_color(uint8_t row, uint8_t col, uint8_t color, uint8_t led_min, uint8_t led_max) {
    uint8_t led = g_led_config.matrix_co[row][col];
    if (led < led_min || led >= led_max) {
        return; // NO_LED или светодиод другой половины
    }
    rgb_t c = layer_palette[color];
    rgb_matrix_set_color(led, c.r * RGB_BRIGHTNESS / 255, c.g * RGB_BRIGHTNESS / 255, c.b * RGB_BRIGHTNESS / 255);
}

static void paint_host_layout(uint8_t led_min, uint8_t led_max) {
    if (host_layout == HOST_LAYOUT_NONE) {
        return;
    }
    // [0,5] — верхняя правая клавиша левой половины, [5,5] — верхняя левая правой.
    set_key_color(0, 5, host_layout == HOST_LAYOUT_EN ? C_PURP : C_OFF, led_min, led_max);
    set_key_color(5, 5, host_layout == HOST_LAYOUT_RU ? C_PURP : C_OFF, led_min, led_max);
}

static void paint_layer(uint8_t layer, uint8_t led_min, uint8_t led_max) {
    // Гасим всё, кроме клавиш C_FX, затем красим клавиши своими цветами.
    bool keep_effect[RGB_MATRIX_LED_COUNT] = {false};
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            uint8_t led = g_led_config.matrix_co[row][col];
            if (led < RGB_MATRIX_LED_COUNT && layer_color_at(layer, row, col) == C_FX) {
                keep_effect[led] = true;
            }
        }
    }
    for (uint8_t i = led_min; i < led_max; i++) {
        if (!keep_effect[i]) {
            rgb_matrix_set_color(i, 0, 0, 0);
        }
    }
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            uint8_t color = layer_color_at(layer, row, col);
            if (color != C_OFF && color != C_FX) {
                set_key_color(row, col, color, led_min, led_max);
            }
        }
    }
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    uint8_t layer = get_highest_layer(layer_state | default_layer_state);
    if (layer != LAYER_BASE) {
        paint_layer(layer, led_min, led_max);
    }
    paint_host_layout(led_min, led_max);
    return false;
}

report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    static int16_t buffer_x = 0;
    if (!tb_tabs_held) {
        buffer_x = 0;
        return mouse_report;
    }
    buffer_x += mouse_report.x;
    mouse_report.x = 0;
    mouse_report.y = 0;
    if (abs(buffer_x) > TB_TABS_THRESHOLD) {
        tap_code16(buffer_x < 0 ? C(KC_PGUP) : C(KC_PGDN));
        buffer_x = 0;
    }
    return mouse_report;
}
