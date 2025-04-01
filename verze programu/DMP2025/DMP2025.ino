#include "MCP41HVX1.h"
#include "SPI.h"
#include <EEPROM.h>
#include <Wire.h> // drát
#include <LiquidCrystal_I2C.h>
#include <ADS1X15.h> // ad converter
#define ON 1
#define OFF 0


#define programversion 1.00
#define rate_of_refreshing 350 // refreshrate hlavni obrazovky
#define DEBUG ON //    ON / OFF    1/0     debugovaci odesilani hodnot do seriove linky

////////////////////////////////////// relay piny
//PIN_relay5 = napajni modulu 2 (dole)
//PIN_relay6 = napajni modulu 1 (nahore)
//PIN_relay4 = hlavní OUT vypinac

//PIN_relay2 + PIN_relay3 = paraler
//PIN_relay1 = ser  
#define PIN_relay5 3
#define PIN_relay6 2


#define PIN_relay1 14
#define PIN_relay2 15
#define PIN_relay3 16
#define PIN_relay4 17
////////////////////////////////////// H mustek fan control
#define fan2en 11
#define fan1en 13
#define fanEn 12
////////////////////////////////////// piny digitálních potenciometrů
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


MCP41HVX1 Digipot1(CS_PIN_1, SHDN_PIN_1, WLAT_PIN_1);     // Create an instance of the MCP41HVX1 class for controlling the digipot
MCP41HVX1 Digipot2(CS_PIN_2, SHDN_PIN_2, WLAT_PIN_2);
MCP41HVX1 Digipot3(CS_PIN_3, SHDN_PIN_3, WLAT_PIN_3);
MCP41HVX1 Digipot4(CS_PIN_4, SHDN_PIN_4, WLAT_PIN_4);
MCP41HVX1 Digipot5(CS_PIN_5, SHDN_PIN_5, WLAT_PIN_5);
MCP41HVX1 Digipot6(CS_PIN_6, SHDN_PIN_6, WLAT_PIN_6);
MCP41HVX1 Digipot7(CS_PIN_7, SHDN_PIN_7, WLAT_PIN_7);
MCP41HVX1 Digipot8(CS_PIN_8, SHDN_PIN_8, WLAT_PIN_8);

////////////////////////////////////// enkoder 1
#define PIN_DT  7 // napětí // 7 8 9
#define PIN_CLK 8
#define PIN_BTN1 9
int lastStateDT;
int currentStateDT;
int stepCounter = 0;



unsigned long lastDebounceTime = 0; // pamet zpoždění enkoderu // enkoder napeti hlavni obrazovka
const unsigned long debounceDelay = 5; // zpoždění enkoderu

unsigned long lastDebounceTime2 = 0; // pamet zpoždění enkoderu // enkoder u zadavani napeti
const unsigned long debounceDelay2 = 5; // zpoždění enkoderu

unsigned long lastDebounceTime3 = 0; // pamet zpoždění enkoderu // tlacitko u zadavani napeti
const unsigned long debounceDelay3 = 150; // zpoždění enkoderu

////////////////////////////////////// enkoder 2
#define PIN_DT2  4 // proud
#define PIN_CLK2 5
#define PIN_BTN2 6

int lastStateDT2;
int currentStateDT2;
int stepCounter2 = 0;


#define CCCVmodedifftreshold 0.2
static float proud = 8;
static float napeti = 1;
//static int napeti2 = 1;

LiquidCrystal_I2C lcd(0x27, 20, 4); //display

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
static int refreshrate = rate_of_refreshing ;



