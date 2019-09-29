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
// status_screen_GDEW0371W7.cpp
// Standard Status Screen for Graphical Display
//

#include "../../inc/MarlinConfigPre.h"

#if HAS_GRAPHICAL_LCD && DISABLED(LIGHTWEIGHT_UI) && DISABLED(DOGLCD)

#include "GDEW0371W7_Statusscreen.h"
#include "ultralcd_GDEW0371W7.h"
#include "../ultralcd.h"
#include "../lcdprint.h"
#include "../../libs/numtostr.h"

#include "../../module/motion.h"
#include "../../module/temperature.h"
#include "epd/fan.h"
#include "epd/icons.h"

#if ENABLED(FILAMENT_LCD_DISPLAY)
  #include "../../feature/filwidth.h"
  #include "../../module/planner.h"
  #include "../../gcode/parser.h"
#endif

#if ENABLED(SDSUPPORT)
  #include "../../sd/cardreader.h"
#endif

#if HAS_PRINT_PROGRESS
  #include "../../module/printcounter.h"
#endif

#if DUAL_MIXING_EXTRUDER
  #include "../../feature/mixing.h"
#endif

#define X_LABEL_POS      3
#define X_VALUE_POS     (FONT_WIDTH * 2)
#define XYZ_SPACING      130
#define XYZ_BASELINE    (30 + FONT_ASCENT + FONT_DESCENT)
#define EXTRAS_BASELINE (40 + FONT_ASCENT)
#define STATUS_BASELINE (LCD_PIXEL_HEIGHT - FONT_HEIGHT)

#define DO_DRAW_LOGO    (STATUS_LOGO_WIDTH && ENABLED(CUSTOM_STATUS_SCREEN_IMAGE))
#define DO_DRAW_BED     (HAS_HEATED_BED && CHAMBER_BMPWIDTH)
#define DO_DRAW_CHAMBER (HAS_TEMP_CHAMBER && STATUS_CHAMBER_WIDTH && HOTENDS <= 4)
#define DO_DRAW_FAN     (HAS_FAN0)

//#define CHAMBER_ALT false

#if HOTENDS
  #define MAX_HOTEND_DRAW _MIN(HOTENDS, ((LCD_PIXEL_WIDTH) / (STATUS_HEATERS_XSPACE)))
#endif

//
// Before homing, blink '123' <-> '???'.
// Homed but unknown... '123' <-> '   '.
// Homed and known, display constantly.
//
FORCE_INLINE void _draw_axis_value(const AxisEnum axis, const char *value) {
  const uint16_t offs = (XYZ_SPACING + FONT_WIDTH) * axis;
  lcd_put_axis(X_LABEL_POS + offs, FONT_HEIGHT - FONT_DESCENT, axis);
  lcd_moveto(X_VALUE_POS + offs, FONT_HEIGHT - FONT_DESCENT);
  if (!TEST(axis_homed, axis))
    lcd_put_u8str(value);
  else {
    #if NONE(HOME_AFTER_DEACTIVATE, DISABLE_REDUCED_ACCURACY_WARNING)
      if (!TEST(axis_known_position, axis))
        lcd_put_u8str_P(axis == Z_AXIS ? "    N/A" : " N/A");
      else
    #endif
        lcd_put_u8str(value);
  }
}

FORCE_INLINE void _draw_centered_temp(const int16_t temp, const uint16_t tx, const uint16_t ty) {
  const char *str = i16tostr3(temp);
  const uint8_t len = str[0] != ' ' ? 3 : str[1] != ' ' ? 2 : 1;
  lcd_put_u8str(tx - len * (FONT_WIDTH) / 2 + 1, ty, &str[3-len]);
  lcd_put_u8str("°C");
}

auto position_degrees_image_degrees = [&](const uint8_t i) {
  return (FONT_HEIGHT) * i + (FONT_ASCENT) - (FONT_DESCENT);
};

