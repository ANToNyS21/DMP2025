//YWROBOT
//Compatible with the Arduino IDE 1.0
//Library version:1.1
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup()
{
  lcd.init();                      // initialize the lcd 
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("U:12.346V Us:12.485V");
  lcd.setCursor(0,1);
  lcd.print("I: 2.546A Is: 1.451A");
  lcd.setCursor(0,2);
  lcd.print("P:31.269W  ");
  lcd.setCursor(0,3);
  lcd.print("OUT: ON    MODE: CC ");
  
}


void loop()
{ /*
Napětí: XX.XX V    Proud: X.XXX A
Výkon: XX.XX W     Režim: CC/CV
------------------------------
Zadané V: XX.XX    Zadané I: X.XXX*/
}
