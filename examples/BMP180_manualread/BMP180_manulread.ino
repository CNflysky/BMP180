#include <BMP180.h>
BMP180 bmp180(0x77);
void setup() {
  Serial.begin(9600);
  Serial.println();
  if(!bmp180.begin(BMP180_MODE_ULTRAHIRES)){
    Serial.println("Sensor init failed.Please check your wiring");
    while(1); //dead
  }
}

void loop() {
  long int1 = bmp180.requestMeasurementT();
  delay(int1);
  /* after measurement operation completed,you must call the corresponding
     getResult() function,otherwise the data will be incorrect.
     that means you should call the getResultT() function to get temp in this
     case.
  */
  if (!bmp180.getResultT()) {
    Serial.println("Measure Temp Failed!");
  } else {
    Serial.print("Temp:");
    Serial.print(bmp180.getTempInC());
    Serial.println(" C");
  }
  // be advised:In order to measure pressure,you must measure temperature first!
  long int2 = bmp180.requestMeasurementP();
  delay(int2);
  if (!bmp180.getResultP()) {
    Serial.println("Measure Pressure Failed!");
  } else {
    Serial.print("Pressure:");
    Serial.print(bmp180.getPressure());
    Serial.println(" Pa");
  }
  delay(1000);
}