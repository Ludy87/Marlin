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
//
// ultralcd_GDEW0371W7.cpp
//

#include "../../inc/MarlinConfigPre.h"

#include "ultralcd_GDEW0371W7.h"
#include "../lcdprint.h"
#include "../fontutils.h"
#include "../../libs/numtostr.h"
#include "../ultralcd.h"
#include "epd/GxEPD2_BW.h"
#include "libs/U8g2_for_Adafruit_GFX/src/U8g2_for_Adafruit_GFX.h"

EPAPER_CLASS epaper(GxEPD2_371(SW_EPAPER_CS_PIN, SW_EPAPER_DC_PIN, SW_EPAPER_RST_PIN, SW_EPAPER_BUSY_PIN));
U8G2_ADAFRUIT_GFX_CLASS u8g2_gfx;

#include "epd/marlin_old_250.h"
#include "epd/marlin.h"

#define marlin_old_250_width 240

#define MARLIN_BMP_BYTEWIDTH CEILING(marlin_old_250_width, 8)
#define MARLIN_BMP_HEIGHT (sizeof(marlin_old_250_bits) / (MARLIN_BMP_BYTEWIDTH))

constexpr bool two_part = ((LCD_PIXEL_HEIGHT) - (MARLIN_BMP_HEIGHT)) < ((FONT_ASCENT) * 2);

void MarlinUI::draw_marlin_bootscreen(const bool line2/*=false*/) {

    // Determine text space needed
    constexpr uint16_t width = LCD_PIXEL_WIDTH, height = LCD_PIXEL_HEIGHT;

    constexpr uint8_t text_width_1 = uint8_t((sizeof(SHORT_BUILD_VERSION) - 1) * (FONT_WIDTH) -1), // 108
                      text_width_2 = uint8_t((sizeof(MARLIN_WEBSITE_URL) - 1) * (FONT_WIDTH) -1),  // 171
                    text_max_width = _MAX(text_width_1, text_width_2),
                 text_total_height = (FONT_HEIGHT) * 2,
                            rspace = width - (marlin_old_250_width);

    uint16_t offx, offy, txt_base, txt_offx_1, txt_offx_2;

    // Can the text fit to the right of the bitmap?
    if (text_max_width < rspace) {
      constexpr uint16_t inter = (width - text_max_width - (marlin_old_250_width)) / 2; // Evenly distribute horizontal space
      offx = inter;                               // First the boot logo...
      offy = (height - (MARLIN_BMP_HEIGHT)) / 2;  // ...V-aligned in the full height
      txt_offx_1 = (marlin_old_250_width) + ((width - (marlin_old_250_width)) - text_width_1) / 2; // Text right of the bitmap
      txt_offx_2 = (marlin_old_250_width) + ((width - (marlin_old_250_width)) - text_width_2) / 2;
      txt_base = (MARLIN_BMP_HEIGHT) - text_total_height;
    }
    else {
      constexpr int8_t inter = (height - text_total_height - (MARLIN_BMP_HEIGHT)) / 3; // Evenly distribute vertical space
      offx = rspace / 2;                        // Center the boot logo in the whole space
      offy = inter;                             // V-align boot logo proportionally
      txt_offx_1 = (width - text_width_1) / 2;  // Text 1 centered
      txt_offx_2 = (width - text_width_2) / 2;  // Text 2 centered
      txt_base = (MARLIN_BMP_HEIGHT) + text_total_height - (FONT_DESCENT);   // Even spacing looks best
    }
    NOLESS(offx, uint16_t(0));
    NOLESS(offy, uint16_t(0));

    #define MRLIN_BANNER_WIDTH 416
    #define MRLIN_BANNER_HEIGHT 42
    #define MARLIN_BMP_Y (MARLIN_BMP_HEIGHT) + (MRLIN_BANNER_HEIGHT) + (FONT_HEIGHT)

    const char * credit[] = {
      "» Scott Lahteine    [@thinkyhead] - USA",   "» Roxanne Neufeld   [@Roxy-3D] - USA",
      "» Bob Kuhn          [@Bob-the-Kuhn] - USA", "» Chris Pepper      [@p3p] - UK",
      "» João Brazio       [@jbrazio] - Portugal", "» Erik van der Zalm [@ErikZalm] - Netherlands" };

    auto _draw_bootscreen_bmp = [&](const uint8_t *bitmap) {
      lcd_drawXBitmap(0, 0, marlin_old_250_bits, marlin_old_250_width, MARLIN_BMP_HEIGHT);
      if (!two_part || !line2) lcd_put_u8str(txt_offx_1, txt_base - (FONT_HEIGHT), SHORT_BUILD_VERSION);
      if (!two_part ||  line2) lcd_put_u8str(txt_offx_2, txt_base, MARLIN_WEBSITE_URL);
      lcd_drawXBitmap(offx, MARLIN_BMP_HEIGHT + 5, marlin_banner, MRLIN_BANNER_WIDTH, MRLIN_BANNER_HEIGHT);
      for (uint8_t i = 0; i < COUNT(credit); i++) {
        lcd_put_u8str(0, MARLIN_BMP_Y * (i + 1), credit[i]);
      }
    };

    auto draw_bootscreen_bmp = [&](const uint8_t *bitmap) {
      epaper.firstPage(); do { _draw_bootscreen_bmp(bitmap); } while (epaper.nextPage());
    };

    #if DISABLED(BOOT_MARLIN_LOGO_ANIMATED)
      draw_bootscreen_bmp(marlin_old_250_bits);
    #else
      constexpr millis_t d = MARLIN_BOOTSCREEN_FRAME_TIME;
      LOOP_L_N(f, COUNT(marlin_bootscreen_animation)) {
        draw_bootscreen_bmp((uint8_t*)pgm_read_ptr(&marlin_bootscreen_animation[f]));
        if (d) safe_delay(d);
      }
    #endif
  }

  // Show the Marlin bootscreen, with the u8g loop and delays
  void MarlinUI::show_marlin_bootscreen() {
    constexpr uint8_t pages = two_part ? 2 : 1;
    for (uint8_t q = pages; q--;) {
      draw_marlin_bootscreen(q == 0);
    }
  }