void setup() {
  delay(10);
  /////////////////////////////////////////// relays
  //module1    
  pinMode(PIN_relay1, OUTPUT);
  digitalWrite(PIN_relay1, HIGH);
  pinMode(PIN_relay2, OUTPUT);
  digitalWrite(PIN_relay2, HIGH);
  pinMode(PIN_relay3, OUTPUT);
  digitalWrite(PIN_relay3, HIGH);
  pinMode(PIN_relay4, OUTPUT);
  digitalWrite(PIN_relay4, HIGH);
  //module2  
  pinMode(PIN_relay5, OUTPUT);
  digitalWrite(PIN_relay5, HIGH);
  pinMode(PIN_relay6, OUTPUT);
  digitalWrite(PIN_relay6, HIGH);

  //proud = EEPROM.read(1)-1;
  /////////////////////////////////////////// fan control
  pinMode(fan2en, OUTPUT);
  analogWrite(fan2en, 255);

  pinMode(fanEn, OUTPUT);
  digitalWrite(fanEn, HIGH);

  pinMode(fan1en, OUTPUT);
  analogWrite(fan1en, 255);
  /////////////////////////////////////////// enkoder 1
  pinMode(PIN_DT, INPUT_PULLUP);
  pinMode(PIN_CLK, INPUT_PULLUP);
  pinMode(PIN_BTN1, INPUT_PULLUP);
  lastStateDT = digitalRead(PIN_DT);
  
  /////////////////////////////////////////// enkoder 2
  pinMode(PIN_DT2, INPUT_PULLUP);
  pinMode(PIN_CLK2, INPUT_PULLUP);
  pinMode(PIN_BTN2, INPUT_PULLUP);
  lastStateDT2 = digitalRead(PIN_DT2);
  /////////////////////////////////////////// ads
  Wire.begin();
  ADS.begin();
  ADS.setGain(0);
  
  /////////////////////////////////////////// lcd
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
    /*
    currentStateDT = digitalRead(PIN_DT);
    if (currentStateDT != lastStateDT && (millis() - lastDebounceTime) > debounceDelay) {
    lastDebounceTime = millis();  // reset  časovače
    stepCounter++;  

    // pouziti kazdeho druheho kroku
    if (stepCounter % 2 == 0) {
      if (digitalRead(PIN_CLK) != currentStateDT) {// určení směru
        proud--;  // proti směru hodinových ručiček
      } else {
        proud++;  // ve směru hodinových ručiček
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
    

    
    

    }
  }
  
    lastStateDT = currentStateDT; // ulozeni do pameti
    */

  if (tik) {
    
    
    //nacteni ads hodnot
    int16_t valadc1 = ADS.readADC_Differential_0_1();
    int16_t valadc0 = ADS.readADC_Differential_0_2();
    float displvolt = (valadc0 * f ) * 13.1 ;//* 13.269933; // vypocet vysupniho napeti
    float displcur = (valadc1 * f) * 4.6376811594 ; // vypocet vysupniho proudu //0.228
    if (displcur < 0) {
    displcur = 0;
    }

    if (displvolt < 0) {
    displvolt = 0;
    }

    float displpwr = displvolt * displcur;// P=U*I
    if (displpwr < 0) {
    displpwr = 0;
    }

    lcd.setCursor(0, 0);
    lcd.print("U:");
    if (displvolt > 10) {
    lcd.setCursor(2, 0);
    }else {
    lcd.setCursor(2, 0);
    lcd.print(" ");
    lcd.setCursor(3, 0);
    }
    lcd.print(displvolt , 3); 
    lcd.setCursor(8, 0);
    lcd.print("V");


    lcd.setCursor(0, 1);
    lcd.print("I:");

    lcd.setCursor(3, 1);    
    lcd.print(displcur , 3); 
    lcd.setCursor(8, 1);
    lcd.print("A");

    lcd.setCursor(0, 2);
    lcd.print("P:");
    if (displpwr > 10) {
    lcd.setCursor(2, 2);
    }else {
    lcd.setCursor(2, 2);
    lcd.print(" ");
    lcd.setCursor(3, 2);
    }
    lcd.print(displpwr , 3);
    lcd.setCursor(8, 2);
    lcd.print("W");

    lcd.setCursor(10, 0);
    lcd.print("Us:");
     if (napeti > 10) {
    lcd.setCursor(13, 0);
    }else {
    lcd.setCursor(13, 0);
    lcd.print(" ");
    lcd.setCursor(14, 0);
    }
    lcd.print(napeti , 3);
    lcd.setCursor(19, 0);
    lcd.print("V");

    lcd.setCursor(10, 1);
    lcd.print("Is:");
    lcd.setCursor(14, 1);
    lcd.print(proud , 3);
    lcd.setCursor(19, 1);
    lcd.print("A");

    lcd.setCursor(0, 3);
    lcd.print("OUT: ON");



    lcd.setCursor(11, 3);
    
    if (abs(napeti - displvolt) > CCCVmodedifftreshold) {
    lcd.print("MODE: CC");
    }else {
    lcd.print("MODE: CV");
    }

  }
    


  int stav_tlacitka = digitalRead(PIN_BTN1);
    static int stav_tlac_predchozi = HIGH;

    
    if (stav_tlacitka == LOW && stav_tlac_predchozi == HIGH && (millis() - lastDebounceTime3) > debounceDelay3) {
      zadani_napeti();
      lastDebounceTime3 = millis();
    }
    stav_tlac_predchozi = stav_tlacitka;

  
}








