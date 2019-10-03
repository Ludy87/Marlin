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

#define X_LABEL_POS      4
#define X_VALUE_POS     FONT_WIDTH * 2
#define XYZF_SPACING    LCD_PIXEL_WIDTH / 4
#define XYZ_BASELINE    30 + FONT_ASCENT + FONT_DESCENT
#define EXTRAS_BASELINE LCD_PIXEL_HEIGHT - FONT_HEIGHT * 3 - FONT_DESCENT - FONT_ASCENT
#define STATUS_BASELINE LCD_PIXEL_HEIGHT - FONT_HEIGHT - FONT_DESCENT

#define DO_DRAW_LOGO    (STATUS_LOGO_WIDTH && ENABLED(CUSTOM_STATUS_SCREEN_IMAGE))
#define DO_DRAW_BED     (HAS_HEATED_BED && BED_BMPWIDTH)
#define DO_DRAW_CHAMBER (HAS_TEMP_CHAMBER && CHAMBER_BMPWIDTH)
#define DO_DRAW_FAN0    HAS_FAN0
#define DO_DRAW_FAN1    HAS_FAN1
#define DO_DRAW_FAN2    HAS_FAN2
#define DO_DRAW_FAN3    HAS_FAN3
#define DO_DRAW_FAN4    HAS_FAN4
#define DO_DRAW_FAN5    HAS_FAN5

#if HOTENDS
  #define MAX_HOTEND_DRAW HOTENDS
#endif

//
// Before homing, blink '123' <-> '123*'.
// Homed but unknown... '123' <-> ' N/A'.
// Homed and known, display constantly.
//
FORCE_INLINE void _draw_axis_value(const AxisEnum axis, const char *value) {
  const uint16_t offs = (XYZF_SPACING) * axis;
  lcd_put_axis(X_LABEL_POS + offs, FONT_HEIGHT - FONT_DESCENT, axis);
  lcd_moveto(X_VALUE_POS + offs, FONT_HEIGHT - FONT_DESCENT);
  if (!TEST(axis_homed, axis)) {
    lcd_put_u8str(value);
    lcd_put_u8str("*");
  }
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

FORCE_INLINE void _draw_heater_status(const heater_ind_t heater, const bool blink) {
  #if DO_DRAW_BED && DISABLED(STATUS_COMBINE_HEATERS)
    const bool isBed = heater < 0;
    #define IFBED(A,B) (isBed ? (A) : (B))
  #else
    #define IFBED(A,B) (B)
  #endif

  const bool isHeat = false;

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
  #else
    #define STATIC_BED  false
    #define BED_DOT     false
  #endif

  #define BAR_TALL (STATUS_HEATERS_HEIGHT - 2)

  const float prop = target - 20,
              perc = prop > 0 && temp >= 20 ? (temp - 20) / prop : 0;
  uint8_t tall = uint8_t(perc * BAR_TALL + 0.5f);
  NOMORE(tall, BAR_TALL);


  if (IFBED(0, 1)) {
    const uint16_t hx = STATUS_HOTEND_X(heater),
                   bw = position_degrees_image_degrees(3) - (FONT_ASCENT);
    epaper.drawBitmap(hx, bw, STATUS_HOTEND_BITMAP, HEATER_BMPWIDTH, HEATER_BMPHEIGHT, GxEPD_BLACK);
    #if HOTENDS > 1
      u8g2_gfx.setFontMode(1);
      u8g2_gfx.setForegroundColor(GxEPD_WHITE);
      u8g2_gfx.setCursor(hx + (FONT_WIDTH) + 1 , bw + (HEATER_BMPHEIGHT) / 2);
      u8g2_gfx.print(heater + 1);
      u8g2_gfx.setFontMode(0);
      u8g2_gfx.setForegroundColor(GxEPD_BLACK);
    #endif
  }

  #if HEATER_IDLE_HANDLER
    const bool is_idle = IFBED(thermalManager.bed_idle.timed_out, thermalManager.hotend_idle[heater].timed_out),
                dodraw = (blink || !is_idle);
  #else
    constexpr bool dodraw = true;
  #endif

  const uint16_t tx = IFBED(STATUS_BED_TEXT_X, STATUS_HOTEND_TEXT_X(heater));

  if (dodraw) _draw_centered_temp(target + 0.5f, tx, position_degrees_image_degrees(2));

  _draw_centered_temp(temp + 0.5f, tx, position_degrees_image_degrees(3) + (HEATER_BMPHEIGHT) - (FONT_DESCENT));

}

#if DO_DRAW_CHAMBER

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

    #if HEATER_IDLE_HANDLER
      const bool is_idle = false, // thermalManager.chamber_idle.timed_out,
                  dodraw = (blink || !is_idle);
    #else
      constexpr bool dodraw = true;
    #endif
    #if HAS_HEATED_CHAMBER
      if (dodraw) _draw_centered_temp(target + 0.5, STATUS_CHAMBER_TEXT_X, position_degrees_image_degrees(2));
    #else
      UNUSED(dodraw);
    #endif
    _draw_centered_temp(temp + 0.5f, STATUS_CHAMBER_TEXT_X, position_degrees_image_degrees(3) + (HEATER_BMPHEIGHT) - (FONT_DESCENT));
  }