void MarlinUI::show_bootscreen() {
  #if ENABLED(SHOW_CUSTOM_BOOTSCREEN)
    show_custom_bootscreen();
  #endif
  show_marlin_bootscreen();
}

void MarlinUI::init_lcd() {
  #if defined(SW_EPAPER_SCK) && defined(SW_EPAPER_MOSI)
    epaper.epd2.init(SW_EPAPER_SCK, SW_EPAPER_MOSI, true, false);
  #endif
  epaper.init();
  epaper.clearScreen();
  epaper.setFullWindow();
  #if ENABLED(E_SCREEN_ROT_90)   // rotation
    epaper.setRotation(1);
  #elif ENABLED(E_SCREEN_ROT_180)
    epaper.setRotation(2);
  #elif ENABLED(E_SCREEN_ROT_270)
    epaper.setRotation(3);
  #endif
  epaper.setTextColor(GxEPD_BLACK);
  u8g2_gfx.begin(epaper);
  u8g2_gfx.setFontMode(0);
  u8g2_gfx.setFontDirection(0);
  u8g2_gfx.setBackgroundColor(GxEPD_WHITE);
  u8g2_gfx.setForegroundColor(GxEPD_BLACK);
  u8g2_gfx.setFont(u8g2_font_9x15_tf);
}

// The kill screen is displayed for unrecoverable conditions
void MarlinUI::draw_kill_screen() {
  const lcd_uint_t h4 = epaper.height() / 4;
  epaper.firstPage();
  do {
    lcd_put_u8str(0, h4 * 1, status_message);
    lcd_put_u8str(0, h4 * 2, GET_TEXT(MSG_HALTED));
    lcd_put_u8str(0, h4 * 3, GET_TEXT(MSG_PLEASE_RESET));
  } while (epaper.nextPage());
}

void MarlinUI::clear_lcd() {
  epaper.clearScreen();
} // Automatically cleared by Picture Loop

bool MarlinUI::detected() { return true; }
