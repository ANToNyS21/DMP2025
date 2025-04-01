//  knihovny
#include <FastX9CXXX.h>
#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "ADS1X15.h"


#define programversion 1.00
////////////////////////////////////// piny digitálních potenciometrů
#define X9_CS1  11
#define X9_INC1 12
#define X9_UD1  13

#define X9_CS2  8
#define X9_INC2 9
#define X9_UD2 10
////////////////////////////////////// piny enkoderu
#define PIN_DT  23 // napětí
#define PIN_CLK 22
#define PIN_BTN1 24

#define PIN_DT2  25 // proud
#define PIN_CLK2 24



FastX9C103 x91;
FastX9C103 x92;
static int proud = 98;
static float napeti = 1;
static int napeti2 = 1;

LiquidCrystal_I2C lcd(0x27, 20, 4); //display
bool halt = true;
byte sipka[8] = { // vytvoreni znaku pro vlastniho display
  B00100, 
  B01110, 
  B11111, 
  B00100, 
  B00100, 
  B00100, 
  B11100, 
  B00000  
}; 




ADS1115 ADS(0x48);
float f = ADS.toVoltage(1);  //  voltage factor


int tik = LOW;
static int refreshrate = 600;



void setup() {
  proud = EEPROM.read(1)-1;
  ///////////////////////////////////////////enkoder 1
  pinMode(PIN_DT, INPUT);
  pinMode(PIN_CLK, INPUT);
  pinMode(PIN_BTN1, INPUT_PULLUP);
 
  Wire.begin();
  ADS.begin();
  ADS.setGain(0);
  

  lcd.createChar(1, sipka);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Laboratorni Zdroj");
  lcd.setCursor(3, 1);
  lcd.print("Smola Antonin");
  lcd.setCursor(12, 3);
  lcd.print("SW:v");
  lcd.setCursor(16, 3);
  lcd.print(programversion);
  // zahájení komunikace po sériové lince
  Serial.begin(9600);
  // nastavení pinů modulu digipot
  x91.Setup(X9_CS1, X9_UD1, X9_INC1);
  x92.Setup(X9_CS2, X9_UD2, X9_INC2);
  delay(2000);
  lcd.clear();
}

void tikani(int T) {
  unsigned long cas = millis();
  static unsigned long cas_predchozi = 0;
  if (cas - cas_predchozi >= T) {
    tik = HIGH;
    cas_predchozi = cas;
  } else tik = LOW;
}




void loop() {
    
    tikani(refreshrate);
    static int _DT;
    int DT = digitalRead(PIN_DT);
    int CLK = digitalRead(PIN_CLK);
     // detekce zmeny na pinu DT (nabezna nebo sestupna hrana)
  if (DT != _DT) {
    
    // detekce nabezne hrany
    if (DT) {
      
      // detekce smeru
      if (CLK) proud++;
      //else pozice++;
      

    // detekce sestupne hrany
    } else {
      
      // detekce smeru
      if (CLK) proud--;
      //else pozice--;
      

    }
    if (proud > 99) {
    proud = 99;
    }
    if (proud < 1) {
    proud = 1;
    }
    lcd.setCursor(0, 0);
    lcd.print("pozice :              ");
    lcd.setCursor(9, 0);
    lcd.print(proud);
    EEPROM.put(1, proud);
    

    x91.JumpToStep(proud);
    x92.JumpToStep(proud);
    
    


  }
    _DT = DT;
    

  if (tik) {
    lcd.setCursor(0, 1);
    int16_t val_0 = ADS.readADC(0);
    lcd.print("napeti A0 : ");
    lcd.setCursor(12, 1);
    lcd.print((val_0 * f) / 1.172, 4);
  }
    


  int stav_tlacitka = digitalRead(PIN_BTN1);
    static int stav_tlac_predchozi = HIGH;

    // Změníme select při stisku tlačítka
    if (stav_tlacitka == LOW && stav_tlac_predchozi == HIGH) {
      zadani_napeti();
    }
    stav_tlac_predchozi = stav_tlacitka;

  
}








void zadani_napeti() {
    
  lcd.clear();
  int select = 1;

  while (true) {
  static int _DT;
  int DT = digitalRead(PIN_DT);
  int CLK = digitalRead(PIN_CLK);
  // detekce zmeny na pinu DT (nabezna nebo sestupna hrana)
  if (DT != _DT) {
    
    // detekce nabezne hrany
    if (DT) {
      
      // detekce smeru
      if (CLK) napeti++;
      //else napeti--;
      
      

    // detekce sestupne hrany
    } else {
      
      // detekce smeru
      if (CLK) napeti--;
      //else napeti++;
      

    }
    
    char formated_napeti[10];  // prevod float na string pro display interkace
    dtostrf(napeti, 6, 3, formated_napeti);  // 6 zanku s 3 desetinymi misty
      if (formated_napeti[0] == ' ') { // detekce mezery na prvnim miste stringu
        formated_napeti[0] = '0';// osetreni aby se cislo 2 netislo 2.000 pak by nesedel ukazatel zvoleneho mista na upravu
      } 

    lcd.setCursor(0, 0);
    lcd.print("napeti : ");
    lcd.setCursor(9, 0);
    lcd.print(formated_napeti);
   
    
    
    delay(10);
    
  }
    lcd.setCursor(0, 1);
    
    switch(select){
      case 1:
        lcd.print("_________");
        lcd.setCursor(9, 1);
        break;

      case 2:
        lcd.print("__________");
        lcd.setCursor(10, 1);
        break;

      case 3:
        lcd.print("____________");
        lcd.setCursor(12, 1);
        break;

      case 4:
        lcd.print("_____________");
        lcd.setCursor(13, 1);
        break;

      case 5:
        lcd.print("______________");
        lcd.setCursor(14, 1);
        break;

    }
    lcd.write(1);
    lcd.print("                   ");
    
    _DT = DT;

    
    int stlav_tlacitka = digitalRead(PIN_BTN1);
    int stav_tlac_predchozi;

    if (stlav_tlacitka == LOW && stav_tlac_predchozi == HIGH){
      select = select + 1;
    }
    stav_tlac_predchozi = stlav_tlacitka;

    if (select >= 6) {
      goto exit_point;  // Přejdeme na label exit_point a ukončíme funkci
    }
    
    
  }
  exit_point:
  lcd.clear();

}









































