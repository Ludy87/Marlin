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

#ifndef _GxEPD2_GFX_H_
#define _GxEPD2_GFX_H_

#include <Adafruit_GFX.h>
#include "GxEPD2_EPD.h"

class GxEPD2_GFX : public Adafruit_GFX
{
  public:
    GxEPD2_GFX(GxEPD2_EPD& _epd2, uint16_t w, uint16_t h) : Adafruit_GFX(w, h), epd2(_epd2) {};
    virtual uint16_t pages() = 0;
    virtual uint16_t pageHeight() = 0;
    virtual bool mirror(bool m) = 0;
    virtual void init() = 0;
    // init method with additional parameters:
    // initial false for re-init after processor deep sleep wake up, if display power supply was kept
    // this can be used to avoid the repeated initial full refresh on displays with fast partial update
    // NOTE: garbage will result on fast partial update displays, if initial full update is omitted after power loss
    // pulldown_rst_mode true for alternate RST handling to avoid feeding 5V through RST pin
    virtual void init(bool initial, bool pulldown_rst_mode = false) = 0;
    virtual void fillScreen(uint16_t color) = 0; // 0x0 black, >0x0 white, to buffer
    // display buffer content to screen, useful for full screen buffer
    virtual void display(bool partial_update_mode = false) = 0;
    // display part of buffer content to screen, useful for full screen buffer
    // displayWindow, use parameters according to actual rotation.
    // x and w should be multiple of 8, for rotation 0 or 2,
    // y and h should be multiple of 8, for rotation 1 or 3,
    // else window is increased as needed,
    // this is an addressing limitation of the e-paper controllers
    virtual void displayWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) = 0;
    virtual void setFullWindow() = 0;
    // setPartialWindow, use parameters according to actual rotation.
    // x and w should be multiple of 8, for rotation 0 or 2,
    // y and h should be multiple of 8, for rotation 1 or 3,
    // else window is increased as needed,
    // this is an addressing limitation of the e-paper controllers
    virtual void setPartialWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) = 0;
    virtual void firstPage() = 0;
    virtual bool nextPage() = 0;
    virtual void drawPaged(void (*drawCallback)(const void*), const void* pv) = 0;
    virtual void drawInvertedBitmap(uint16_t x, uint16_t y, const uint8_t bitmap[], uint16_t w, uint16_t h, uint16_t color) = 0;
    //  Support for Bitmaps (Sprites) to Controller Buffer and to Screen
    virtual void clearScreen(uint8_t value = 0xFF) = 0; // init controller memory and screen (default white)
    virtual void writeScreenBuffer(uint8_t value = 0xFF) = 0; // init controller memory (default white)
    // write to controller memory, without screen refresh; x and w should be multiple of 8
    virtual void writeImage(const uint8_t bitmap[], uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool invert = false, bool mirror_y = false, bool pgm = false) = 0;
    virtual void writeImagePart(const uint8_t bitmap[], uint16_t x_part, uint16_t y_part, uint16_t w_bitmap, uint16_t h_bitmap,
                                uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool invert = false, bool mirror_y = false, bool pgm = false) = 0;
    virtual void writeImage(const uint8_t* black, const uint8_t* color, uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool invert, bool mirror_y, bool pgm) = 0;
    virtual void writeImage(const uint8_t* black, const uint8_t* color, uint16_t x, uint16_t y, uint16_t w, uint16_t h) = 0; // default options false
    virtual void writeImagePart(const uint8_t* black, const uint8_t* color, uint16_t x_part, uint16_t y_part, uint16_t w_bitmap, uint16_t h_bitmap,
                                uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool invert, bool mirror_y, bool pgm) = 0;
    virtual void writeImagePart(const uint8_t* black, const uint8_t* color, uint16_t x_part, uint16_t y_part, uint16_t w_bitmap, uint16_t h_bitmap,
                                uint16_t x, uint16_t y, uint16_t w, uint16_t h) = 0; // default options false
    // write to controller memory, with screen refresh; x and w should be multiple of 8
    virtual void drawImage(const uint8_t bitmap[], uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool invert = false, bool mirror_y = false, bool pgm = false) = 0;
    virtual void drawImagePart(const uint8_t bitmap[], uint16_t x_part, uint16_t y_part, uint16_t w_bitmap, uint16_t h_bitmap,
                                uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool invert = false, bool mirror_y = false, bool pgm = false) = 0;
    virtual void drawImage(const uint8_t* black, const uint8_t* color, uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool invert, bool mirror_y, bool pgm) = 0;
    virtual void drawImage(const uint8_t* black, const uint8_t* color, uint16_t x, uint16_t y, uint16_t w, uint16_t h) = 0; // default options false
    virtual void drawImagePart(const uint8_t* black, const uint8_t* color, uint16_t x_part, uint16_t y_part, uint16_t w_bitmap, uint16_t h_bitmap,
                                uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool invert, bool mirror_y, bool pgm) = 0;
    virtual void drawImagePart(const uint8_t* black, const uint8_t* color, uint16_t x_part, uint16_t y_part, uint16_t w_bitmap, uint16_t h_bitmap,
                                uint16_t x, uint16_t y, uint16_t w, uint16_t h) = 0; // default options false
    virtual void refresh(bool partial_update_mode = false) = 0; // screen refresh from controller memory to full screen
    virtual void refresh(uint16_t x, uint16_t y, uint16_t w, uint16_t h) = 0; // screen refresh from controller memory, partial screen
    virtual void powerOff() = 0; // turns off generation of panel driving voltages, avoids screen fading over time
    virtual void hibernate() = 0; // turns powerOff() and sets controller to deep sleep for minimum power use, ONLY if wakeable by RST (rst >= 0)
  public:
    GxEPD2_EPD& epd2;
};

#endif
