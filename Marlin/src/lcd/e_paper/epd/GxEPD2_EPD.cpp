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

#include "GxEPD2_EPD.h"
#include <SPI.h>

#if defined(ESP8266) || defined(ESP32)
  #include <pgmspace.h>
#else
  #include <avr/pgmspace.h>
#endif

GxEPD2_EPD::GxEPD2_EPD(int8_t cs, int8_t dc, int8_t rst, int8_t busy, int8_t busy_level, uint32_t busy_timeout,
                       uint16_t w, uint16_t h, GxEPD2::Panel p, bool c, bool pu, bool fpu) :
  WIDTH(w), HEIGHT(h), panel(p), hasColor(c), hasPartialUpdate(pu), hasFastPartialUpdate(fpu),
  _sck(-1), _mosi(-1), _cs(cs), _dc(dc), _rst(rst),
  _busy(busy), _busy_level(busy_level), _busy_timeout(busy_timeout),
  _spi_settings(4000000, MSBFIRST, SPI_MODE0) {
  _initial_write = true;
  _initial_refresh = true;
  _power_is_on = false;
  _using_partial_mode = false;
  _hibernating = false;
  sw_spi = false;
}

void GxEPD2_EPD::init() { init(true, false); }

void GxEPD2_EPD::init(bool initial, bool pulldown_rst_mode) {
  _initial_write = initial;
  _initial_refresh = initial;
  _pulldown_rst_mode = pulldown_rst_mode;
  _power_is_on = false;
  _using_partial_mode = false;
  _hibernating = false;
  if (_cs >= 0) {
    digitalWrite(_cs, HIGH);
    pinMode(_cs, OUTPUT);
  }
  if (_dc >= 0) {
    digitalWrite(_dc, HIGH);
    pinMode(_dc, OUTPUT);
  }
  _reset();
  if (_busy >= 0)
    pinMode(_busy, INPUT);
  if (sw_spi == false)
    SPI.begin();
}

void GxEPD2_EPD::init(int8_t sck, int8_t mosi, bool initial, bool pulldown_rst_mode) {
  sw_spi = true;
  if ((sck >= 0) && (mosi >= 0)) {
    _sck = sck;
    _mosi = mosi;
    digitalWrite(_sck, LOW);
    digitalWrite(_mosi, LOW);
    pinMode(_sck, OUTPUT);
    pinMode(_mosi, OUTPUT);
  } else _sck = -1;
  init(initial, pulldown_rst_mode);
}

void GxEPD2_EPD::_reset() {
  if (_rst >= 0) {
    if (_pulldown_rst_mode) {
      digitalWrite(_rst, LOW);
      pinMode(_rst, OUTPUT);
      delay(20);
      pinMode(_rst, INPUT_PULLUP);
      delay(200);
    }
    else {
      digitalWrite(_rst, HIGH);
      pinMode(_rst, OUTPUT);
      delay(20);
      digitalWrite(_rst, LOW);
      delay(20);
      digitalWrite(_rst, HIGH);
      delay(200);
    }
    _hibernating = false;
  }
}

void GxEPD2_EPD::_waitWhileBusy(const char* comment, uint16_t busy_time) {
  if (_busy >= 0) {
    delay(1); // add some margin to become active
    unsigned long start = micros();
    while (1) {
      if (digitalRead(_busy) != _busy_level) break;
      delay(1);
      if (micros() - start > _busy_timeout) break;
    }
    (void) start;
  }
  else delay(busy_time);
}

void GxEPD2_EPD::_writeCommand(uint8_t c) {
  _beginTransaction(_spi_settings);
  if (_dc >= 0) digitalWrite(_dc, LOW);
  if (_cs >= 0) digitalWrite(_cs, LOW);
  _transfer(c);
  if (_cs >= 0) digitalWrite(_cs, HIGH);
  if (_dc >= 0) digitalWrite(_dc, HIGH);
  _endTransaction();
}

void GxEPD2_EPD::_writeData(uint8_t d) {
  _beginTransaction(_spi_settings);
  if (_cs >= 0) digitalWrite(_cs, LOW);
  _transfer(d);
  if (_cs >= 0) digitalWrite(_cs, HIGH);
  _endTransaction();
}

void GxEPD2_EPD::_writeData(const uint8_t* data, uint16_t n) {
  _beginTransaction(_spi_settings);
  if (_cs >= 0) digitalWrite(_cs, LOW);
  for (uint16_t i = 0; i < n; i++) {
    _transfer(*data++);
  }
  if (_cs >= 0) digitalWrite(_cs, HIGH);
  _endTransaction();
}

void GxEPD2_EPD::_writeDataPGM(const uint8_t* data, uint16_t n, uint16_t fill_with_zeroes) {
  _beginTransaction(_spi_settings);
  if (_cs >= 0) digitalWrite(_cs, LOW);
  for (uint16_t i = 0; i < n; i++) {
    _transfer(pgm_read_byte(&*data++));
  }
  while (fill_with_zeroes > 0) {
    _transfer(0x00);
    fill_with_zeroes--;
  }
  if (_cs >= 0) digitalWrite(_cs, HIGH);
  _endTransaction();
}

void GxEPD2_EPD::_writeCommandData(const uint8_t* pCommandData, uint8_t datalen) {
  _beginTransaction(_spi_settings);
  if (_dc >= 0) digitalWrite(_dc, LOW);
  if (_cs >= 0) digitalWrite(_cs, LOW);
  _transfer(*pCommandData++);
  if (_dc >= 0) digitalWrite(_dc, HIGH);
  for (uint8_t i = 0; i < datalen - 1; i++) {  // sub the command
    _transfer(*pCommandData++);
  }
  if (_cs >= 0) digitalWrite(_cs, HIGH);
  _endTransaction();
}

void GxEPD2_EPD::_writeCommandDataPGM(const uint8_t* pCommandData, uint8_t datalen) {
  _beginTransaction(_spi_settings);
  if (_dc >= 0) digitalWrite(_dc, LOW);
  if (_cs >= 0) digitalWrite(_cs, LOW);
  _transfer(pgm_read_byte(&*pCommandData++));
  if (_dc >= 0) digitalWrite(_dc, HIGH);
  for (uint8_t i = 0; i < datalen - 1; i++) {  // sub the command
    _transfer(pgm_read_byte(&*pCommandData++));
  }
  if (_cs >= 0) digitalWrite(_cs, HIGH);
  _endTransaction();
}

void GxEPD2_EPD::_beginTransaction(const SPISettings& settings) {
  if (sw_spi == false) SPI.beginTransaction(settings);
}

void GxEPD2_EPD::_transfer(uint8_t data) {
  if (sw_spi == false) SPI.transfer(data);
  else {
    for (int i = 0; i < 8; i++) {
      digitalWrite(_mosi, (data & 0x80) ? HIGH : LOW);
      data <<= 1;
      digitalWrite(_sck, HIGH);
      digitalWrite(_sck, LOW);
    }
  }
}

void GxEPD2_EPD::_endTransaction() {
  if (sw_spi == false) SPI.endTransaction();
}
