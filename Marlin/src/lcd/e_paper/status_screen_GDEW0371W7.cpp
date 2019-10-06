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

#define X_LABEL_POS     4
#define Y_LABEL_POS     (FONT_HEIGHT) - (FONT_DESCENT)
#define X_VALUE_POS     (FONT_WIDTH)

#if DUAL_MIXING_EXTRUDER
  #define FIRST_LINE_SPACING (LCD_PIXEL_WIDTH) / 5 // X Y Z FR MX
#else
  #define FIRST_LINE_SPACING (LCD_PIXEL_WIDTH) / 4 // X Y Z FR
#endif

#define STATUS_BASELINE (LCD_PIXEL_HEIGHT) - (FONT_HEIGHT) - (FONT_DESCENT)
#define EXTRAS_BASELINE (LCD_PIXEL_HEIGHT) - (FONT_HEIGHT) * 3 - (FONT_DESCENT) - (FONT_ASCENT)

#define DO_DRAW_LOGO    ((STATUS_LOGO_WIDTH) && ENABLED(CUSTOM_STATUS_SCREEN_IMAGE))
#define DO_DRAW_BED     (HAS_HEATED_BED && (BED_BMP_WIDTH))
#define DO_DRAW_CHAMBER ((HAS_TEMP_CHAMBER) && (CHAMBER_BMP_WIDTH))
#define MAX_FAN         ((STATUS_LOGO_WIDTH) < ((LCD_PIXEL_WIDTH) - (POS_8) * (6)))
#define DO_DRAW_FAN0    HAS_FAN0
#define DO_DRAW_FAN1    HAS_FAN1
#define DO_DRAW_FAN2    HAS_FAN2
#define DO_DRAW_FAN3    HAS_FAN3
#define DO_DRAW_FAN4    HAS_FAN4
#define DO_DRAW_FAN5    (HAS_FAN5 && MAX_FAN)

#if HOTENDS
  #define MAX_HOTEND_DRAW HOTENDS
#endif

#define TEMP_TEXT2_Y POS_DEG_BMP_DEG(3) + (HEATER_BMP_HEIGHT) - (FONT_DESCENT)

