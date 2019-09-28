/**
 * This is the core graphics library for all our displays, providing a common
 * set of graphics primitives (points, lines, circles, etc.).  It needs to be
 * paired with a hardware-specific library for each display device we carry
 * (to handle the lower-level functions).
 *
 * Adafruit invests time and resources providing this open source code, please
 * support Adafruit & open-source hardware by purchasing products from Adafruit!
 *
 * Copyright (c) 2013 Adafruit Industries.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _ADAFRUIT_GFX_H
#define _ADAFRUIT_GFX_H

#if ARDUINO >= 100
 #include "Arduino.h"
 #include "Print.h"
#else
 #include "WProgram.h"
#endif
#include "gfxfont.h"

/// A generic graphics superclass that can handle all sorts of drawing. At a minimum you can subclass and provide drawPixel(). At a maximum you can do a ton of overriding to optimize. Used for any/all Adafruit displays!
class Adafruit_GFX : public Print {

 public:

  Adafruit_GFX(uint16_t w, uint16_t h); // Constructor

  // This MUST be defined by the subclass:
  virtual void drawPixel(uint16_t x, uint16_t y, uint16_t color) = 0;    ///< Virtual drawPixel() function to draw to the screen/framebuffer/etc, must be overridden in subclass. @param x X coordinate.  @param y Y coordinate. @param color 16-bit pixel color.

  // TRANSACTION API / CORE DRAW API
  // These MAY be overridden by the subclass to provide device-specific
  // optimized code.  Otherwise 'generic' versions are used.
  virtual void startWrite();
  virtual void writePixel(uint16_t x, uint16_t y, uint16_t color);
  virtual void writeFillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
  virtual void writeFastVLine(uint16_t x, uint16_t y, uint16_t h, uint16_t color);
  virtual void writeFastHLine(uint16_t x, uint16_t y, uint16_t w, uint16_t color);
  virtual void writeLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
  virtual void endWrite();

  // CONTROL API
  // These MAY be overridden by the subclass to provide device-specific
  // optimized code.  Otherwise 'generic' versions are used.
  virtual void setRotation(uint8_t r);
  virtual void invertDisplay(boolean i);

  // BASIC DRAW API
  // These MAY be overridden by the subclass to provide device-specific
  // optimized code.  Otherwise 'generic' versions are used.
  virtual void
    // It's good to implement those, even if using transaction API
    drawFastVLine(uint16_t x, uint16_t y, uint16_t h, uint16_t color),
    drawFastHLine(uint16_t x, uint16_t y, uint16_t w, uint16_t color),
    fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color),
    fillScreen(uint16_t color),
    // Optional and probably not necessary to change
    drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color),
    drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

  // These exist only with Adafruit_GFX (no subclass overrides)
  void
    drawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color),
    drawCircleHelper(uint16_t x0, uint16_t y0, uint16_t r, uint8_t cornername,
      uint16_t color),
    fillCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color),
    fillCircleHelper(uint16_t x0, uint16_t y0, uint16_t r, uint8_t cornername,
      uint16_t delta, uint16_t color),
    drawTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
      uint16_t x2, uint16_t y2, uint16_t color),
    fillTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
      uint16_t x2, uint16_t y2, uint16_t color),
    drawRoundRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h,
      uint16_t radius, uint16_t color),
    fillRoundRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h,
      uint16_t radius, uint16_t color),
    drawBitmap(uint16_t x, uint16_t y, const uint8_t bitmap[],
      uint16_t w, uint16_t h, uint16_t color),
    drawBitmap(uint16_t x, uint16_t y, const uint8_t bitmap[],
      uint16_t w, uint16_t h, uint16_t color, uint16_t bg),
    drawBitmap(uint16_t x, uint16_t y, uint8_t *bitmap,
      uint16_t w, uint16_t h, uint16_t color),
    drawBitmap(uint16_t x, uint16_t y, uint8_t *bitmap,
      uint16_t w, uint16_t h, uint16_t color, uint16_t bg),
    drawXBitmap(uint16_t x, uint16_t y, const uint8_t bitmap[],
      uint16_t w, uint16_t h, uint16_t color),
    drawGrayscaleBitmap(uint16_t x, uint16_t y, const uint8_t bitmap[],
      uint16_t w, uint16_t h),
    drawGrayscaleBitmap(uint16_t x, uint16_t y, uint8_t *bitmap,
      uint16_t w, uint16_t h),
    drawGrayscaleBitmap(uint16_t x, uint16_t y,
      const uint8_t bitmap[], const uint8_t mask[],
      uint16_t w, uint16_t h),
    drawGrayscaleBitmap(uint16_t x, uint16_t y,
      uint8_t *bitmap, uint8_t *mask, uint16_t w, uint16_t h),
    drawRGBBitmap(uint16_t x, uint16_t y, const uint16_t bitmap[],
      uint16_t w, uint16_t h),
    drawRGBBitmap(uint16_t x, uint16_t y, uint16_t *bitmap,
      uint16_t w, uint16_t h),
    drawRGBBitmap(uint16_t x, uint16_t y,
      const uint16_t bitmap[], const uint8_t mask[],
      uint16_t w, uint16_t h),
    drawRGBBitmap(uint16_t x, uint16_t y,
      uint16_t *bitmap, uint8_t *mask, uint16_t w, uint16_t h),
    drawChar(uint16_t x, uint16_t y, unsigned char c, uint16_t color,
      uint16_t bg, uint8_t size),
    drawChar(uint16_t x, uint16_t y, unsigned char c, uint16_t color,
	      uint16_t bg, uint8_t size_x, uint8_t size_y),
    getTextBounds(const char *string, uint16_t x, uint16_t y,
      uint16_t *x1, uint16_t *y1, uint16_t *w, uint16_t *h),
    getTextBounds(const __FlashStringHelper *s, uint16_t x, uint16_t y,
      uint16_t *x1, uint16_t *y1, uint16_t *w, uint16_t *h),
    getTextBounds(const String &str, uint16_t x, uint16_t y,
      uint16_t *x1, uint16_t *y1, uint16_t *w, uint16_t *h),
    setTextSize(uint8_t s),
    setTextSize(uint8_t sx, uint8_t sy),
    setFont(const GFXfont *f = NULL);

  /**********************************************************************/
  /*!
    @brief  Set text cursor location
    @param  x    X coordinate in pixels
    @param  y    Y coordinate in pixels
  */
  /**********************************************************************/
  void setCursor(uint16_t x, uint16_t y) { cursor_x = x; cursor_y = y; }

  /**********************************************************************/
  /*!
    @brief   Set text font color with transparant background
    @param   c   16-bit 5-6-5 Color to draw text with
    @note    For 'transparent' background, background and foreground
             are set to same color rather than using a separate flag.
  */
  /**********************************************************************/
  void setTextColor(uint16_t c) { textcolor = textbgcolor = c; }

  /**********************************************************************/
  /*!
    @brief   Set text font color with custom background color
    @param   c   16-bit 5-6-5 Color to draw text with
    @param   bg  16-bit 5-6-5 Color to draw background/fill with
  */
  /**********************************************************************/
  void setTextColor(uint16_t c, uint16_t bg) {
    textcolor   = c;
    textbgcolor = bg;
  }

  /**********************************************************************/
  /*!
  @brief  Set whether text that is too long for the screen width should
          automatically wrap around to the next line (else clip right).
  @param  w  true for wrapping, false for clipping
  */
  /**********************************************************************/
  void setTextWrap(boolean w) { wrap = w; }

  /**********************************************************************/
  /*!
    @brief  Enable (or disable) Code Page 437-compatible charset.
            There was an error in glcdfont.c for the longest time -- one
            character (#176, the 'light shade' block) was missing -- this
            threw off the index of every character that followed it.
            But a TON of code has been written with the erroneous
            character indices. By default, the library uses the original
            'wrong' behavior and old sketches will still work. Pass
            'true' to this function to use correct CP437 character values
            in your code.
    @param  x  true = enable (new behavior), false = disable (old behavior)
  */
  /**********************************************************************/
  void cp437(boolean x=true) { _cp437 = x; }

