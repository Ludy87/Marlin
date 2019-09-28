
#include "../../inc/MarlinConfigPre.h"

#if HAS_GRAPHICAL_LCD && ENABLED(EPAPER)

#include "ultralcd_GDEW0371W7.h"

#include "../ultralcd.h"
#include "../../Marlin.h"
#include "../fontutils.h"
#include "../lcdprint.h"

void lcd_moveto(const lcd_uint_t col, const lcd_uint_t row) {
  u8g2_gfx.setCursor(col, row);
}

void lcd_put_int(const int i) { u8g2_gfx.print(i); }

int lcd_put_wchar_max(const wchar_t c, pixel_len_t max_length) {
    if (c < 256) {
    u8g2_gfx.print((char)c);
    return 1;
  }
  uint16_t x = u8g2_gfx.getCursorX(), y = u8g2_gfx.getCursorY();
  u8g2_gfx.setCursor(x, y);
  return 0;
}

int lcd_put_u8str_max(const char * utf8_str, pixel_len_t max_length) {
  uint16_t x = u8g2_gfx.getCursorX(),
           y = u8g2_gfx.getCursorY(),
         ret = u8g2_gfx.getUTF8Width(utf8_str);
  u8g2_gfx.setCursor(x, y);
  u8g2_gfx.print(utf8_str);
  return ret;
}

int lcd_put_u8str_max_P(PGM_P utf8_str_P, pixel_len_t max_length) {
  uint16_t x = u8g2_gfx.getCursorX(),  // 66 - 34
           y = u8g2_gfx.getCursorY(),  // 45 - 60
         ret = u8g2_gfx.getUTF8Width(utf8_str_P); // 107 - 170
  u8g2_gfx.setCursor(x, y);
  u8g2_gfx.print(utf8_str_P);
  return ret;
}

void lcd_put_axis(lcd_uint_t col, lcd_uint_t row, const AxisEnum axis) {
  const char a = 'X' + axis;
  u8g2_gfx.setCursor(col, row);
  u8g2_gfx.print( a );
}

#endif