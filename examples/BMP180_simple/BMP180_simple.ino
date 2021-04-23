#include <BMP180.h>
BMP180 bmp180(0x77);
void setup(){
    Serial.begin(9600);
    Serial.println();
    bmp180.begin(BMP180_MODE_ULTRAHIRES);
}

void loop(){
    Serial.print("Temp:");
    Serial.print(bmp180.getTempInCDirect());
    Serial.println(" \'C");
    Serial.println("Prssure:");
    Serial.print(bmp180.getPressureDirect());
    Serial.println(" Pa");
    delay(1000);
}