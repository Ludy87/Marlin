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
#include "epd/chamber.h"
#include "epd/heaters.h"

/*auto position_degrees_image_degrees = [&](const uint8_t i) {
  return (FONT_HEIGHT) * i + (FONT_ASCENT) - (FONT_DESCENT);
};*/

#define POS_DEG_BMP_DEG(I) (FONT_HEIGHT) * (I) + (FONT_ASCENT) - (FONT_DESCENT)

#define POS_8 (LCD_PIXEL_WIDTH) / 8

#define STATUS_HEATER_BMP_X(I)  (POS_8) * I - ((POS_8) + (HEATER_BMPWIDTH)) / 2
#define STATUS_HEATER_TEXT_X(I) STATUS_HEATER_BMP_X(I) + (FONT_WIDTH)

#ifndef STATUS_HOTEND1_TEXT_X
  #define STATUS_HOTEND1_TEXT_X STATUS_HEATER_TEXT_X(1)
  #ifndef STATUS_HOTEND2_TEXT_X
    #define STATUS_HOTEND2_TEXT_X STATUS_HEATER_TEXT_X(2)
  #endif
  #ifndef STATUS_HOTEND3_TEXT_X
    #define STATUS_HOTEND3_TEXT_X STATUS_HEATER_TEXT_X(3)
  #endif
  #ifndef STATUS_HOTEND4_TEXT_X
    #define STATUS_HOTEND4_TEXT_X STATUS_HEATER_TEXT_X(4)
  #endif
  #ifndef STATUS_HOTEND5_TEXT_X
    #define STATUS_HOTEND5_TEXT_X STATUS_HEATER_TEXT_X(5)
  #endif
  #ifndef STATUS_HOTEND6_TEXT_X
    #define STATUS_HOTEND6_TEXT_X STATUS_HEATER_TEXT_X(6)
  #endif
  constexpr uint16_t status_hotend_text_x[] = ARRAY_N(HOTENDS, STATUS_HOTEND1_TEXT_X, STATUS_HOTEND2_TEXT_X, STATUS_HOTEND3_TEXT_X, STATUS_HOTEND4_TEXT_X, STATUS_HOTEND5_TEXT_X, STATUS_HOTEND6_TEXT_X);
  #define STATUS_HOTEND_TEXT_X(N) status_hotend_text_x[N]
#endif

#ifndef STATUS_HOTEND1_X
  #define STATUS_HOTEND1_X STATUS_HEATER_BMP_X(1)
  #ifndef STATUS_HOTEND2_X
    #define STATUS_HOTEND2_X STATUS_HEATER_BMP_X(2)
  #endif
  #ifndef STATUS_HOTEND3_X
    #define STATUS_HOTEND3_X STATUS_HEATER_BMP_X(3)
  #endif
  #ifndef STATUS_HOTEND4_X
    #define STATUS_HOTEND4_X STATUS_HEATER_BMP_X(4)
  #endif
  #ifndef STATUS_HOTEND5_X
    #define STATUS_HOTEND5_X STATUS_HEATER_BMP_X(5)
  #endif
  #ifndef STATUS_HOTEND6_X
    #define STATUS_HOTEND6_X STATUS_HEATER_BMP_X(6)
  #endif
  constexpr uint16_t status_hotend_x[HOTENDS] = ARRAY_N(HOTENDS, STATUS_HOTEND1_X, STATUS_HOTEND2_X, STATUS_HOTEND3_X, STATUS_HOTEND4_X, STATUS_HOTEND5_X, STATUS_HOTEND6_X);
  #define STATUS_HOTEND_X(N) status_hotend_x[N]
#endif

#define STATUS_CHAMBER_BMP_X  (LCD_PIXEL_WIDTH) - ((POS_8) + (CHAMBER_BMP_WIDTH)) / 2
#define STATUS_CHAMBER_TEXT_X (STATUS_CHAMBER_BMP_X) + (FONT_WIDTH)

#define STATUS_BED_BMP_X      (LCD_PIXEL_WIDTH) - (POS_8) - ((POS_8) + (BED_BMPWIDTH)) / 2
#define STATUS_BED_TEXT_X     (STATUS_BED_BMP_X) + (FONT_WIDTH)

#define STATUS_FAN_BMP_X(I)   (POS_8) * (I) - ((POS_8) + (FAN_WIDTH)) / 2
#define STATUS_FAN_BMP_Y      POS_DEG_BMP_DEG(6) - (FONT_DESCENT) + 2
#define STATUS_FAN_TEXT_X(I)  STATUS_FAN_BMP_X(I)
#define STATUS_FAN_TEXT_Y     POS_DEG_BMP_DEG(7)

#if ENABLED(CUSTOM_STATUS_SCREEN_IMAGE)

  #include "../../../_Statusscreen.h"
  #ifndef STATUS_LOGO_HEIGHT
    #define STATUS_LOGO_BYTEWIDTH CEILING(STATUS_LOGO_WIDTH, 8)
    #define STATUS_LOGO_HEIGHT    (sizeof(status_logo_bmp) / (STATUS_LOGO_BYTEWIDTH))
  #endif
  #define STATUS_LOGO_X ((LCD_PIXEL_WIDTH) - (STATUS_LOGO_WIDTH))
  #undef  STATUS_LOGO_Y
  #define _STATUS_LOGO_Y POS_DEG_BMP_DEG(6) + (FONT_DESCENT) - 1
  #define STATUS_LOGO_Y (_STATUS_LOGO_Y) + ((LCD_PIXEL_HEIGHT) - (_STATUS_LOGO_Y) - (STATUS_LOGO_HEIGHT) - 65) / 2

  #ifdef STATUS_SCREENWIDTH
    #error "Your custom _Statusscreen.h needs to be converted for Marlin 2.0."
  #endif
#endif