FORCE_INLINE void _draw_heater_status(const heater_ind_t heater, const bool blink) {
  #if !HEATER_IDLE_HANDLER
    UNUSED(blink);
  #endif

  #if DO_DRAW_BED && DISABLED(STATUS_COMBINE_HEATERS)
    const bool isBed = heater < 0;
    #define IFBED(A,B) (isBed ? (A) : (B))
  #else
    #define IFBED(A,B) (B)
  #endif

  #if ENABLED(MARLIN_DEV_MODE)
    constexpr bool isHeat = true;
  #else
    const bool isHeat = false;
  #endif

  #if ENABLED(MARLIN_DEV_MODE)
    const float temp = 20 + (millis() >> 8) % IFBED(100, 200),
              target = IFBED(100, 200);
  #else
    const float temp = IFBED(thermalManager.degBed(), thermalManager.degHotend(heater)),
              target = IFBED(thermalManager.degTargetBed(), thermalManager.degTargetHotend(heater));
  #endif

  #define STATIC_HOTEND true
  #define HOTEND_DOT    isHeat

  #if DO_DRAW_BED
    #define STATIC_BED  true
    #define BED_DOT     isHeat
  #endif

  #define OFF_BMP(N) bitmap_heater
  #define ON_BMP(N)  bitmap_heater

  #if STATUS_HOTEND_BITMAPS > 1
    static const unsigned char* const status_hotend_gfx[STATUS_HOTEND_BITMAPS] PROGMEM = ARRAY_N(STATUS_HOTEND_BITMAPS, OFF_BMP(1), OFF_BMP(2), OFF_BMP(3), OFF_BMP(4), OFF_BMP(5), OFF_BMP(6));
    #define HOTEND_BITMAP(N,S) (unsigned char*)pgm_read_ptr(&status_hotend_gfx[(N) % (STATUS_HOTEND_BITMAPS)])
  #else
    #define HOTEND_BITMAP(N,S) bitmap_heater
  #endif

  #define BAR_TALL (STATUS_HEATERS_HEIGHT - 2)

  const float prop = target - 20,
              perc = prop > 0 && temp >= 20 ? (temp - 20) / prop : 0;
  uint8_t tall = uint8_t(perc * BAR_TALL + 0.5f);
  NOMORE(tall, BAR_TALL);


  if (IFBED(0, 1)) {
    const uint16_t hx = STATUS_HOTEND_X(heater),
                   bw = STATUS_HOTEND_WIDTH(heater) + position_degrees_image_degrees(1) - (FONT_ASCENT);
    epaper.drawBitmap(hx, bw, HOTEND_BITMAP(heater, isHeat), HEATER_BMPWIDTH, HEATER_BMPHEIGHT, GxEPD_BLACK);
    u8g2_gfx.setFontMode(1);
    u8g2_gfx.setForegroundColor(GxEPD_WHITE);
    u8g2_gfx.setCursor(hx + (FONT_WIDTH) + 1 , bw + (HEATER_BMPHEIGHT) / 2);
    u8g2_gfx.print(heater + 1);
    u8g2_gfx.setFontMode(0);
    u8g2_gfx.setForegroundColor(GxEPD_BLACK);
  }

  // Draw a heating progress bar, if specified
  #if DO_DRAW_BED && ENABLED(STATUS_HEAT_PERCENT)
    if (IFBED(true, STATIC_HOTEND) && isHeat) {
      const uint8_t bx = IFBED(STATUS_BED_X + STATUS_BED_WIDTH - 1, STATUS_HOTEND_X(heater) + STATUS_HOTEND_WIDTH(heater)) + 1;
      u8g.drawFrame(bx, STATUS_HEATERS_Y, 3, STATUS_HEATERS_HEIGHT);
      if (tall) {
        const uint8_t ph = STATUS_HEATERS_HEIGHT - 1 - tall;
        if (PAGE_OVER(STATUS_HEATERS_Y + ph))
          u8g.drawVLine(bx + 1, STATUS_HEATERS_Y + ph, tall);
      }
    }
  #endif

  #if HEATER_IDLE_HANDLER
    const bool is_idle = IFBED(thermalManager.bed_idle.timed_out, thermalManager.hotend_idle[heater].timed_out),
                dodraw = (blink || !is_idle);
  #else
    constexpr bool dodraw = true;
  #endif

  const uint16_t tx = IFBED(STATUS_BED_TEXT_X, STATUS_HOTEND_TEXT_X(heater));

  if (dodraw) _draw_centered_temp(target + 0.5f, tx, position_degrees_image_degrees(2));

  _draw_centered_temp(temp + 0.5f, tx, position_degrees_image_degrees(3) + (HEATER_BMPHEIGHT) - (FONT_DESCENT));

  if (IFBED(STATIC_BED && BED_DOT, STATIC_HOTEND && HOTEND_DOT)) {
    //u8g2_gfx.setBackgroundColor(GxEPD_BLACK); // set to white on black
    //u8g2_gfx.setForegroundColor(GxEPD_WHITE); // set to white on black
    //epaper.drawRect(tx + 20, IFBED(102-2, 20-3), 50, 50, GxEPD_BLACK);
    //u8g.drawBox(tx, IFBED(20-2, 20-3), 2, 2);
    //u8g2_gfx.setBackgroundColor(GxEPD_WHITE); // set to white on black
    //u8g2_gfx.setForegroundColor(GxEPD_BLACK); // set to white on black
  }

}
//TODO
/*#if DO_DRAW_CHAMBER

  FORCE_INLINE void _draw_chamber_status(const bool blink) {
    #if ENABLED(MARLIN_DEV_MODE)
	    const float temp = 10 + (millis() >> 8) % CHAMBER_MAXTEMP,
                target = CHAMBER_MAXTEMP;
    #else
      const float temp = thermalManager.degChamber();
      #if HAS_HEATED_CHAMBER
        const float target = thermalManager.degTargetChamber();
      #endif
    #endif

    #if !HEATER_IDLE_HANDLER
      UNUSED(blink);
    #endif

    if (PAGE_UNDER(7)) {
      #if HEATER_IDLE_HANDLER
        const bool is_idle = false, // thermalManager.chamber_idle.timed_out,
                   dodraw = (blink || !is_idle);
      #else
        constexpr bool dodraw = true;
      #endif
      #if HAS_HEATED_CHAMBER
        if (dodraw) _draw_centered_temp(target + 0.5, STATUS_CHAMBER_TEXT_X, 7);
      #else
        UNUSED(dodraw);
      #endif
    }
    if (PAGE_CONTAINS(28 - INFO_FONT_ASCENT, 28 - 1))
      _draw_centered_temp(temp + 0.5f, STATUS_CHAMBER_TEXT_X, 28);
  }

#endif // DO_DRAW_CHAMBER*/

