# giktoer_3x6

QMK keymap for the Bastard Keyboards Charybdis (4x6), placed under `keyboards/bastardkb/charybdis/4x6/keymaps/giktoer_3x6` in a QMK userspace.

> **Status: raw first version.** This is an early, unpolished draft — expect rough edges, leftover comments and changes to the layout.

## Files

- `keymap.c` — layers and custom logic
- `config.h` — keyboard/pointing/RGB configuration
- `rules.mk`, `keymap.json` — build settings
- `argos_rgb.h` — stub header so `bk_pointing_rgb.c` builds without the Argos module

## Build

```sh
qmk compile -kb bastardkb/charybdis/4x6 -km giktoer_3x6
```
