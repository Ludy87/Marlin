// Display Library for SPI e-paper panels from Dalian Good Display and boards from Waveshare.
// Requires HW SPI and Adafruit_GFX. Caution: these e-papers require 3.3V supply AND data lines!
//
// based on Demo Example from Good Display: http://www.e-paper-display.com/download_list/downloadcategoryid=34&isMode=false.html
//
// Author: Jean-Marc Zingg
//
// Version: see library.properties
//
// Library: https://github.com/ZinggJM/GxEPD2

#include "GxEPD2_EPD.h"

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
  if (_sck < 0)
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
  if (_sck < 0) SPI.beginTransaction(settings);
}

void GxEPD2_EPD::_transfer(uint8_t data) {
  if (_sck < 0) SPI.transfer(data);
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
  if (_sck < 0) SPI.endTransaction();
}
