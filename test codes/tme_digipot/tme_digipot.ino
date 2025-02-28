#include "MCP41HVX1.h"
#include "SPI.h"
#include <EEPROM.h>
#include <Wire.h> // drát
#include <LiquidCrystal_I2C.h>
#include <ADS1X15.h> // ad converter
#define ON 1
#define OFF 0

ADS1115 ADS(0x48);
float f = ADS.toVoltage(1);  //  voltage factor
/////////////////////////////////////////////////////// relay setup



#define PIN_relay5 3
#define PIN_relay6 2


#define PIN_relay1 14
#define PIN_relay2 15
#define PIN_relay3 16
#define PIN_relay4 17

/////////////////////////////////////////////////////// digitpot setup
#define FORWARD true
#define REVERSE false
#define MAX_WIPER_VALUE 255  //maximum wiper value

#define CS_PIN_1    26 //CC 1   //chip select  for SPI communications
#define WLAT_PIN_1  24          // Latch pin
#define SHDN_PIN_1  22          // resistor network enable

#define CS_PIN_2    32 //CC 1 
#define WLAT_PIN_2  30
#define SHDN_PIN_2  28

#define CS_PIN_3    35 //CV 1
#define WLAT_PIN_3  37
#define SHDN_PIN_3  39

#define CS_PIN_4    45 //CV 1
#define WLAT_PIN_4  43
#define SHDN_PIN_4  41

#define CS_PIN_5    23 //CC 2
#define WLAT_PIN_5  25
#define SHDN_PIN_5  27

#define CS_PIN_6    33 //CC 2
#define WLAT_PIN_6  31
#define SHDN_PIN_6  29
 
#define CS_PIN_7    47 //CV 2
#define WLAT_PIN_7  49
#define SHDN_PIN_7  40

#define CS_PIN_8    34 //CV 2
#define WLAT_PIN_8  36
#define SHDN_PIN_8  38


#define set1 230 //CC 1 
#define set2 230 //CC 1 
#define set3 15 //CV 1
#define set4 15 //CV 1
#define set5 230 //CC 2
#define set6 230 //CC 2
#define set7 15 //CV 2
#define set8 15 //CV 2

int modulepower1 = 0;
int modulepower2 = 0;
int serparrelaymode = 0; // 0 off    1 serial    2 paraler
int output = 0; // 0 off     1 out on

//PIN_relay5 = napajni modulu 2 (dole)
//PIN_relay6 = napajni modulu 1 (nahore)
//PIN_relay4 = hlavní OUT vypinac

//PIN_relay2 + PIN_relay3 = paraler
//PIN_relay1 = ser     


MCP41HVX1 Digipot1(CS_PIN_1, SHDN_PIN_1, WLAT_PIN_1);     // Create an instance of the MCP41HVX1 class for controlling the digipot
MCP41HVX1 Digipot2(CS_PIN_2, SHDN_PIN_2, WLAT_PIN_2);
MCP41HVX1 Digipot3(CS_PIN_3, SHDN_PIN_3, WLAT_PIN_3);
MCP41HVX1 Digipot4(CS_PIN_4, SHDN_PIN_4, WLAT_PIN_4);
MCP41HVX1 Digipot5(CS_PIN_5, SHDN_PIN_5, WLAT_PIN_5);
MCP41HVX1 Digipot6(CS_PIN_6, SHDN_PIN_6, WLAT_PIN_6);
MCP41HVX1 Digipot7(CS_PIN_7, SHDN_PIN_7, WLAT_PIN_7);
MCP41HVX1 Digipot8(CS_PIN_8, SHDN_PIN_8, WLAT_PIN_8);
 
void setup ()
{
    delay(10);
    Wire.begin();
    ADS.begin();
    ADS.setGain(0);
    Serial.begin(9600);

    
    pinMode(PIN_relay1, OUTPUT);
    digitalWrite(PIN_relay1, HIGH);
    pinMode(PIN_relay2, OUTPUT);
    digitalWrite(PIN_relay2, HIGH);
    pinMode(PIN_relay3, OUTPUT);
    digitalWrite(PIN_relay3, HIGH);
    pinMode(PIN_relay4, OUTPUT);
    digitalWrite(PIN_relay4, HIGH);

    pinMode(PIN_relay5, OUTPUT);
    digitalWrite(PIN_relay5, HIGH);
    pinMode(PIN_relay6, OUTPUT);
    digitalWrite(PIN_relay6, HIGH);


    pinMode(11, OUTPUT);
    analogWrite(11, 255);

    pinMode(12, OUTPUT);
    digitalWrite(12, HIGH);

    pinMode(13, OUTPUT);
    analogWrite(13, 255);
    delay(1000);

}

void loop ()
{   
    if (output) {
    digitalWrite(PIN_relay4, LOW);
    }else {
    digitalWrite(PIN_relay4, HIGH);
    }

    if (modulepower1) {
    digitalWrite(PIN_relay6, LOW);
    }else {
    digitalWrite(PIN_relay6, HIGH);
    }

    if (modulepower2) {
    digitalWrite(PIN_relay5, LOW);
    }else {
    digitalWrite(PIN_relay5, HIGH);
    }

    switch (serparrelaymode) {
   case 0:
      digitalWrite(PIN_relay1, HIGH);
      digitalWrite(PIN_relay2, HIGH);
      digitalWrite(PIN_relay3, HIGH);
        break;
   case 1:
      digitalWrite(PIN_relay1, LOW);
      digitalWrite(PIN_relay2, HIGH);
      digitalWrite(PIN_relay3, HIGH);
        break;
   case 2:
      digitalWrite(PIN_relay1, HIGH);
      digitalWrite(PIN_relay2, LOW);
      digitalWrite(PIN_relay3, LOW);
        break;
  }


    int16_t valadc1 = ADS.readADC_Differential_0_1();
    int16_t valadc0 = ADS.readADC_Differential_0_2();
    

    
    float displvolt = (valadc0 * f ) * 13.1 ;//* 13.269933; // vypocet vysupniho napeti
    float displcur = (valadc1 * f) * 4.6376811594 ; // vypocet vysupniho proudu //0.228
    


    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println();

    Serial.print("napeti ");
    Serial.print(displvolt);
    Serial.println();
    Serial.print("proud  ");
    Serial.print(displcur);

    Serial.println();

    Digipot1.WiperSetPosition(set1);
    Serial.print( "Digipot1 = ");
    Serial.println( Digipot1.WiperGetPosition());   
    

    Digipot2.WiperSetPosition(set2);
    Serial.print("Digipot2 = ");
    Serial.println(Digipot2.WiperGetPosition());
    

    Digipot3.WiperSetPosition(set3);
    Serial.print("Digipot3 = ");
    Serial.println(Digipot3.WiperGetPosition());
    

    Digipot4.WiperSetPosition(set4);
    Serial.print("Digipot4 = ");
    Serial.println(Digipot4.WiperGetPosition());
    

    Digipot5.WiperSetPosition(set5);
    Serial.print("Digipot5 = ");
    Serial.println(Digipot5.WiperGetPosition());
    

    Digipot6.WiperSetPosition(set6);
    Serial.print("Digipot6 = ");
    Serial.println(Digipot6.WiperGetPosition());
    

    Digipot7.WiperSetPosition(set7);
    Serial.print("Digipot7 = ");
    Serial.println(Digipot7.WiperGetPosition());
    

    Digipot8.WiperSetPosition(set8);
    Serial.print("Digipot8 = ");
    Serial.println(Digipot8.WiperGetPosition());
    delay(300);

    
    
    
    
     
    
}
