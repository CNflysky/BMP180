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
  Serial.print("Temp:");
  Serial.print(bmp180.getTempInCDirect());
  Serial.println(" \'C");
  Serial.println("Prssure:");
  // you can use this function to get pressure data directly without measure
  // temp.
  Serial.print(bmp180.getPressureDirect());
  Serial.println(" Pa");
  delay(1000);
}