void zadani_napeti() {
    
  lcd.clear();
  int select = 1;
  lastStateDT2 = digitalRead(PIN_DT);
  lastDebounceTime3 = millis();
  while (true) {
    tikani(refreshrate);
  


      currentStateDT2 = digitalRead(PIN_DT);
  if (currentStateDT2 != lastStateDT2 && (millis() - lastDebounceTime2) > debounceDelay2) {
    lastDebounceTime2 = millis();  // reset  časovače
    stepCounter2++;  

    // pouziti kazdeho druheho kroku
    if (stepCounter2 % 2 == 0) {
      if (digitalRead(PIN_CLK) != currentStateDT2) {// určení směru
        
         switch(select){ // proti směru hodinových ručiček
          case 1:
             napeti = napeti - 10;
             break;

          case 2:
             napeti = napeti - 1;
              break;

          case 3:
             napeti = napeti - 0.1;
              break;

          case 4:
             napeti = napeti - 0.01;
              break;

          case 5:
             napeti = napeti - 0.001;
              break;

      }
        if (DEBUG) {
          Serial.print(napeti,3);
          Serial.println("--");
        }
        
        

      } else {

        switch(select){ // proti směru hodinových ručiček
          case 1:
             napeti = napeti + 10;
             break;

          case 2:
             napeti = napeti + 1;
              break;

          case 3:
             napeti = napeti + 0.1;
              break;

          case 4:
             napeti = napeti + 0.01;
              break;

          case 5:
             napeti = napeti + 0.001;
              break;

      }
        if (DEBUG) {
          Serial.print(napeti,3);
          Serial.println("++");
        }
        
      }

        if (napeti > 60) {
          napeti = 60;
       }
         if (napeti < 0) {
          napeti = 0;
        }

      lcd.setCursor(0, 0);
      lcd.print("NAPETI : ");
      if (napeti > 9.999) {
      lcd.setCursor(9, 0);
      }else {
      lcd.setCursor(10, 0);
      }
      lcd.print(napeti); 
      
    

    }
  }

  lastStateDT2 = currentStateDT2; // ulozeni do pameti

    if (tik) {
      
      char formated_napeti[10];  // prevod float na string pro display interkace
      dtostrf(napeti, 6, 3, formated_napeti);  // 6 zanku s 3 desetinymi misty
      if (formated_napeti[0] == ' ') { // detekce mezery na prvnim miste stringu
        formated_napeti[0] = '0';// osetreni aby se cislo 2 netislo 2.000 pak by nesedel ukazatel zvoleneho mista na upravu
      } 

      lcd.setCursor(0, 0);
      lcd.print("NAPETI : ");
      lcd.setCursor(9, 0);
      lcd.print(formated_napeti); 
      
      lcd.setCursor(15, 0);
      lcd.print("V");

    
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
    


  }
    
    
    

    
    int stlav_tlacitka = digitalRead(PIN_BTN1);
    int stav_tlac_predchozi;

    if (stlav_tlacitka == LOW && stav_tlac_predchozi == HIGH && (millis() - lastDebounceTime3) > debounceDelay3){
      select = select + 1;
      lastDebounceTime3 = millis();
    }
    stav_tlac_predchozi = stlav_tlacitka;




    

    if (select >= 6) {
      goto exit_point;  // Přejdeme na label exit_point a ukončíme funkci
    }
    
    
  }
  exit_point:
  lcd.clear();

}









































