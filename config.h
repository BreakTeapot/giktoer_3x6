/**
 * Copyright 2021 Charly Delay <charly@codesink.dev> (@0xcharly)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#ifndef __arm__
/* Disable unused features. */
#    define NO_ACTION_ONESHOT
#endif // __arm__

/* RGB: радуга слева направо на базовом слое (эффект 13 в Argos). Ведомой
 * половине нужен текущий слой, чтобы красить свою сторону по цвету слоя. */
#define ENABLE_RGB_MATRIX_CYCLE_LEFT_RIGHT
#define SPLIT_LAYER_STATE_ENABLE

/* Передача раскладки ПК (Raw HID) на ведомую половину, см. keymap.c. */
#define SPLIT_TRANSACTION_IDS_USER HOST_LAYOUT_SYNC

/* Комбо: клавиши берутся с базового слоя, слои для срабатывания
 * ограничены в combo_should_trigger (keymap.c). */
#define COMBO_ONLY_FROM_LAYER 0
#define COMBO_SHOULD_TRIGGER
/* Окно, за которое нужно нажать все клавиши комбо, мс. */
#define COMBO_TERM 25

/* Tap-hold: Flow Tap (тап при быстром наборе) и Chordal Hold (hold только
 * с клавишей другой руки), исключения — в keymap.c. */
#define FLOW_TAP_TERM 150
#define CHORDAL_HOLD

/* Caps Word: зажать оба home row Shift. */
#define BOTH_SHIFTS_TURNS_ON_CAPS_WORD

#undef AUTO_MOUSE_DEFAULT_LAYER
#define AUTO_MOUSE_DEFAULT_LAYER 3

#undef LED_DPI_INDICATOR_INDEX
#define LED_DPI_INDICATOR_INDEX 1 // accomodate invisible 2 first LEDs for easier DIY build debugging

#undef RGBLIGHT_LED_COUNT
#define RGBLIGHT_LED_COUNT 56

/* bk_pointing_device без Argos: модуль всё равно использует типы устройств
 * из argos.h, поэтому без Argos объявляем их здесь сами. */
#ifndef COMMUNITY_MODULE_ARGOS_ENABLE
#    define pointing_device_type_unknown 0
#    define pointing_device_type_trackpad_procyon 1
#    define pointing_device_type_trackball 2
#    define pointing_device_type_trackpad_cirque 3
#endif