#if ENABLED(MARLIN_DEV_MODE)
  uint16_t count_renders = 0;
  uint32_t total_cycles = 0;
#endif

void MarlinUI::draw_status_screen() {

  #if ENABLED(MARLIN_DEV_MODE)
    if (first_page) count_renders++;
  #endif

  static char xstring[5], ystring[5], zstring[8];
  #if ENABLED(FILAMENT_LCD_DISPLAY)
    static char wstring[5], mstring[4];
  #endif

  // At the first page, generate new display values
  if (first_page) {
    strcpy(xstring, ftostr4sign(LOGICAL_X_POSITION(current_position[X_AXIS])));
    strcpy(ystring, ftostr4sign(LOGICAL_Y_POSITION(current_position[Y_AXIS])));
    strcpy(zstring, ftostr52sp( LOGICAL_Z_POSITION(current_position[Z_AXIS])));
    /*#if ENABLED(FILAMENT_LCD_DISPLAY)
      strcpy(wstring, ftostr12ns(filament_width_meas));
      strcpy(mstring, i16tostr3(100.0 * (
          parser.volumetric_enabled
            ? planner.volumetric_area_nominal / planner.volumetric_multiplier[FILAMENT_SENSOR_EXTRUDER_NUM]
            : planner.volumetric_multiplier[FILAMENT_SENSOR_EXTRUDER_NUM]
        )
      ));
    #endif*/
  }

  const bool blink = get_blink();

  // Status Menu Font
  //set_font(FONT_STATUSMENU);

  #if ENABLED(MARLIN_DEV_MODE)
    TCNT5 = 0;
  #endif

  /*#if DO_DRAW_LOGO
    if (PAGE_CONTAINS(STATUS_LOGO_Y, STATUS_LOGO_Y + STATUS_LOGO_HEIGHT - 1))
      u8g.drawBitmapP(STATUS_LOGO_X, STATUS_LOGO_Y, STATUS_LOGO_BYTEWIDTH, STATUS_LOGO_HEIGHT, status_logo_bmp);
  #endif*/

  /*#if STATUS_HEATERS_WIDTH
    // Draw all heaters (and maybe the bed) in one go
    if (PAGE_CONTAINS(STATUS_HEATERS_Y, STATUS_HEATERS_Y + STATUS_HEATERS_HEIGHT - 1))
      u8g.drawBitmapP(STATUS_HEATERS_X, STATUS_HEATERS_Y, STATUS_HEATERS_BYTEWIDTH, STATUS_HEATERS_HEIGHT, status_heaters_bmp);
  #endif*/

  #if DO_DRAW_BED && DISABLED(STATUS_COMBINE_HEATERS)
    #define BED_BITMAP(S) bitmap_bed

    #define STATUS_BED_HEIGHT BED_BMPHEIGHT
    #define STATUS_BED_Y (100 - STATUS_BED_HEIGHT)
    //#define STATUS_BED_X (8)

    const uint16_t bedx = STATUS_BED_X(12, 5) + BED_BMPWIDTH + 4, //STATUS_HOTEND6_X + HEATER_BMPWIDTH + STATUS_HEATERS_XSPACE, //STATUS_HOTEND_WIDTH(HOTENDS) + position_degrees_image_degrees(2) + (FONT_ASCENT) - (FONT_DESCENT),
                   bedy = position_degrees_image_degrees(2) - (FONT_DESCENT) + 2; // + (FONT_ASCENT) + (FONT_DESCENT);//STATUS_HOTEND6_X + HEATER_BMPWIDTH + STATUS_HEATERS_XSPACE;

u8g2_gfx.setCursor(100, 120);
u8g2_gfx.print(bedy);

    //if (PAGE_CONTAINS(bedy, bedy + bedh - 1))
    epaper.drawBitmap(bedx, bedy, bitmap_bed, BED_BMPWIDTH, BED_BMPHEIGHT, GxEPD_BLACK);
  #endif

  /*#if DO_DRAW_CHAMBER
    #define CHAMBER_BITMAP(S) status_chamber_bmp
    const uint8_t chambery = STATUS_CHAMBER_Y(CHAMBER_ALT()),
                  chamberh = STATUS_CHAMBER_HEIGHT(CHAMBER_ALT());
    if (PAGE_CONTAINS(chambery, chambery + chamberh - 1))
      u8g.drawBitmapP(STATUS_CHAMBER_X, chambery, STATUS_CHAMBER_BYTEWIDTH, chamberh, CHAMBER_BITMAP(CHAMBER_ALT()));
  #endif*/

  #if DO_DRAW_FAN //FONT_ASCENT + FONT_HEIGHT * 2 - FONT_DESCENT
    epaper.drawXBitmap(LCD_PIXEL_WIDTH - FAN_WIDTH, position_degrees_image_degrees(2) - (FONT_DESCENT) + 2, fan_image, FAN_WIDTH, FAN_HEIGHT, GxEPD_BLACK);
  #endif// hier gehts weiter

  //
  // Temperature Graphics and Info
  //
  // Extruders
  #if HOTENDS
    for (uint16_t e = 0; e < MAX_HOTEND_DRAW; ++e)
      _draw_heater_status((heater_ind_t)e, blink);
  #endif

  // Heated bed
  /**/
  #if DO_DRAW_BED && DISABLED(STATUS_COMBINE_HEATERS)
    _draw_heater_status(H_BED, blink);
  #endif

  /*#if DO_DRAW_CHAMBER
    _draw_chamber_status(blink);
  #endif*/

  // Fan, if a bitmap was provided
  #if DO_DRAW_FAN
    char c = '%';
    uint16_t spd = thermalManager.fan_speed[0];
    if (spd) {
      #if ENABLED(ADAPTIVE_FAN_SLOWING)
        if (!blink && thermalManager.fan_speed_scaler[0] < 128) {
          spd = thermalManager.scaledFanSpeed(0, spd);
          c = '*';
        }
      #endif
      lcd_put_u8str(LCD_PIXEL_WIDTH - FAN_WIDTH, position_degrees_image_degrees(3) + (FAN_HEIGHT) - (FONT_DESCENT), i16tostr3(thermalManager.fanPercent(spd)));
      lcd_put_wchar(c);
    }
  #endif

  /*#if ENABLED(MARLIN_DEV_MODE)
    total_cycles += TCNT5;
  #endif*/

  /*#if ENABLED(SDSUPPORT)
    //
    // SD Card Symbol
    //
    if (card.isFileOpen() && PAGE_CONTAINS(42, 51)) {
      // Upper box
      u8g.drawBox(42, 42, 8, 7);     // 42-48 (or 41-47)
      // Right edge
      u8g.drawBox(50, 44, 2, 5);     // 44-48 (or 43-47)
      // Bottom hollow box
      u8g.drawFrame(42, 49, 10, 4);  // 49-52 (or 48-51)
      // Corner pixel
      u8g.drawPixel(50, 43);         // 43 (or 42)
    }
  #endif // SDSUPPORT*/

  /*#if HAS_PRINT_PROGRESS
    //
    // Progress bar frame
    //
    #define PROGRESS_BAR_X 54
    #define PROGRESS_BAR_WIDTH (LCD_PIXEL_WIDTH - PROGRESS_BAR_X)

    if (PAGE_CONTAINS(49, 52))
      u8g.drawFrame(PROGRESS_BAR_X, 49, PROGRESS_BAR_WIDTH, 4);

    const uint8_t progress = get_progress();

    if (progress > 1) {

      //
      // Progress bar solid part
      //

      if (PAGE_CONTAINS(50, 51))     // 50-51 (or just 50)
        u8g.drawBox(
          PROGRESS_BAR_X + 1, 50,
          (uint16_t)((PROGRESS_BAR_WIDTH - 2) * progress * 0.01), 2
        );

      //
      // SD Percent Complete
      //

      #if ENABLED(DOGM_SD_PERCENT)
        if (PAGE_CONTAINS(41, 48)) {
          // Percent complete
          lcd_put_u8str(55, 48, ui8tostr3(progress));
          lcd_put_wchar('%');
        }
      #endif
    }

    //
    // Elapsed Time
    //

    #if DISABLED(DOGM_SD_PERCENT)
      #define SD_DURATION_X (PROGRESS_BAR_X + (PROGRESS_BAR_WIDTH / 2) - len * (MENU_FONT_WIDTH / 2))
    #else
      #define SD_DURATION_X (LCD_PIXEL_WIDTH - len * MENU_FONT_WIDTH)
    #endif

    if (PAGE_CONTAINS(EXTRAS_BASELINE - INFO_FONT_ASCENT, EXTRAS_BASELINE - 1)) {
      char buffer[13];
      duration_t elapsed = print_job_timer.duration();
      bool has_days = (elapsed.value >= 60*60*24L);
      uint8_t len = elapsed.toDigital(buffer, has_days);
      lcd_put_u8str(SD_DURATION_X, EXTRAS_BASELINE, buffer);
    }

  #endif // HAS_PRINT_PROGRESS*/

  //
  // XYZ Coordinates
  //

  #if ENABLED(XYZ_HOLLOW_FRAME)
    #define XYZ_FRAME_TOP (29*2)
    #define XYZ_FRAME_HEIGHT ((FONT_ASCENT) + (FONT_HEIGHT))
  #else
    #define XYZ_FRAME_TOP (30*2)
    #define XYZ_FRAME_HEIGHT ((INFO_FONT_ASCENT) + 1)
  #endif

  #if ENABLED(XYZ_HOLLOW_FRAME)
    epaper.drawRect(0, 0, LCD_PIXEL_WIDTH, XYZ_FRAME_HEIGHT, GxEPD_BLACK);
    epaper.drawLine((LCD_PIXEL_WIDTH) / 3, 0, (LCD_PIXEL_WIDTH) / 3, XYZ_FRAME_HEIGHT, GxEPD_BLACK);
    epaper.drawLine((LCD_PIXEL_WIDTH) / 3 * 2, 0, (LCD_PIXEL_WIDTH) / 3 * 2, XYZ_FRAME_HEIGHT, GxEPD_BLACK);
  #else
    epaper.drawRect(0, XYZ_FRAME_TOP, LCD_PIXEL_WIDTH, XYZ_FRAME_HEIGHT, GxEPD_BLACK);
  #endif

  #if DISABLED(XYZ_HOLLOW_FRAME)
    u8g.setColorIndex(0); // white on black
  #endif

  #if DUAL_MIXING_EXTRUDER

    // Two-component mix / gradient instead of XY

    char mixer_messages[12];
    const char *mix_label;
    #if ENABLED(GRADIENT_MIX)
      if (mixer.gradient.enabled) {
        mixer.update_mix_from_gradient();
        mix_label = "Gr";
      }
      else
    #endif
      {
        mixer.update_mix_from_vtool();
        mix_label = "Mx";
      }
    sprintf_P(mixer_messages, PSTR("%s %d;%d%% "), mix_label, int(mixer.mix[0]), int(mixer.mix[1]));
    lcd_put_u8str(X_LABEL_POS, XYZ_BASELINE, mixer_messages);

  #else

    _draw_axis_value(X_AXIS, xstring);
    _draw_axis_value(Y_AXIS, ystring);

  #endif

  _draw_axis_value(Z_AXIS, zstring);

  #if DISABLED(XYZ_HOLLOW_FRAME)
    u8g.setColorIndex(1); // black on white
  #endif

  //
  // Feedrate
  //
  #define EXTRAS_2_BASELINE (EXTRAS_BASELINE + 3)

  /*//if (PAGE_CONTAINS(EXTRAS_2_BASELINE - INFO_FONT_ASCENT, EXTRAS_2_BASELINE - 1)) {
    //set_font(FONT_MENU);
    lcd_put_wchar(3, EXTRAS_2_BASELINE, LCD_STR_FEEDRATE[0]);

    //set_font(FONT_STATUSMENU);
    lcd_put_u8str(12, EXTRAS_2_BASELINE, i16tostr3(feedrate_percentage));
    lcd_put_wchar('%');

    //
    // Filament sensor display if SD is disabled
    //
    #if ENABLED(FILAMENT_LCD_DISPLAY) && DISABLED(SDSUPPORT)
      lcd_put_u8str(56, EXTRAS_2_BASELINE, wstring);
      lcd_put_u8str(102, EXTRAS_2_BASELINE, mstring);
      lcd_put_wchar('%');
      //set_font(FONT_MENU);
      lcd_put_wchar(47, EXTRAS_2_BASELINE, LCD_STR_FILAM_DIA[0]); // lcd_put_u8str_P(PSTR(LCD_STR_FILAM_DIA));
      lcd_put_wchar(93, EXTRAS_2_BASELINE, LCD_STR_FILAM_MUL[0]);
    #endif
  //}*/

  //
  // Status line
  //
  //if (PAGE_CONTAINS(STATUS_BASELINE - INFO_FONT_ASCENT, STATUS_BASELINE + INFO_FONT_DESCENT)) {
    lcd_moveto(0, STATUS_BASELINE);

    #if BOTH(FILAMENT_LCD_DISPLAY, SDSUPPORT)
      // Alternate Status message and Filament display
      if (ELAPSED(millis(), next_filament_display)) {
        lcd_put_u8str_P(PSTR(LCD_STR_FILAM_DIA));
        lcd_put_wchar(':');
        lcd_put_u8str(wstring);
        lcd_put_u8str_P(PSTR("  " LCD_STR_FILAM_MUL));
        lcd_put_wchar(':');
        lcd_put_u8str(mstring);
        lcd_put_wchar('%');
      }
      else
    #endif
        draw_status_message(blink);
  //}
}

