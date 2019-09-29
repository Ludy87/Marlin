/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2019 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#pragma once

/**
 * Standard Marlin Status Screen bitmaps
 *
 * Use the Marlin Bitmap Converter to make your own:
 * http://marlinfw.org/tools/u8glib/converter.html
 */

#include "../../inc/MarlinConfig.h"
#include "ultralcd_GDEW0371W7.h"

#include "epd/icons.h"

#define STATUS_HEATERS_XSPACE 24

#define STATUS_BED_X(V,I) (V + (STATUS_HEATERS_XSPACE + HEATER_BMPWIDTH) * I)
#define STATUS_X(V,I) (HOTENDS > I ? (V + (STATUS_HEATERS_XSPACE + HEATER_BMPWIDTH) * I) : 0)

#ifndef STATUS_HOTEND_TEXT_X
  #define STATUS_HOTEND1_TEXT_X 12
  #ifdef STATUS_HOTEND1_TEXT_X
    #ifndef STATUS_HOTEND2_TEXT_X
      #define STATUS_HOTEND2_TEXT_X STATUS_X(12, 1) //12 + 24 + 30  // STATUS_HOTEND1_TEXT_X + STATUS_HEATERS_XSPACE + HEATER_BMPWIDTH
    #endif
    #ifndef STATUS_HOTEND3_TEXT_X
      #define STATUS_HOTEND3_TEXT_X STATUS_X(12, 2)
    #endif
    #ifndef STATUS_HOTEND4_TEXT_X
      #define STATUS_HOTEND4_TEXT_X STATUS_X(12, 3)
    #endif
    #ifndef STATUS_HOTEND5_TEXT_X
      #define STATUS_HOTEND5_TEXT_X STATUS_X(12, 4)
    #endif
    #ifndef STATUS_HOTEND6_TEXT_X
      #define STATUS_HOTEND6_TEXT_X STATUS_X(12, 5)
    #endif
    constexpr uint16_t status_hotend_text_x[] = ARRAY_N(HOTENDS, STATUS_HOTEND1_TEXT_X, STATUS_HOTEND2_TEXT_X, STATUS_HOTEND3_TEXT_X, STATUS_HOTEND4_TEXT_X, STATUS_HOTEND5_TEXT_X, STATUS_HOTEND6_TEXT_X);
    #define STATUS_HOTEND_TEXT_X(N) status_hotend_text_x[N]
  #endif
#endif

#define STATUS_BED_TEXT_X STATUS_X(12, 5) + BED_BMPWIDTH + 13

#ifndef STATUS_HEATERS_X
  #define STATUS_HEATERS_X 8
  #ifndef STATUS_HOTEND1_X
    #define STATUS_HOTEND1_X (HOTENDS > 0 ? 8 : 0)
  #endif
  #ifndef STATUS_HOTEND2_X
    #define STATUS_HOTEND2_X STATUS_X(8, 1)
  #endif
  #ifndef STATUS_HOTEND3_X
    #define STATUS_HOTEND3_X STATUS_X(8, 2)
  #endif
  #ifndef STATUS_HOTEND4_X
    #define STATUS_HOTEND4_X STATUS_X(8, 3)
  #endif
  #ifndef STATUS_HOTEND5_X
    #define STATUS_HOTEND5_X STATUS_X(8, 4)
  #endif
  #ifndef STATUS_HOTEND6_X
    #define STATUS_HOTEND6_X STATUS_X(8, 5)
  #endif
  constexpr uint16_t status_hotend_x[HOTENDS] = ARRAY_N(HOTENDS, STATUS_HOTEND1_X, STATUS_HOTEND2_X, STATUS_HOTEND3_X, STATUS_HOTEND4_X, STATUS_HOTEND5_X, STATUS_HOTEND6_X);
  #define STATUS_HOTEND_X(N) status_hotend_x[N]
#endif