#endif // DO_DRAW_CHAMBER

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
    const xyz_pos_t lpos = current_position.asLogical();
    strcpy(xstring, ftostr4sign(lpos.x));
    strcpy(ystring, ftostr4sign(lpos.y));
    strcpy(zstring, ftostr52sp( lpos.z));
    #if ENABLED(FILAMENT_LCD_DISPLAY)
      strcpy(wstring, ftostr12ns(filwidth.measured_mm));
      strcpy(mstring, i16tostr3(planner.volumetric_percent(parser.volumetric_enabled)));
    #endif
  }

  const bool blink = get_blink();

  #if ENABLED(MARLIN_DEV_MODE)
    TCNT5 = 0;
  #endif

  #if DO_DRAW_LOGO
    #undef DO_DRAW_FAN5
    epaper.drawBitmap(STATUS_LOGO_X, STATUS_LOGO_Y, status_logo_bmp, STATUS_LOGO_WIDTH, STATUS_LOGO_HEIGHT, GxEPD_BLACK);
  #endif

  #if DO_DRAW_BED && DISABLED(STATUS_COMBINE_HEATERS)
    #define STATUS_BED_HEIGHT BED_BMPHEIGHT
    #define STATUS_BED_Y (100 - STATUS_BED_HEIGHT)

    const uint16_t bedx = STATUS_BED_BMP_X, //(13, 5) + (BED_BMPWIDTH) + 6,
                   bedy = position_degrees_image_degrees(2) - (FONT_DESCENT) + 2;

    epaper.drawBitmap(bedx, bedy, bitmap_bed, BED_BMPWIDTH, BED_BMPHEIGHT, GxEPD_BLACK);
  #endif

  #if DO_DRAW_CHAMBER // hier gehts weiter icon größe 36 width anpassen
    const uint16_t chamberx = STATUS_CHAMBER_BMP_X,
                   chambery = position_degrees_image_degrees(2) - (FONT_DESCENT) + 2;
    epaper.drawBitmap(chamberx, chambery, bitmap_chamber, CHAMBER_BMPWIDTH, CHAMBER_BMPHEIGHT, GxEPD_BLACK);
  #endif
  #if DO_DRAW_FAN0
    epaper.drawXBitmap(STATUS_FAN_BMP_X(1), STATUS_FAN_BMP_Y, fan_image, FAN_WIDTH, FAN_HEIGHT, GxEPD_BLACK);
    #if DO_DRAW_FAN1
      lcd_put_u8str(STATUS_FAN_TEXT_X(1) - 7, position_degrees_image_degrees(7), "1");
    #endif
  #endif
  #if DO_DRAW_FAN1
    epaper.drawXBitmap(STATUS_FAN_BMP_X(2), STATUS_FAN_BMP_Y, fan_image, FAN_WIDTH, FAN_HEIGHT, GxEPD_BLACK);
    lcd_put_u8str(STATUS_FAN_TEXT_X(2) - 7, position_degrees_image_degrees(7), "2");
  #endif
  #if DO_DRAW_FAN2
    epaper.drawXBitmap(STATUS_FAN_BMP_X(3), STATUS_FAN_BMP_Y, fan_image, FAN_WIDTH, FAN_HEIGHT, GxEPD_BLACK);
    lcd_put_u8str(STATUS_FAN_TEXT_X(3) - 7, position_degrees_image_degrees(7), "3");
  #endif
  #if DO_DRAW_FAN3
    epaper.drawXBitmap(STATUS_FAN_BMP_X(4), STATUS_FAN_BMP_Y, fan_image, FAN_WIDTH, FAN_HEIGHT, GxEPD_BLACK);
    lcd_put_u8str(STATUS_FAN_TEXT_X(4) - 7, position_degrees_image_degrees(7), "4");
  #endif
  #if DO_DRAW_FAN4
    epaper.drawXBitmap(STATUS_FAN_BMP_X(5), STATUS_FAN_BMP_Y, fan_image, FAN_WIDTH, FAN_HEIGHT, GxEPD_BLACK);
    lcd_put_u8str(STATUS_FAN_TEXT_X(5) - 7, position_degrees_image_degrees(7), "5");
  #endif
  #if DO_DRAW_FAN5
    epaper.drawXBitmap(STATUS_FAN_BMP_X(6), STATUS_FAN_BMP_Y, fan_image, FAN_WIDTH, FAN_HEIGHT, GxEPD_BLACK);
    lcd_put_u8str(STATUS_FAN_TEXT_X(6) - 7, position_degrees_image_degrees(7), "6");
  #endif

  //
  // Temperature Graphics and Info
  //

  // Extruders
  #if HOTENDS
    for (uint16_t e = 0; e < MAX_HOTEND_DRAW; ++e)
      _draw_heater_status((heater_ind_t)e, blink);
  #endif

  // Heated bed
  #if DO_DRAW_BED && DISABLED(STATUS_COMBINE_HEATERS)
    _draw_heater_status(H_BED, blink);
  #endif

  #if DO_DRAW_CHAMBER
    _draw_chamber_status(blink);
  #endif

  auto do_draw_fan = [&](const int i, bool no = false) {
    char c = '%';
    uint16_t spd = thermalManager.fan_speed[i];
    lcd_moveto(STATUS_FAN_TEXT_X((i + 1)), position_degrees_image_degrees(7) + (FAN_HEIGHT) - (FONT_DESCENT));
    if (no){
      lcd_put_u8str_P("n/a ");
    }
    else {
      #if ENABLED(ADAPTIVE_FAN_SLOWING)
        if (!blink && thermalManager.fan_speed_scaler[i] < 128) {
          spd = thermalManager.scaledFanSpeed(i, spd);
          c = '*';
        }
      #endif
      lcd_put_u8str(i16tostr3(thermalManager.fanPercent(spd)));
      lcd_put_wchar(c);
    }
  };

  // Fan, if a bitmap was provided
  #if DO_DRAW_FAN0
    do_draw_fan(0);
  #endif
  #if DO_DRAW_FAN1
    do_draw_fan(1);
  #endif
  #if DO_DRAW_FAN2
    do_draw_fan(2);
  #endif
  #if DO_DRAW_FAN3
    do_draw_fan(3, true);
  #endif
  #if DO_DRAW_FAN4
    do_draw_fan(4, true);
  #endif
  #if DO_DRAW_FAN5
    do_draw_fan(5, true);
  #endif

  /*#if ENABLED(MARLIN_DEV_MODE)
    total_cycles += TCNT5;
  #endif*/

  #if ENABLED(SDSUPPORT)
    #define SD_X ((LCD_PIXEL_WIDTH) - 128 - 54)
    #define SD_Y ((EXTRAS_BASELINE) - FONT_ASCENT)
    //
    // SD Card Symbol
    //
    if (!card.isFileOpen()) {
      // Upper box
      epaper.fillRect( (SD_X) + 42, (SD_Y) + 2 /*142*/, 8, 7, GxEPD_BLACK);     // 42-48 (or 41-47)
      // Right edge
      epaper.fillRect( (SD_X) + 50, (SD_Y) + 4/*144*/, 2, 5, GxEPD_BLACK);     // 44-48 (or 43-47)
      // Bottom hollow box
      epaper.drawRect( (SD_X) + 42, (SD_Y) + 9/*149*/, 10, 4, GxEPD_BLACK);  // 49-52 (or 48-51)
      // Corner pixel
      epaper.drawPixel((SD_X) + 50, (SD_Y) + 3/*143*/, GxEPD_BLACK);         // 43 (or 42)
    }
  #endif // SDSUPPORT

  #if HAS_PRINT_PROGRESS
    //
    // Progress bar frame
    //
    #define PROGRESS_BAR_X 54
    #define PROGRESS_BAR_WIDTH 128 // (LCD_PIXEL_WIDTH - PROGRESS_BAR_X)

    epaper.drawRect((LCD_PIXEL_WIDTH) - (PROGRESS_BAR_WIDTH), (EXTRAS_BASELINE), PROGRESS_BAR_WIDTH, 4, GxEPD_BLACK);

    const uint8_t progress = 100; //get_progress();
    if (progress > 1) {

      //
      // Progress bar solid part
      //
      epaper.fillRect(
        (LCD_PIXEL_WIDTH) - (PROGRESS_BAR_WIDTH), (EXTRAS_BASELINE) + 1,
        (uint16_t)((PROGRESS_BAR_WIDTH - 2) * progress * 0.01), 2, GxEPD_BLACK
      );

      //
      // SD Percent Complete
      //
      #if ENABLED(DOGM_SD_PERCENT)
        lcd_put_u8str((LCD_PIXEL_WIDTH) - (PROGRESS_BAR_WIDTH), (EXTRAS_BASELINE) - 1, ui8tostr3(progress));
        lcd_put_wchar('%');
      #endif
    }

    //
    // Elapsed Time
    //
    #if DISABLED(DOGM_SD_PERCENT)
                            // 54 + (128 / 2) - len * (9 / 2)
      #define SD_DURATION_X (LCD_PIXEL_WIDTH - (FONT_WIDTH * len))
    #else
      #define SD_DURATION_X (LCD_PIXEL_WIDTH - len * MENU_FONT_WIDTH)
    #endif

    char buffer[13];
    duration_t elapsed = millis(); //print_job_timer.duration();
    bool has_days = (elapsed.value >= 60*60*24L);
    uint8_t len = elapsed.toDigital(buffer, has_days);
    lcd_put_u8str(SD_DURATION_X, (EXTRAS_BASELINE) - 1, buffer);

  #endif // HAS_PRINT_PROGRESS

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
    epaper.drawFastVLine((LCD_PIXEL_WIDTH) / 4, 0, XYZ_FRAME_HEIGHT, GxEPD_BLACK);
    epaper.drawFastVLine((LCD_PIXEL_WIDTH) / 4 * 2, 0, XYZ_FRAME_HEIGHT, GxEPD_BLACK);
    epaper.drawFastVLine((LCD_PIXEL_WIDTH) / 4 * 3, 0, XYZ_FRAME_HEIGHT, GxEPD_BLACK);
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

  lcd_put_u8str_P((X_LABEL_POS) + (XYZF_SPACING) * 3, (FONT_HEIGHT) - (FONT_DESCENT), "FR");
  lcd_put_u8str((X_VALUE_POS) + (XYZF_SPACING) * 3 + (FONT_WIDTH), (FONT_HEIGHT) - (FONT_DESCENT), i16tostr3(feedrate_percentage));
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

  //
  // Status line
  //
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
}

void MarlinUI::draw_status_message(const bool blink) {
  lcd_put_u8str(status_message); // Just print the string to the LCD
}

#endif // HAS_GRAPHICAL_LCD && !LIGHTWEIGHT_UI && !DOGLCD
