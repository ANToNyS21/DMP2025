#include <EEPROM.h>
#include <Wire.h> // drát
#include <LiquidCrystal_I2C.h>
#include <ADS1X15.h> // ad converter
#define ON 1
#define OFF 0

ADS1115 ADS(0x48);
float f = ADS.toVoltage(1);  //  voltage factor

void setup() {
  Wire.begin();
  ADS.begin();
  ADS.setGain(0);
  Serial.begin(9600);

}

void loop() {
    int16_t valadc1 = ADS.readADC(1);
    int16_t valadc0 = ADS.readADC(2);
    

    
    float displvolt = (valadc0 * f ) * 13.269933; // vypocet vysupniho napeti
    float displcur = (valadc1 * f) / 0.228; // vypocet vysupniho proudu
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println(displvolt);
    Serial.println(displcur);
    delay(200);
    


}