void MarlinUI::draw_status_message(const bool blink) {

  /*#if ENABLED(MARLIN_DEV_MODE)
    if (PAGE_CONTAINS(64-8, 64-1)) {
      lcd_put_int(total_cycles);
      lcd_put_wchar('/');
      lcd_put_int(count_renders);
      lcd_put_wchar('=');
      lcd_put_int(int(total_cycles / count_renders));
      return;
    }
  #endif*/

  // Get the UTF8 character count of the string
  uint8_t slen = utf8_strlen(status_message);

  #if ENABLED(STATUS_MESSAGE_SCROLLING)

    static bool last_blink = false;

    if (slen <= LCD_WIDTH) {
      // The string fits within the line. Print with no scrolling
      lcd_put_u8str(status_message);
      while (slen < LCD_WIDTH) { lcd_put_wchar(' '); ++slen; }
    }
    else {
      // String is longer than the available space

      // Get a pointer to the next valid UTF8 character
      // and the string remaining length
      uint8_t rlen;
      const char *stat = status_and_len(rlen);
      lcd_put_u8str_max(stat, LCD_PIXEL_WIDTH);

      // If the remaining string doesn't completely fill the screen
      if (rlen < LCD_WIDTH) {
        lcd_put_wchar('.');                     // Always at 1+ spaces left, draw a dot
        uint8_t chars = LCD_WIDTH - rlen;       // Amount of space left in characters
        if (--chars) {                          // Draw a second dot if there's space
          lcd_put_wchar('.');
          if (--chars) {                        // Print a second copy of the message
            lcd_put_u8str_max(status_message, LCD_PIXEL_WIDTH - (rlen + 2) * (MENU_FONT_WIDTH));
            lcd_put_wchar(' ');
          }
        }
      }
      if (last_blink != blink) {
        last_blink = blink;
        advance_status_scroll();
      }
    }

  #else // !STATUS_MESSAGE_SCROLLING

    UNUSED(blink);

    // Just print the string to the LCD
    lcd_put_u8str_max(status_message, LCD_PIXEL_WIDTH);

    // Fill the rest with spaces
    for (; slen < LCD_WIDTH; ++slen) lcd_put_wchar(' ');

  #endif // !STATUS_MESSAGE_SCROLLING
}

#endif // HAS_GRAPHICAL_LCD && !LIGHTWEIGHT_UI && !DOGLCD
