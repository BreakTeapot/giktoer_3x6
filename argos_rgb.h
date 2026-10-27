// Заглушка: bk_pointing_rgb.c подключает argos_rgb.h без проверки на Argos,
// но ничего из него не использует. При включённом Argos берём настоящий файл.
#pragma once

#ifdef COMMUNITY_MODULE_ARGOS_ENABLE
#    include_next "argos_rgb.h"
#endif
