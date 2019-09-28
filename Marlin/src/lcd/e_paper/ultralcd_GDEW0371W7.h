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

#include "../../inc/MarlinConfig.h"

#ifndef EPAPER_CLASS
  #include "epd/GxEPD2_BW.h"
//  #include <U8g2_for_Adafruit_GFX.h>
  #include "libs/U8g2_for_Adafruit_GFX/src/U8g2_for_Adafruit_GFX.h"
  #define MAX_DISPAY_BUFFER_SIZE 800
  #define MAX_HEIGHT(EPD) (EPD::HEIGHT <= MAX_DISPAY_BUFFER_SIZE / (EPD::WIDTH / 8) ? EPD::HEIGHT : MAX_DISPAY_BUFFER_SIZE / (EPD::WIDTH / 8))
  #define EPAPER_CLASS GxEPD2_BW<GxEPD2_371, MAX_HEIGHT(GxEPD2_371)>
  #define U8G2_ADAFRUIT_GFX_CLASS U8G2_FOR_ADAFRUIT_GFX
#endif

#define FONT_WIDTH     9 // 5
#define FONT_HEIGHT   15 // 7
#define FONT_ASCENT   10 // 6
#define FONT_DESCENT  -3 //-1

#ifndef LCD_PIXEL_WIDTH
  #if ENABLED(E_SCREEN_ROT_90) || ENABLED(E_SCREEN_ROT_270)
    #define LCD_PIXEL_WIDTH GxEPD2_371::HEIGHT
  #else
    #define LCD_PIXEL_WIDTH GxEPD2_371::WIDTH
  #endif
#endif
#ifndef LCD_PIXEL_HEIGHT
  #if ENABLED(E_SCREEN_ROT_90) || ENABLED(E_SCREEN_ROT_270)
    #define LCD_PIXEL_HEIGHT GxEPD2_371::WIDTH
  #else
    #define LCD_PIXEL_HEIGHT GxEPD2_371::HEIGHT
  #endif
#endif

#include "../fontutils.h"
#include "../lcdprint.h"

extern EPAPER_CLASS epaper;
extern U8G2_ADAFRUIT_GFX_CLASS u8g2_gfx;