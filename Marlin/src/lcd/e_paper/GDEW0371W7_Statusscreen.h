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
#include "epd/fan.h"

#define STATUS_HEATERS_XSPACE 25

#define STATUS_HEATER_X(V,I) (HOTENDS >= I ? (V + (STATUS_HEATERS_XSPACE + HEATER_BMPWIDTH) * I) : 0)

#ifndef STATUS_HOTEND_TEXT_X
  #define STATUS_HOTENDS_TEXT_X 13
  #ifndef STATUS_HOTEND1_TEXT_X
    #ifndef STATUS_HOTEND1_TEXT_X
      #define STATUS_HOTEND1_TEXT_X STATUS_HEATER_X(STATUS_HOTENDS_TEXT_X, 0)
    #endif
    #ifndef STATUS_HOTEND2_TEXT_X
      #define STATUS_HOTEND2_TEXT_X STATUS_HEATER_X(STATUS_HOTENDS_TEXT_X, 1)
    #endif
    #ifndef STATUS_HOTEND3_TEXT_X
      #define STATUS_HOTEND3_TEXT_X STATUS_HEATER_X(STATUS_HOTENDS_TEXT_X, 2)
    #endif
    #ifndef STATUS_HOTEND4_TEXT_X
      #define STATUS_HOTEND4_TEXT_X STATUS_HEATER_X(STATUS_HOTENDS_TEXT_X, 3)
    #endif
    #ifndef STATUS_HOTEND5_TEXT_X
      #define STATUS_HOTEND5_TEXT_X STATUS_HEATER_X(STATUS_HOTENDS_TEXT_X, 4)
    #endif
    #ifndef STATUS_HOTEND6_TEXT_X
      #define STATUS_HOTEND6_TEXT_X STATUS_HEATER_X(STATUS_HOTENDS_TEXT_X, 5)
    #endif
    constexpr uint16_t status_hotend_text_x[] = ARRAY_N(HOTENDS, STATUS_HOTEND1_TEXT_X, STATUS_HOTEND2_TEXT_X, STATUS_HOTEND3_TEXT_X, STATUS_HOTEND4_TEXT_X, STATUS_HOTEND5_TEXT_X, STATUS_HOTEND6_TEXT_X);
    #define STATUS_HOTEND_TEXT_X(N) status_hotend_text_x[N]
  #endif
#endif

#ifndef STATUS_HEATERS_X
  #define STATUS_HEATERS_X 9
  #ifndef STATUS_HOTEND1_X
    #define STATUS_HOTEND1_X (HOTENDS > 0 ? STATUS_HEATERS_X : 0)
  #endif
  #ifndef STATUS_HOTEND2_X
    #define STATUS_HOTEND2_X STATUS_HEATER_X(STATUS_HEATERS_X, 1)
  #endif
  #ifndef STATUS_HOTEND3_X
    #define STATUS_HOTEND3_X STATUS_HEATER_X(STATUS_HEATERS_X, 2)
  #endif
  #ifndef STATUS_HOTEND4_X
    #define STATUS_HOTEND4_X STATUS_HEATER_X(STATUS_HEATERS_X, 3)
  #endif
  #ifndef STATUS_HOTEND5_X
    #define STATUS_HOTEND5_X STATUS_HEATER_X(STATUS_HEATERS_X, 4)
  #endif
  #ifndef STATUS_HOTEND6_X
    #define STATUS_HOTEND6_X STATUS_HEATER_X(STATUS_HEATERS_X, 5)
  #endif
  constexpr uint16_t status_hotend_x[HOTENDS] = ARRAY_N(HOTENDS, STATUS_HOTEND1_X, STATUS_HOTEND2_X, STATUS_HOTEND3_X, STATUS_HOTEND4_X, STATUS_HOTEND5_X, STATUS_HOTEND6_X);
  #define STATUS_HOTEND_X(N) status_hotend_x[N]
#endif

#define STATUS_CHAMBER_TEXT_X (LCD_PIXEL_WIDTH) - (CHAMBER_BMPWIDTH) + (FONT_WIDTH)
#define STATUS_CHAMBER_BMP_X  (LCD_PIXEL_WIDTH) - (CHAMBER_BMPWIDTH)

#define STATUS_BED_BMP_X(V,I) (V + (STATUS_HEATERS_XSPACE + HEATER_BMPWIDTH) * I)
#define STATUS_BED_TEXT_X (LCD_PIXEL_WIDTH) - (CHAMBER_BMPWIDTH) - (BED_BMPWIDTH) - 2

#define STATUS_FAN_XSPACE 19
#define STATUS_FAN_BMP_X(V,I) ((V + (STATUS_FAN_XSPACE + FAN_WIDTH) * I))
#define STATUS_FAN_TEXT_X(N) (STATUS_FAN_BMP_X(5, N))