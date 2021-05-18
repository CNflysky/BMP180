#ifndef _BMP180_H_
#define _BMP180_H_
#include <Arduino.h>
#include <Wire.h>

typedef enum {
  BMP180_MODE_ULTRALOWPOWER,
  BMP180_MODE_STANDARD,
  BMP180_MODE_HIRES,
  BMP180_MODE_ULTRAHIRES
} BMP180modes_t;


class BMP180 {
 public:
  BMP180(uint8_t addr);
  BMP180();
  ~BMP180();
  void changeMode(BMP180modes_t mode);
  uint32_t requestMeasurementT();
  uint32_t requestMeasurementP();
  bool getResultT();
  bool getResultP();
  double getTempInCDirect();
  double getTempInC();
  int32_t getPressureDirect();
  int32_t getPressure();
  bool begin(BMP180modes_t mode);
  bool begin(BMP180modes_t mode,uint8_t sda,uint8_t scl);
  private:
  void _setDelay(BMP180modes_t mode);
  int32_t _rpressure;
  int32_t _rtemp;
  int32_t _getB5(int32_t UT);
  int16_t _idata[8];
  uint16_t _uidata[3];
  uint8_t _addr;
  uint8_t _delay;
  BMP180modes_t _mode;
};

#endif