#if ARDUINO >= 100
  virtual size_t write(uint8_t);
#else
  virtual void   write(uint8_t);
#endif

  /************************************************************************/
  /*!
    @brief      Get width of the display, accounting for current rotation
    @returns    Width in pixels
  */
  /************************************************************************/
  uint16_t width() const { return _width; };

  /************************************************************************/
  /*!
    @brief      Get height of the display, accounting for current rotation
    @returns    Height in pixels
  */
  /************************************************************************/
  uint16_t height() const { return _height; }

  /************************************************************************/
  /*!
    @brief      Get rotation setting for display
    @returns    0 thru 3 corresponding to 4 cardinal rotations
  */
  /************************************************************************/
  uint8_t getRotation() const { return rotation; }

  // get current cursor position (get rotation safe maximum values,
  // using: width() for x, height() for y)
  /************************************************************************/
  /*!
    @brief  Get text cursor X location
    @returns    X coordinate in pixels
  */
  /************************************************************************/
  uint16_t getCursorX() const { return cursor_x; }

  /************************************************************************/
  /*!
    @brief      Get text cursor Y location
    @returns    Y coordinate in pixels
  */
  /************************************************************************/
  uint16_t getCursorY() const { return cursor_y; };

 protected:
  void
    charBounds(char c, uint16_t *x, uint16_t *y,
      uint16_t *minx, uint16_t *miny, uint16_t *maxx, uint16_t *maxy);
  uint16_t
    WIDTH,          ///< This is the 'raw' display width - never changes
    HEIGHT;         ///< This is the 'raw' display height - never changes
  uint16_t
    _width,         ///< Display width as modified by current rotation
    _height,        ///< Display height as modified by current rotation
    cursor_x,       ///< x location to start print()ing text
    cursor_y;       ///< y location to start print()ing text
  uint16_t
    textcolor,      ///< 16-bit background color for print()
    textbgcolor;    ///< 16-bit text color for print()
  uint8_t
    textsize_x,      ///< Desired magnification in X-axis of text to print()
    textsize_y,      ///< Desired magnification in Y-axis of text to print()
    rotation;       ///< Display rotation (0 thru 3)
  boolean
    wrap,           ///< If set, 'wrap' text at right edge of display
    _cp437;         ///< If set, use correct CP437 charset (default is off)
  GFXfont
    *gfxFont;       ///< Pointer to special font
};


