/**
 * Display Library for SPI e-paper panels from Dalian Good Display and boards from Waveshare.
 * Requires HW/SW SPI and Adafruit_GFX. Caution: the e-paper panels require 3.3V supply AND data lines!
 *
 * Library: https://github.com/ZinggJM/GxEPD2
 *
 * Copyright (c) 2013 Jean-Marc Zingg.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this list
 * of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation
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

#ifndef _GxEPD2_EPD_H_
#define _GxEPD2_EPD_H_

#include <Arduino.h>
#include <SPI.h>

#include "GxEPD2.h"

class GxEPD2_EPD
{
  public:
    // attributes
    const uint16_t WIDTH;
    const uint16_t HEIGHT;
    const GxEPD2::Panel panel;
    const bool hasColor;
    const bool hasPartialUpdate;
    const bool hasFastPartialUpdate;
    // constructor
    GxEPD2_EPD(int8_t cs, int8_t dc, int8_t rst, int8_t busy, int8_t busy_level, uint32_t busy_timeout,
               uint16_t w, uint16_t h, GxEPD2::Panel p, bool c, bool pu, bool fpu);
    virtual void init();
    virtual void init(bool initial, bool pulldown_rst_mode = false);
    virtual void init(int8_t sck, int8_t mosi, bool initial, bool pulldown_rst_mode = false);
    //  Support for Bitmaps (Sprites) to Controller Buffer and to Screen
    virtual void clearScreen(uint8_t value) = 0; // init controller memory and screen (default white)
    virtual void writeScreenBuffer(uint8_t value) = 0; // init controller memory (default white)
    // write to controller memory, without screen refresh; x and w should be multiple of 8
    virtual void writeImage(const uint8_t bitmap[], uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool invert = false, bool mirror_y = false, bool pgm = false) = 0;
    virtual void writeImagePart(const uint8_t bitmap[], uint16_t x_part, uint16_t y_part, uint16_t w_bitmap, uint16_t h_bitmap,
                                uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool invert = false, bool mirror_y = false, bool pgm = false) = 0;
    virtual void writeImage(const uint8_t* black, const uint8_t* color, uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool invert = false, bool mirror_y = false, bool pgm = false) = 0;
    virtual void writeImagePart(const uint8_t* black, const uint8_t* color, uint16_t x_part, uint16_t y_part, uint16_t w_bitmap, uint16_t h_bitmap,
                                uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool invert = false, bool mirror_y = false, bool pgm = false) = 0;
    // for differential update: set current and previous buffers equal (for fast partial update to work correctly)
    virtual void writeScreenBufferAgain(uint8_t value = 0xFF) // init controller memory (default white)
    {
      // most controllers with differential update do switch buffers on refresh, can use:
      writeScreenBuffer(value);
    }
    virtual void writeImageAgain(const uint8_t bitmap[], uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool invert = false, bool mirror_y = false, bool pgm = false)
    {
      // most controllers with differential update do switch buffers on refresh, can use:
      writeImage(bitmap, x, y, w, h, invert, mirror_y, pgm);
    }
    virtual void writeImagePartAgain(const uint8_t bitmap[], uint16_t x_part, uint16_t y_part, uint16_t w_bitmap, uint16_t h_bitmap,
                                     uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool invert = false, bool mirror_y = false, bool pgm = false)
    {
      // most controllers with differential update do switch buffers on refresh, can use:
      writeImagePart(bitmap, x_part, y_part, w_bitmap, h_bitmap, x, y, w, h, invert, mirror_y, pgm);
    }
    // write to controller memory, with screen refresh; x and w should be multiple of 8
    virtual void drawImage(const uint8_t bitmap[], uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool invert = false, bool mirror_y = false, bool pgm = false) = 0;
    virtual void drawImagePart(const uint8_t bitmap[], uint16_t x_part, uint16_t y_part, uint16_t w_bitmap, uint16_t h_bitmap,
                               uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool invert = false, bool mirror_y = false, bool pgm = false) = 0;
    virtual void drawImage(const uint8_t* black, const uint8_t* color, uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool invert = false, bool mirror_y = false, bool pgm = false) = 0;
    virtual void drawImagePart(const uint8_t* black, const uint8_t* color, uint16_t x_part, uint16_t y_part, uint16_t w_bitmap, uint16_t h_bitmap,
                               uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool invert = false, bool mirror_y = false, bool pgm = false) = 0;
    virtual void refresh(bool partial_update_mode = false) = 0; // screen refresh from controller memory to full screen
    virtual void refresh(uint16_t x, uint16_t y, uint16_t w, uint16_t h) = 0; // screen refresh from controller memory, partial screen
    virtual void powerOff() = 0; // turns off generation of panel driving voltages, avoids screen fading over time
    virtual void hibernate() = 0; // turns powerOff() and sets controller to deep sleep for minimum power use, ONLY if wakeable by RST (rst >= 0)
    virtual void setPaged() {}; // for GxEPD2_154c paged workaround
    static inline uint16_t gx_uint16_min(uint16_t a, uint16_t b) {
      return (a < b ? a : b);
    };
    static inline uint16_t gx_uint16_max(uint16_t a, uint16_t b) {
      return (a > b ? a : b);
    };
  protected:
    void _reset();
    void _waitWhileBusy(const char* comment = 0, uint16_t busy_time = 5000);
    void _writeCommand(uint8_t c);
    void _writeData(uint8_t d);
    void _writeData(const uint8_t* data, uint16_t n);
    void _writeDataPGM(const uint8_t* data, uint16_t n, uint16_t fill_with_zeroes = 0);
    void _writeCommandData(const uint8_t* pCommandData, uint8_t datalen);
    void _writeCommandDataPGM(const uint8_t* pCommandData, uint8_t datalen);
    void _beginTransaction(const SPISettings& settings);
    void _transfer(uint8_t data);
    void _endTransaction();
  protected:
    int8_t _sck, _mosi, _cs, _dc, _rst, _busy, _busy_level;
    uint32_t _busy_timeout;
    bool _pulldown_rst_mode;
    SPISettings _spi_settings;
    bool _initial_write, _initial_refresh;
    bool _power_is_on, _using_partial_mode, _hibernating;
  private:
    bool sw_spi;
};

#endif
