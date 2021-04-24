#include "BMP180.h"

#include <Arduino.h>
#include <limits.h>

BMP180::BMP180(uint8_t addr) { _addr = addr; }
BMP180::BMP180() { _addr = 0x77; }
BMP180::~BMP180() {}
bool BMP180::begin(BMP180modes_t mode, uint8_t sda, uint8_t scl) {
  _mode = mode;
  Wire.begin(sda, scl);
  switch (_mode) {
    case BMP180_MODE_ULTRALOWPOWER:
      _delay = 5;
      break;
    case BMP180_MODE_STANDARD:
      _delay = 8;
      break;
    case BMP180_MODE_HIRES:
      _delay = 14;
      break;
    default:
      _delay = 26;
      break;
  }
  for (uint8_t i = 0; i < 8; i++) {
    static uint8_t addr = 0xAA;
    uint16_t ret = 0x0;
    Wire.beginTransmission(0x77);
    Wire.write(addr);
    Wire.endTransmission();
    Wire.requestFrom(0x77, 2);
    ret = Wire.read();
    ret <<= 8;
    ret |= Wire.read();
    switch (addr) {
      case 0xB0:
        _uidata[0] = ret;
        i--;
        break;
      case 0xB2:
        _uidata[1] = ret;
        i--;
        break;
      case 0xB4:
        _uidata[2] = ret;
        i--;
        break;
      default:
        _idata[i] = ret;
        break;
    }
    addr += 2;
  }
  if ((_idata[0] == 0 || _idata[0] == -1) &&
      (_idata[5] == 0 || _idata[5] == -1))
    return false;
  else
    return true;
}

bool BMP180::begin(BMP180modes_t mode) {
  return begin(mode, 4, 5);
}  // default SDA and SCL

uint32_t BMP180::requestMeasurementT() {
  Wire.beginTransmission(_addr);
  Wire.write(0xF4);
  Wire.write(0x2E);
  Wire.endTransmission();
  return 5;
}

uint32_t BMP180::requestMeasurementP() {
  Wire.beginTransmission(_addr);
  Wire.write(0xF4);
  Wire.write(0x34 + (_mode << 6));
  Wire.endTransmission();
  return _delay;
}

bool BMP180::getResultT() {
  Wire.beginTransmission(_addr);
  Wire.write(0xF6);

  if (Wire.endTransmission() != 0) return false;
  Wire.requestFrom(_addr, (uint8_t)2);
  _rtemp = Wire.read();
  _rtemp <<= 8;
  _rtemp |= Wire.read();
  return true;
}

bool BMP180::getResultP() {
  Wire.beginTransmission(_addr);
  Wire.write(0xF6);
  if (Wire.endTransmission() != 0) return false;
  Wire.requestFrom(_addr, (uint8_t)2);
  _rpressure = Wire.read();
  _rpressure <<= 8;
  _rpressure |= Wire.read();

  _rpressure <<= 8;

  Wire.beginTransmission(_addr);
  Wire.write(0xF8);
  Wire.endTransmission();
  Wire.requestFrom(_addr, (uint8_t)1);
  _rpressure |= Wire.read();
  _rpressure >>= (8 - _mode);
  return true;
}

double BMP180::getTempInC() { return ((_getB5(_rtemp) + 8) >> 4) / 10.0; }

int32_t BMP180::getPressure() {
  int32_t p;

  int32_t B5 = _getB5(_rtemp);

  int32_t B6 = B5 - 4000;
  int32_t X1 = ((int32_t)_idata[4] * ((B6 * B6) >> 12)) >> 11;
  int32_t X2 = ((int32_t)_idata[1] * B6) >> 11;
  int32_t X3 = X1 + X2;
  int32_t B3 = ((((int32_t)_idata[0] * 4 + X3) << _mode) + 2) / 4;

  X1 = ((int32_t)_idata[2] * B6) >> 13;
  X2 = ((int32_t)_idata[3] * ((B6 * B6) >> 12)) >> 16;
  X3 = ((X1 + X2) + 2) >> 2;
  uint32_t B4 = ((uint32_t)_uidata[0] * (uint32_t)(X3 + 32768)) >> 15;
  uint32_t B7 = ((uint32_t)_rpressure - B3) * (uint32_t)(50000UL >> _mode);

  if (B7 < (uint32_t)0x80000000) {
    p = (B7 * 2) / B4;
  } else {
    p = (B7 / B4) * 2;
  }
  X1 = (p >> 8) * (p >> 8);
  X1 = (X1 * 3038) >> 16;
  X2 = (-7357 * p) >> 16;
  p = p + ((X1 + X2 + (int32_t)3791) >> 4);

  return p;
}

int32_t BMP180::_getB5(int32_t UT) {
  return (((UT - _uidata[2]) * _uidata[1]) >> 15) +
         (((int32_t)_idata[6] << 11) /
          ((((UT - _uidata[2]) * _uidata[1]) >> 15) + _idata[7]));
}

double BMP180::getTempInCDirect() {
  delay(requestMeasurementT());
  if (!getResultT())
    return -273.15;
  else
    return getTempInC();
}

int32_t BMP180::getPressureDirect() {
  getTempInCDirect();
  delay(requestMeasurementP());
  if (!getResultP())
    return INT_MAX;
  else
    return getPressure();
}