//
// Before homing, blink '123' <-> '123*'.
// Homed but unknown... '123' <-> ' N/A'.
// Homed and known, display constantly.
//
FORCE_INLINE void _draw_axis_value(const AxisEnum axis, const char *value) {
  const uint16_t offs = (FIRST_LINE_SPACING) * axis;
  lcd_put_axis(X_LABEL_POS + offs, Y_LABEL_POS, axis);
  lcd_moveto(X_VALUE_POS + offs, Y_LABEL_POS);
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
#if HOTENDS || DO_DRAW_BED || DO_DRAW_CHAMBER
  FORCE_INLINE void _draw_centered_temp(const int16_t temp, const uint16_t tx, const uint16_t ty) {
    const char *str = i16tostr3(temp);
    const uint8_t len = str[0] != ' ' ? 3 : str[1] != ' ' ? 2 : 1;
    lcd_put_u8str(tx - len * (FONT_WIDTH) / 2 + 1, ty, &str[3-len]);
    lcd_put_u8str("°C");
  }
#endif

#if HOTENDS || DO_DRAW_BED
  FORCE_INLINE void _draw_heater_status(const heater_ind_t heater, const bool blink) {
    #if DO_DRAW_BED && DISABLED(STATUS_COMBINE_HEATERS)
      const bool isBed = heater < 0;
      #define IFBED(A,B) (isBed ? (A) : (B))
    #else
      #define IFBED(A,B) (B)
    #endif

    #if ENABLED(MARLIN_DEV_MODE)
      const float temp = 20 + (millis() >> 8) % IFBED(100, 200),
                target = IFBED(100, 200);
    #else
      const float temp = IFBED(thermalManager.degBed(), thermalManager.degHotend(heater)),
                target = IFBED(thermalManager.degTargetBed(), thermalManager.degTargetHotend(heater));
    #endif

    #define STATIC_HOTEND true

    #if DO_DRAW_BED
      #define STATIC_BED  true
    #else
      #define STATIC_BED  false
    #endif

    #define BAR_TALL (STATUS_HEATERS_HEIGHT - 2)

    const float prop = target - 20,
                perc = prop > 0 && temp >= 20 ? (temp - 20) / prop : 0;
    uint8_t tall = uint8_t(perc * BAR_TALL + 0.5f);
    NOMORE(tall, BAR_TALL);


    if (IFBED(0, 1)) {
      const uint16_t hx = STATUS_HOTEND_X(heater),
                     bw = POS_DEG_BMP_DEG(3) - (FONT_ASCENT);
      lcd_drawBitmap(hx, bw, STATUS_HOTEND_BITMAP, HEATER_BMP_WIDTH, HEATER_BMP_HEIGHT);
      #if HOTENDS > 1
        u8g2_gfx.setFontMode(1);
        #if ENABLED(HEATER_INVERT)
          u8g2_gfx.setForegroundColor(GxEPD_WHITE);
        #endif
        u8g2_gfx.setCursor(hx + (FONT_WIDTH) + 1 , bw + (HEATER_BMP_HEIGHT) / 2);
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

    if (dodraw) _draw_centered_temp(target + 0.5f, tx, POS_DEG_BMP_DEG(2));

    _draw_centered_temp(temp + 0.5f, tx, TEMP_TEXT2_Y);

  }
#endif

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
      if (dodraw) _draw_centered_temp(target + 0.5, STATUS_CHAMBER_TEXT_X, POS_DEG_BMP_DEG(2));
    #else
      UNUSED(dodraw);
    #endif
    _draw_centered_temp(temp + 0.5f, STATUS_CHAMBER_TEXT_X, TEMP_TEXT2_Y);
  }

#endif // DO_DRAW_CHAMBER

#if FAN_COUNT > 0

  FORCE_INLINE void _draw_fan_status(uint8_t i) {
    lcd_drawXBitmap(STATUS_FAN_BMP_X((i+1)), STATUS_FAN_BMP_Y, fan_image, FAN_BMP_WIDTH, FAN_BMP_HEIGHT);
    #if FAN_COUNT > 1
      lcd_put_int(STATUS_FAN_TEXT_X((i+1)) - 7, STATUS_FAN_TEXT_Y, (i+1));
    #endif
    char c = '%';
    uint16_t spd = thermalManager.fan_speed[i];
    lcd_moveto(STATUS_FAN_TEXT_X((i + 1)), STATUS_FAN_TEXT_Y + (FAN_BMP_HEIGHT) - (FONT_DESCENT));
    #if ENABLED(ADAPTIVE_FAN_SLOWING)
      if (!blink && thermalManager.fan_speed_scaler[i] < 128) {
        spd = thermalManager.scaledFanSpeed(i, spd);
        c = '*';
      }
    #endif
    lcd_put_u8str(i16tostr3(thermalManager.fanPercent(spd)));
    lcd_put_wchar(c);
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
    lcd_drawBitmap(STATUS_LOGO_X - (POS_8 *
    #if DO_DRAW_FAN5
      2
    #else
      3
    #endif
    - STATUS_LOGO_WIDTH) / 2, STATUS_LOGO_Y, status_logo_bmp, STATUS_LOGO_WIDTH, STATUS_LOGO_HEIGHT);
  #endif

  #if DO_DRAW_BED && DISABLED(STATUS_COMBINE_HEATERS)
    #define STATUS_BED_HEIGHT BED_BMP_HEIGHT
    #define STATUS_BED_Y (100 - STATUS_BED_HEIGHT)

    const uint16_t bedx = STATUS_BED_BMP_X,
                   bedy = POS_DEG_BMP_DEG(2) - (FONT_DESCENT) + 2;

    lcd_drawBitmap(bedx, bedy, bitmap_bed, BED_BMP_WIDTH, BED_BMP_HEIGHT);
  #endif

  #if DO_DRAW_CHAMBER
    const uint16_t chamberx = STATUS_CHAMBER_BMP_X,
                   chambery = POS_DEG_BMP_DEG(2) - (FONT_DESCENT) + 2;
    lcd_drawBitmap(chamberx, chambery, bitmap_chamber, CHAMBER_BMP_WIDTH, CHAMBER_BMP_HEIGHT);
  #endif

  //
  // Temperature Graphics and Info
  //

  // Extruders
  #if HOTENDS
    for (uint8_t e = 0; e < MAX_HOTEND_DRAW; ++e)
      _draw_heater_status((heater_ind_t)e, blink);
  #endif

  // Heated bed
  #if DO_DRAW_BED
    _draw_heater_status(H_BED, blink);
  #endif

  #if DO_DRAW_CHAMBER
    _draw_chamber_status(blink);
  #endif

  // Fans
  #if FAN_COUNT > 0
    for (uint8_t i = 0; i < FAN_COUNT; ++i)
      _draw_fan_status(i);
  #endif

  #if ENABLED(MARLIN_DEV_MODE)
    total_cycles += TCNT5;
  #endif

  #define PROGRESS_BAR_X 54
  #define PROGRESS_BAR_WIDTH 128

  #if ENABLED(SDSUPPORT)
    #define SD_X (LCD_PIXEL_WIDTH) - (PROGRESS_BAR_WIDTH) - (PROGRESS_BAR_X)
    #define SD_Y (EXTRAS_BASELINE) - (FONT_ASCENT)

    //
    // SD Card Symbol
    //
    if (card.isFileOpen()) {
      epaper.fillRect( (SD_X) + 42, (SD_Y) + 2,  8, 7, GxEPD_BLACK); // Upper box
      epaper.fillRect( (SD_X) + 50, (SD_Y) + 4,  2, 5, GxEPD_BLACK); // Right edge
      epaper.drawRect( (SD_X) + 42, (SD_Y) + 9, 10, 4, GxEPD_BLACK); // Bottom hollow box
      epaper.drawPixel((SD_X) + 50, (SD_Y) + 3,        GxEPD_BLACK); // Corner pixel
    }
  #endif // SDSUPPORT

  #if HAS_PRINT_PROGRESS
    //
    // Progress bar frame
    //

    epaper.drawRect((LCD_PIXEL_WIDTH) - (PROGRESS_BAR_WIDTH), EXTRAS_BASELINE, PROGRESS_BAR_WIDTH, 4, GxEPD_BLACK);

    const uint8_t progress = get_progress();
    if (progress > 0) {

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
    #define SD_DURATION_X (LCD_PIXEL_WIDTH - (FONT_WIDTH) * len)

    char buffer[13];
    duration_t elapsed = print_job_timer.duration();
    bool has_days = (elapsed.value >= 60*60*24L);
    uint8_t len = elapsed.toDigital(buffer, has_days);
    lcd_put_u8str(SD_DURATION_X, (EXTRAS_BASELINE) - 1, buffer);

  #endif // HAS_PRINT_PROGRESS

  //
  // XYZ Coordinates
  //

  #if ENABLED(XYZ_HOLLOW_FRAME)
    #define XYZ_FRAME_HEIGHT ((FONT_ASCENT) + (FONT_HEIGHT))
  #else
    #define XYZ_FRAME_HEIGHT (FONT_ASCENT) + (FONT_HEIGHT)
  #endif

  #if ENABLED(XYZ_HOLLOW_FRAME)
    epaper.drawRect(0, 0, LCD_PIXEL_WIDTH, XYZ_FRAME_HEIGHT, GxEPD_BLACK);
    epaper.drawFastVLine(FIRST_LINE_SPACING,     0, XYZ_FRAME_HEIGHT, GxEPD_BLACK);
    epaper.drawFastVLine(FIRST_LINE_SPACING * 2, 0, XYZ_FRAME_HEIGHT, GxEPD_BLACK);
    epaper.drawFastVLine(FIRST_LINE_SPACING * 3, 0, XYZ_FRAME_HEIGHT, GxEPD_BLACK);
    epaper.drawFastVLine(FIRST_LINE_SPACING * 4, 0, XYZ_FRAME_HEIGHT, GxEPD_BLACK);
  #else
    epaper.fillRect(0, 0, LCD_PIXEL_WIDTH, XYZ_FRAME_HEIGHT, GxEPD_BLACK);
    epaper.drawFastVLine(FIRST_LINE_SPACING,     0, XYZ_FRAME_HEIGHT, GxEPD_WHITE);
    epaper.drawFastVLine(FIRST_LINE_SPACING * 2, 0, XYZ_FRAME_HEIGHT, GxEPD_WHITE);
    epaper.drawFastVLine(FIRST_LINE_SPACING * 3, 0, XYZ_FRAME_HEIGHT, GxEPD_WHITE);
    epaper.drawFastVLine(FIRST_LINE_SPACING * 4, 0, XYZ_FRAME_HEIGHT, GxEPD_WHITE);
  #endif

  #if DISABLED(XYZ_HOLLOW_FRAME)
    u8g2_gfx.setFontMode(1);
    u8g2_gfx.setForegroundColor(GxEPD_WHITE);
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
    lcd_put_u8str((X_LABEL_POS) + (FIRST_LINE_SPACING) * 4 - 2, Y_LABEL_POS, mixer_messages);

  #endif

  _draw_axis_value(X_AXIS, xstring);
  _draw_axis_value(Y_AXIS, ystring);
  _draw_axis_value(Z_AXIS, zstring);

  //
  // Feedrate
  //
  lcd_put_u8str((X_LABEL_POS) + (FIRST_LINE_SPACING) * 3, Y_LABEL_POS, "FR");
  lcd_put_u8str((X_VALUE_POS) + (FIRST_LINE_SPACING) * 3 + (FONT_WIDTH) * 2, Y_LABEL_POS, i16tostr3(feedrate_percentage));
  lcd_put_wchar('%');

  #if DISABLED(XYZ_HOLLOW_FRAME)
    u8g2_gfx.setFontMode(0);
    u8g2_gfx.setForegroundColor(GxEPD_BLACK);
  #endif

  //
  // Filament sensor display
  //
  #define EXTRAS_2_BASELINE (EXTRAS_BASELINE + 3)
  #if ENABLED(FILAMENT_LCD_DISPLAY) && !DISABLED(SDSUPPORT)
    lcd_moveto(0, EXTRAS_2_BASELINE);
    lcd_put_u8str(MSG_FILAMENT);
    lcd_put_u8str(" ø:");
    lcd_put_u8str(wstring);
    lcd_put_u8str("  ¤:");
    lcd_put_u8str(mstring);
    lcd_put_u8str("%");
  #endif

  //
  // Status line
  //
  draw_status_message(blink);
}

void MarlinUI::draw_status_message(const bool blink) {
  lcd_moveto(0, STATUS_BASELINE);
  #if ENABLED(MARLIN_DEV_MODE)
    lcd_put_int(total_cycles);
    lcd_put_wchar('/');
    lcd_put_int(count_renders);
    lcd_put_wchar('=');
    lcd_put_int(int(total_cycles / count_renders));
  #else
    lcd_put_u8str(status_message); // Just print the string to the LCD
  #endif
}

#endif // HAS_GRAPHICAL_LCD && !LIGHTWEIGHT_UI && !DOGLCD