/// A simple drawn button UI element
class Adafruit_GFX_Button {

 public:
  Adafruit_GFX_Button();
  // "Classic" initButton() uses center & size
  void initButton(Adafruit_GFX *gfx, uint16_t x, uint16_t y,
   uint16_t w, uint16_t h, uint16_t outline, uint16_t fill,
   uint16_t textcolor, char *label, uint8_t textsize);
  void initButton(Adafruit_GFX *gfx, uint16_t x, uint16_t y,
   uint16_t w, uint16_t h, uint16_t outline, uint16_t fill,
   uint16_t textcolor, char *label, uint8_t textsize_x, uint8_t textsize_y);
  // New/alt initButton() uses upper-left corner & size
  void initButtonUL(Adafruit_GFX *gfx, uint16_t x1, uint16_t y1,
   uint16_t w, uint16_t h, uint16_t outline, uint16_t fill,
   uint16_t textcolor, char *label, uint8_t textsize);
  void initButtonUL(Adafruit_GFX *gfx, uint16_t x1, uint16_t y1,
   uint16_t w, uint16_t h, uint16_t outline, uint16_t fill,
   uint16_t textcolor, char *label, uint8_t textsize_x, uint8_t textsize_y);
  void drawButton(boolean inverted = false);
  boolean contains(uint16_t x, uint16_t y);

  /**********************************************************************/
  /*!
    @brief    Sets button state, should be done by some touch function
    @param    p  True for pressed, false for not.
  */
  /**********************************************************************/
  void press(boolean p) { laststate = currstate; currstate = p; }

  boolean justPressed();
  boolean justReleased();

  /**********************************************************************/
  /*!
    @brief    Query whether the button is currently pressed
    @returns  True if pressed
  */
  /**********************************************************************/
  boolean isPressed() { return currstate; };

 private:
  Adafruit_GFX *_gfx;
  uint16_t       _x1, _y1; // Coordinates of top-left corner
  uint16_t      _w, _h;
  uint8_t       _textsize_x;
  uint8_t       _textsize_y;
  uint16_t      _outlinecolor, _fillcolor, _textcolor;
  char          _label[10];

  boolean currstate, laststate;
};


/// A GFX 1-bit canvas context for graphics
class GFXcanvas1 : public Adafruit_GFX {
 public:
  GFXcanvas1(uint16_t w, uint16_t h);
  ~GFXcanvas1();
  void     drawPixel(uint16_t x, uint16_t y, uint16_t color),
           fillScreen(uint16_t color);
  /**********************************************************************/
  /*!
    @brief    Get a pointer to the internal buffer memory
    @returns  A pointer to the allocated buffer
  */
  /**********************************************************************/
  uint8_t *getBuffer() const { return buffer; }
 private:
  uint8_t *buffer;
};


/// A GFX 8-bit canvas context for graphics
class GFXcanvas8 : public Adafruit_GFX {
 public:
  GFXcanvas8(uint16_t w, uint16_t h);
  ~GFXcanvas8();
  void     drawPixel(uint16_t x, uint16_t y, uint16_t color),
           fillScreen(uint16_t color),
           writeFastHLine(uint16_t x, uint16_t y, uint16_t w, uint16_t color);
  /**********************************************************************/
  /*!
   @brief    Get a pointer to the internal buffer memory
   @returns  A pointer to the allocated buffer
  */
  /**********************************************************************/
  uint8_t *getBuffer() const { return buffer; }
 private:
  uint8_t *buffer;
};


///  A GFX 16-bit canvas context for graphics
class GFXcanvas16 : public Adafruit_GFX {
 public:
  GFXcanvas16(uint16_t w, uint16_t h);
  ~GFXcanvas16();
  void      drawPixel(uint16_t x, uint16_t y, uint16_t color),
            fillScreen(uint16_t color),
            byteSwap();
  /**********************************************************************/
  /*!
    @brief    Get a pointer to the internal buffer memory
    @returns  A pointer to the allocated buffer
  */
  /**********************************************************************/
  uint16_t *getBuffer() const { return buffer; }
 private:
  uint16_t *buffer;
};

#endif // _ADAFRUIT_GFX_H
