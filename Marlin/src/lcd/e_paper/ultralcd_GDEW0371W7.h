
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
  #if ENABLED(LCD_SCREEN_ROT_90) || ENABLED(LCD_SCREEN_ROT_180)
    #define LCD_PIXEL_WIDTH GxEPD2_371::HEIGHT
  #else
    #define LCD_PIXEL_WIDTH GxEPD2_371::WIDTH
  #endif
#endif
#ifndef LCD_PIXEL_HEIGHT
  #if ENABLED(LCD_SCREEN_ROT_90) || ENABLED(LCD_SCREEN_ROT_180)
    #define LCD_PIXEL_HEIGHT GxEPD2_371::WIDTH
  #else
    #define LCD_PIXEL_HEIGHT GxEPD2_371::HEIGHT
  #endif
#endif

#include "../fontutils.h"
#include "../lcdprint.h"

extern EPAPER_CLASS epaper;
extern U8G2_ADAFRUIT_GFX_CLASS u8g2_gfx;