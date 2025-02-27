#include "MCP41HVX1.h"
#include "SPI.h"
#include <EEPROM.h>
#include <Wire.h> // drát
#include <LiquidCrystal_I2C.h> 
#include <ADS1X15.h> // AD converter 
#define ON 1
#define OFF 0
int prvnispusteni = 1;


#define programversion 1.00
#define rate_of_refreshing 350 // refreshrate hlavni obrazovky
#define DEBUG ON //    ON / OFF    1/0     debugovaci odesilani hodnot do seriove linky

////////////////////////////////////// relay piny
//PIN_relay5 = napajni modulu 2 (dole)
//PIN_relay6 = napajni modulu 1 (nahore)
//PIN_relay4 = hlavní OUT vypinac

//PIN_relay2 + PIN_relay3 = paraler
//PIN_relay1 = ser  
#define PIN_relay5 3 //napajni modulu 2
#define PIN_relay6 2 //napajni modulu 1


#define PIN_relay1 14
#define PIN_relay2 15
#define PIN_relay3 16
#define PIN_relay4 17

int modulepower1 = 0;
int modulepower2 = 0;
int serparrelaymode = 0; // 0 off    1 serial    2 paraler
int output = 0; // 0 off     1 out on

////////////////////////////////////// H mustek fan control
#define fan2en 11 // inside
#define fan1en 13 // exhaust
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


int set1 = 150; //CC 1 
int set2 = 150; //CC 1 
int set3 = 200; //CV 1
int set4 = 200; //CV 1
int set5 = 150; //CC 2
int set6 = 150; //CC 2
int set7 = 200; //CV 2
int set8 = 200; //CV 2


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
static float proud = 0.5;
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
static int refreshrate = rate_of_refreshing;


/////////////////////////////////////////// vypocty kroku

float resistanceLookup_U[256];
constexpr float MAX_VOLTAGE = 32.0;     // max U
constexpr float RESISTOR_U_VALUE = 100000.0; // Hodnota odporu jednoho potenciometru
constexpr float TARGET_U_RESISTANCE = 50000.0; // Paralelní kombinace dvou potenciometrů
constexpr float TOLERANCE_U = 0.01;         // TOLERANCE_U pro shodu napětí
float DESIRED_U = 1.00;

float resistanceLookup_I[256];
constexpr float MAX_CURRENT = 4.00;     // max I
constexpr float RESISTOR_I_VALUE = 5000; // Hodnota odporu jednoho potenciometru
constexpr float TARGET_I_RESISTANCE = 2500.0; // Paralelní kombinace dvou potenciometrů
constexpr float TOLERANCE_I = 0.01;         // TOLERANCE_I pro shodu proudu
float DESIRED_I = 1.00;



/////////////////////////////////////////// vypocty korekce

///////////////// korekce vystupniho napětí
const int tableSize = 32; // korekce vystupniho napětí
float requestedVoltages[tableSize] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32};
float realVoltages[tableSize] = {1.46, 2.39, 3.32, 4.2, 5.1, 6.1, 7, 8, 8.8, 9.8, 10.7, 11.6, 12.6, 13.5, 14.4, 15.3, 16.3, 17.2, 18.1, 19.09, 20, 20.9, 21.8, 22.7, 23.7, 24.6, 25.5, 26.4, 27.4, 28.3, 29.2, 30.1};


//////////// korekce mereneho proudu
const int AtableSize = 40; // korekce mereneho proudu
float zdroj[AtableSize] = {0.2, 0.588, 0.8, 0.98, 1.18, 1.37, 1.57, 1.76, 1.94, 2.14, 2.34, 2.54, 2.73, 2.92, 3.11, 3.31, 3.49, 3.68, 3.86, 4.04, 4.23, 4.43, 4.63, 4.81, 4.99, 5.16, 5.36, 5.56, 5.72, 5.91, 6.09, 6.29, 6.49, 6.69, 6.89, 7.09, 7.32, 7.55, 7.76, 7.967};
float ampermetr[AtableSize] = {0.1, 0.5, 0.9, 1.18, 1.48, 1.77, 2.06, 2.36, 2.63, 2.93, 3.24, 3.53, 3.83, 4.13, 4.4, 4.71, 5.01, 5.31, 5.59, 5.86, 6.15, 6.47, 6.79, 7.1, 7.36, 7.63, 7.95, 8.32, 8.58, 8.89, 9.21, 9.55, 9.95, 10.29, 10.65, 10.95, 11.39, 11.75, 12.21, 12.57};

//float zdroj[AtableSize] = {0.45, 0.588, 0.8, 0.98, 1.18, 1.37, 1.57, 1.76, 1.94, 2.14, 2.34, 2.54, 2.73, 2.92, 3.11, 3.31, 3.49, 3.68, 3.86, 4.04, 4.23, 4.43, 4.63, 4.81, 4.99, 5.16, 5.36, 5.56, 5.72, 5.91, 6.09, 6.29, 6.49, 6.69, 6.89, 7.09, 7.32, 7.55, 7.76, 7.967};
//float ampermetr[AtableSize] = {0.18, 0.5, 0.9, 1.18, 1.48, 1.77, 2.06, 2.36, 2.63, 2.93, 3.24, 3.53, 3.83, 4.13, 4.4, 4.71, 5.01, 5.31, 5.59, 5.86, 6.15, 6.47, 6.79, 7.1, 7.36, 7.63, 7.95, 8.32, 8.58, 8.89, 9.21, 9.55, 9.95, 10.29, 10.65, 10.95, 11.39, 11.75, 12.21, 12.57};

///////////// korekce vystupniho proudu
const int A_Out_tableSize = 41; // korekce vystupniho proudu
float setCurrent[A_Out_tableSize] = {0, 0.2, 0.4, 0.6, 0.8, 1, 1.2, 1.4, 1.6, 1.8, 2, 2.2, 2.4, 2.6, 2.8, 3, 3.2, 3.4, 3.6, 3.8, 4, 4.2, 4.4, 4.6, 4.8, 5, 5.2, 5.4, 5.6, 5.8, 6, 6.2, 6.4, 6.6, 6.8, 7, 7.2, 7.4, 7.6, 7.8, 8};
float realCurrent[A_Out_tableSize] = {0, 0.38, 0.588, 0.8, 0.98, 1.18, 1.2, 1.4, 1.6, 1.8, 2, 2.2, 2.4, 2.6, 2.8, 3, 3.2, 3.4, 3.6, 3.8, 4, 4.2, 4.4, 4.6, 4.8, 5, 5.2, 5.4, 5.6, 5.8, 6, 6.2, 6.4, 6.6, 6.8, 7, 7.2, 7.4, 7.6, 7.8, 8};



const int A_AtableSize = 40; // korekce vystupniho proudu V2
float A_ampermetr[A_AtableSize] = {0, 0.2, 0.4, 0.6, 0.8, 1.0, 1.2, 1.4, 1.6, 1.8, 2.0, 2.2, 2.4, 2.6, 2.8, 3.0, 3.2, 3.4, 3.6, 3.8, 4.0, 4.2, 4.4, 4.6, 4.8, 5.0, 5.2, 5.4, 5.6, 5.8, 6.0, 6.2, 6.4, 6.6, 6.8, 7.0, 7.2, 7.4, 7.6, 7.8};
float A_zdroj[A_AtableSize] = {0, 0.38, 0.58, 0.79, 0.98, 1.17, 1.37, 1.57, 1.76, 1.94, 2.14, 2.33, 2.53, 2.74, 2.92, 3.12, 3.31, 3.50, 3.69, 3.86, 4.05, 4.24, 4.43, 4.63, 4.81, 4.99, 5.19, 5.38, 5.56, 5.73, 5.91, 6.09, 6.26, 6.50, 6.68, 6.88, 7.08, 7.29, 7.51, 7.70};

const int C_tableSize = 15; 
float A_setCurrent[C_tableSize] =      {0, 0.1, 0.2 , 0.3, 0.4, 0.5,  0.6, 0.7,  0.8,  0.9,  1,    1.1,  1.2, 1.3,  1.4};
float A_measuredCurrent[C_tableSize] = {0, 0.2, 0.25, 0.3, 0.4, 0.45, 0.5, 0.55, 0.62, 0.73, 0.82, 0.93, 1  , 1.12, 1.21};


/////////////////////////////////////////// SETUP ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  delay(10);
  /////////////////////////////////////////// relays
  // relay module1    
  pinMode(PIN_relay1, OUTPUT);
  digitalWrite(PIN_relay1, HIGH);
  pinMode(PIN_relay2, OUTPUT);
  digitalWrite(PIN_relay2, HIGH);
  pinMode(PIN_relay3, OUTPUT);
  digitalWrite(PIN_relay3, HIGH);
  pinMode(PIN_relay4, OUTPUT);
  digitalWrite(PIN_relay4, HIGH);
  // relay module2  
  pinMode(PIN_relay5, OUTPUT);
  digitalWrite(PIN_relay5, HIGH);
  pinMode(PIN_relay6, OUTPUT);
  digitalWrite(PIN_relay6, HIGH);

  /////////////////////////////////////////// EEPROM read
  //EEPROM.put(1, proud);
  //EEPROM.put(6, napeti);

  EEPROM.get(1, proud);
  EEPROM.get(6, napeti);

  //EEPROM.put(1, proud);

  /////////////////////////////////////////// fan control setup

  pinMode(fan2en, OUTPUT);
  analogWrite(fan2en, 255);

  

  pinMode(fan1en, OUTPUT);
  analogWrite(fan1en, 255);


  pinMode(fanEn, OUTPUT);
  digitalWrite(fanEn, HIGH);

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
  /////////////////////////////////////////// adc
  Wire.begin();
  ADS.begin();
  ADS.setGain(0);
  /////////////////////////////////////////// vypocty
  initializeResistanceLookup_U();
  initializeResistanceLookup_I();
  
  /////////////////////////////////////////// lcd

  
  lcd.createChar(2, sipka);
  

  delay(10);
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
  if (DEBUG) {
    Serial.println();
    Serial.println("DEBUG ENABLED");
  }

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
    int16_t valadc1 = ADS.readADC(1); // proud
    //valadc1 = valadc1 - 116;
    int16_t valadc0 = ADS.readADC_Differential_0_2(); // napeti
    //Serial.println(valadc1);
    //Serial.println(valadc0);
    
    float displvolt = (valadc0 * f ) * 13.1 ;//* 13.269933; // vypocet vysupniho napeti
    float displcur = (valadc1 * f) * 7.1428571429 ;  // because 1 / 0.19 ≈ 5.56 7,1428571429
    //float displcur = (valadc1 * f) * 4.2016806723;
    //float displcur = (valadc1 * f) * 4.6376811594 ; // vypocet vysupniho proudu //0.228
    //Serial.println(displcur);
    //Serial.println(displvolt);
    //Serial.println();
    
    if (displcur < 0) {
      displcur = 0;
    }
    if (DEBUG) {
      Serial.println(displcur);
    }
    displcur = apply_Measured_Current_Correction(displcur);
    if (DEBUG) {
      Serial.println(displcur);
      Serial.println();
    }
    

    if (displcur < 0) {
      displcur = 0;
    }

    if (displvolt < 0) {
      displvolt = 0;
    }

    if (output == 0) {
      displcur = 0;
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
    
    if (output == 0) {
      lcd.print("OUT: OFF");
      analogWrite(fan1en, 0);
      analogWrite(fan2en, 0);
    }else {
      lcd.print("OUT: ON");
      analogWrite(fan1en, 255);
      analogWrite(fan2en, 255);
    }



    lcd.setCursor(11, 3);
    if (output == 0) {
      lcd.print("MODE:N/A");
    }else {
      if (abs(napeti - displvolt) > CCCVmodedifftreshold) {
      lcd.print("MODE: CC");
      }else {
        lcd.print("MODE: CV");
      }

    
    }

    Digipot1.WiperSetPosition(set1);
    
    Digipot2.WiperSetPosition(set2);
    
    Digipot3.WiperSetPosition(set3);
    
    Digipot4.WiperSetPosition(set4);
    
    Digipot5.WiperSetPosition(set5);
    
    Digipot6.WiperSetPosition(set6);
    
    Digipot7.WiperSetPosition(set7);
    
    Digipot8.WiperSetPosition(set8);

     if(DEBUG) {
        Serial.println();
        Serial.print("DCP1 walue =");
        Serial.print(Digipot1.WiperGetPosition());
        

        Serial.println();
        Serial.print("DCP2 walue =");
        Serial.print(Digipot2.WiperGetPosition());
        

        Serial.println();
        Serial.print("DCP3 walue =");
        Serial.print(Digipot3.WiperGetPosition());
        

        Serial.println();
        Serial.print("DCP4 walue =");
        Serial.print(Digipot4.WiperGetPosition());
        

        Serial.println();
        Serial.print("DCP5 walue =");
        Serial.print(Digipot5.WiperGetPosition());
        

        Serial.println();
        Serial.print("DCP6 walue =");
        Serial.print(Digipot6.WiperGetPosition());
        

        Serial.println();
        Serial.print("DCP7 walue =");
        Serial.print(Digipot7.WiperGetPosition());
        

        Serial.println();
        Serial.print("DCP8 walue =");
        Serial.print(Digipot8.WiperGetPosition());
        Serial.println();
        Serial.println();
        Serial.println();
        
        
    }
    
    

  }
    


  int stav_tlacitka = digitalRead(PIN_BTN1);
    static int stav_tlac_predchozi = HIGH;

    
    if (stav_tlacitka == LOW && stav_tlac_predchozi == HIGH && (millis() - lastDebounceTime3) > debounceDelay3) {
      zadani_napeti();
      lastDebounceTime3 = millis();
    }
    stav_tlac_predchozi = stav_tlacitka;



  int stav_tlacitka_2 = digitalRead(PIN_BTN2);
    static int stav_tlac_predchozi_2 = HIGH;

    
    if (stav_tlacitka_2 == LOW && stav_tlac_predchozi_2 == HIGH && (millis() - lastDebounceTime3) > debounceDelay3) {
      zadani_proudu();
      lastDebounceTime3 = millis();
    }
    stav_tlac_predchozi_2 = stav_tlacitka_2;

  
}








void zadani_napeti() {
  disable_output();
  power_DOWN_module1();
  power_DOWN_module2();
  analogWrite(fan1en, 0);
  if (prvnispusteni == 0) {
    analogWrite(fan2en, 255);
  }
    
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

        if (napeti > 32) {
          napeti = 32;
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
      
  
  
      lcd.print("|");
      //lcd.print(char(2));
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
      power_DOWN_module1();
      power_DOWN_module2();
      lcd.clear();
      lcd.setCursor(5, 1);
      lcd.print("Loading...");
      lcd.setCursor(5, 2);
      lcd.print("Please wait");

      EEPROM.put(6, napeti);

      DESIRED_U = applyVoltageCorrection(napeti);
      DESIRED_I = get_Corrected_Output(proud / 2);

        if(proud < 1.4){
        float input = proud; // Změň hodnotu pro testování
        float corrected = C_apply_Measured_Current_Correction(input);
        float diference = corrected - input;
        DESIRED_I = DESIRED_I + diference;
        if(proud > 0.499 && proud < 0.599){
          DESIRED_I = DESIRED_I - 0.05;
        }else if (proud >= 0.6 && proud < 1.399) {
          DESIRED_I = DESIRED_I - 0.1;
        }
      }else if (proud >= 1.4 && proud <= 5) {
        DESIRED_I = DESIRED_I + 0.1;
      }else if (proud > 5) {
        DESIRED_I = DESIRED_I + 0.15;
        }
        
  
      
      
      
      //DESIRED_I = proud / 2;

      //DESIRED_I = getCorrectedSetValue(proud / 2);
      
      
      /*if (proud > 1.2) {
        DESIRED_I = DESIRED_I + 0.2;
      }*/
      
      vcalc();
      if (prvnispusteni) {
        icalc();
      }
      DCPsetup();
      prvnispusteni = 0;
      goto exit_point;  // Přejdeme na label exit_point a ukončíme funkci
    }
    
    
  }
  exit_point:
  lcd.clear();

}

void zadani_proudu() {
  disable_output();
  power_DOWN_module1();
  power_DOWN_module2();
  analogWrite(fan1en, 0);
  if (prvnispusteni == 0) {
    analogWrite(fan2en, 255);
  }
  
    
  lcd.clear();
  int select = 1;
  lastStateDT2 = digitalRead(PIN_DT2);
  lastDebounceTime3 = millis();
  while (true) {
    tikani(refreshrate);
  


      currentStateDT2 = digitalRead(PIN_DT2);
  if (currentStateDT2 != lastStateDT2 && (millis() - lastDebounceTime2) > debounceDelay2) {
    lastDebounceTime2 = millis();  // reset  časovače
    stepCounter2++;  

    // pouziti kazdeho druheho kroku
    if (stepCounter2 % 2 == 0) {
      if (digitalRead(PIN_CLK2) != currentStateDT2) {// určení směru
        
         switch(select){ // proti směru hodinových ručiček
          case 1:
             proud = proud - 10;
             break;

          case 2:
             proud = proud - 1;
              break;

          case 3:
             proud = proud - 0.1;
              break;

          case 4:
             proud = proud - 0.01;
              break;

          case 5:
             proud = proud - 0.001;
              break;

      }
        if (DEBUG) {
          Serial.print(proud,3);
          Serial.println("--");
        }
        
        

      } else {

        switch(select){ // proti směru hodinových ručiček
          case 1:
             proud = proud + 10;
             break;

          case 2:
             proud = proud + 1;
              break;

          case 3:
             proud = proud + 0.1;
              break;

          case 4:
             proud = proud + 0.01;
              break;

          case 5:
             proud = proud + 0.001;
              break;

      }
        if (DEBUG) {
          Serial.print(proud,3);
          Serial.println("++");
        }
        
      }

        if (proud > 8) {
          proud = 8;
       }
         if (proud < 0) {
          proud = 0;
        }

      lcd.setCursor(0, 0);
      lcd.print("PROUD : ");
      if (proud > 9.999) {
      lcd.setCursor(9, 0);
      }else {
      lcd.setCursor(10, 0);
      }
      lcd.print(proud); 
      
    

    }
  }

  lastStateDT2 = currentStateDT2; // ulozeni do pameti

    if (tik) {
      
      char formated_proud[10];  // prevod float na string pro display interkace
      dtostrf(proud, 6, 3, formated_proud);  // 6 zanku s 3 desetinymi misty
      if (formated_proud[0] == ' ') { // detekce mezery na prvnim miste stringu
        formated_proud[0] = '0';// osetreni aby se cislo 2 netislo 2.000 pak by nesedel ukazatel zvoleneho mista na upravu
      } 

      lcd.setCursor(0, 0);
      lcd.print("PROUD : ");
      lcd.setCursor(9, 0);
      lcd.print(formated_proud); 
      
      lcd.setCursor(15, 0);
      lcd.print("A");

    
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
       
  
  
      lcd.print("|");
      //lcd.print(char(2));
      lcd.print("                   ");
    


  }
    
    
    

    
    int stlav_tlacitka = digitalRead(PIN_BTN2);
    int stav_tlac_predchozi;

    if (stlav_tlacitka == LOW && stav_tlac_predchozi == HIGH && (millis() - lastDebounceTime3) > debounceDelay3){
      select = select + 1;
      lastDebounceTime3 = millis();
    }
    stav_tlac_predchozi = stlav_tlacitka;




    

    if (select >= 6) {
      power_DOWN_module1();
      power_DOWN_module2();
      lcd.clear();
      lcd.setCursor(5, 1);
      lcd.print("Loading...");
      lcd.setCursor(5, 2);
      lcd.print("Please wait");

      EEPROM.put(1, proud);

      DESIRED_U = applyVoltageCorrection(napeti);
      //DESIRED_I = getCorrectedSetValue(proud / 2);
      DESIRED_I =get_Corrected_Output(proud / 2);


        if(proud < 1.4){
        float input = proud; // Změň hodnotu pro testování
        float corrected = C_apply_Measured_Current_Correction(input);
        float diference = corrected - input;
        DESIRED_I = DESIRED_I + diference;
        if(proud > 0.499 && proud < 0.599){
          DESIRED_I = DESIRED_I - 0.05;
        }else if (proud >= 0.6 && proud < 1.399) {
          DESIRED_I = DESIRED_I - 0.1;
        }
      }else if (proud >= 1.4 && proud <= 5) {
        DESIRED_I = DESIRED_I + 0.1;
      }else if (proud > 5) {
        DESIRED_I = DESIRED_I + 0.15;
        }
      
      
     
     
    
      
      icalc();

      if (prvnispusteni) {
        vcalc();
      }
      DCPsetup();
      prvnispusteni = 0;
      goto exit_point2;  // Přejdeme na label exit_point a ukončíme funkci
    }
    
    
  }
  exit_point2:
  lcd.clear();

}






int vcalc(){
  if (DEBUG) {
          Serial.println("Starting voltage optimization...");
        }
  


  float desiredVoltage = DESIRED_U; // Cílové napětí (možno změnit dle potřeby)

    int closestX = 0, closestY = 0;
    float closestVoltage = 0.0;
    float minDifference = MAX_VOLTAGE; // Inicializace s maximálním rozdílem

    unsigned long startTime = millis();

    // Optimalizované vyhledávání
    for (int X = 0; X < 256; ++X) {
        float R1 = resistanceLookup_U[X];
        for (int Y = 0; Y <= X; ++Y) { // Smyčka běží jen pro Y <= X díky symetrii
            float R2 = resistanceLookup_U[Y];
            float voltage = calculateVoltage(R1, R2);
            float difference = abs(voltage - desiredVoltage);

            // Pokud je rozdíl menší než TOLERANCE_U, nalezli jsme shodu
            if (difference < TOLERANCE_U) {
              set3 = X;
              set4 = Y;
              if (DEBUG) {
                  Serial.print("Exact match found: X = ");
                  Serial.print(X);
                  Serial.print(", Y = ");
                  Serial.print(Y);
                  Serial.print(", Voltage = ");
                  Serial.println(voltage);
                  Serial.print("Time elapsed: ");
                  Serial.print(millis() - startTime);
                  Serial.println(" ms");
                  Serial.print(set3);
                  Serial.print(set4);
                  }
                
                
                
                return; // Ukončíme smyčku loop
            }

            // Aktualizace nejbližší hodnoty
            if (difference < minDifference) {
                minDifference = difference;
                closestVoltage = voltage;
                closestX = X;
                closestY = Y;
            }
        }
    }

    // Pokud nebyla nalezena přesná shoda
    set3 = closestX;
    set4 = closestY;
    if (DEBUG) {
          Serial.println("No exact match found.");
          Serial.print("Closest match: X = ");
          Serial.print(closestX);
          Serial.print(", Y = ");
          Serial.print(closestY);
          Serial.print(", Voltage = ");
          Serial.println(closestVoltage);
          Serial.print("Time elapsed: ");
          Serial.print(millis() - startTime);
          Serial.println(" ms");
          Serial.print(set3);
          Serial.print(set4);
        }
    

    



}

int icalc(){
  if (DEBUG) {
          Serial.println("Starting current optimization...");
        }


  float desiredCurrent = DESIRED_I; // Cílové napětí (možno změnit dle potřeby)

    int closestX = 0, closestY = 0;
    float closestCurrent = 0.0;
    float minDifference = MAX_CURRENT; // Inicializace s maximálním rozdílem

    unsigned long startTime = millis();

    // Optimalizované vyhledávání
    for (int X = 0; X < 256; ++X) {
        float R1 = resistanceLookup_I[X];
        for (int Y = 0; Y <= X; ++Y) { // Smyčka běží jen pro Y <= X díky symetrii
            float R2 = resistanceLookup_I[Y];
            float current = calculateCurrent(R1, R2);
            float difference = abs(current - desiredCurrent);

            // Pokud je rozdíl menší než TOLERANCE_I, nalezli jsme shodu
            if (difference < TOLERANCE_I) {
                set1 = X;
                set2 = Y;
                if (DEBUG) {
                    Serial.print("Exact match found: X = ");
                    Serial.print(X);
                    Serial.print(", Y = ");
                    Serial.print(Y);
                    Serial.print(", Current = ");
                    Serial.println(current);
                    Serial.print("Time elapsed: ");
                    Serial.print(millis() - startTime);
                    Serial.println(" ms");
                    Serial.print(set1);
                    Serial.print(set2);
                  }
                
                return; // Ukončíme smyčku loop
            }

            // Aktualizace nejbližší hodnoty
            if (difference < minDifference) {
                minDifference = difference;
                closestCurrent = current;
                closestX = X;
                closestY = Y;
            }
        }
    }

    // Pokud nebyla nalezena přesná shoda
    set1 = closestX;
    set2 = closestY;
    if (DEBUG) {
          Serial.println("No exact match found.");
          Serial.print("Closest match: X = ");
          Serial.print(closestX);
          Serial.print(", Y = ");
          Serial.print(closestY);
          Serial.print(", current = ");
          Serial.println(closestCurrent);
          Serial.print("Time elapsed: ");
          Serial.print(millis() - startTime);
          Serial.println(" ms");
          Serial.print(set1);
          Serial.print(set2);
        }
    



}








void initializeResistanceLookup_U() {
    for (int i = 0; i < 256; ++i) {
        resistanceLookup_U[i] = (RESISTOR_U_VALUE / 256) * i;
    }
}

inline float calculateVoltage(float R1, float R2) {
    float R_parallel = (R1 * R2) / (R1 + R2);
    return (R_parallel / TARGET_U_RESISTANCE) * MAX_VOLTAGE;
}



void initializeResistanceLookup_I() {
    for (int i = 0; i < 256; ++i) {
        resistanceLookup_I[i] = (RESISTOR_I_VALUE / 256) * i;
    }
}

inline float calculateCurrent(float R1, float R2) {
    float R_parallel = (R1 * R2) / (R1 + R2);
    return (R_parallel / TARGET_I_RESISTANCE) * MAX_CURRENT;
}


int DCPsetup(){
    setvaliditycheck();
    set5 = set1; // settovani slave DCP podle mastru
    set6 = set2;
    set7 = set3;
    set8 = set4;
    if (DEBUG) {

      Serial.println();
      Serial.print("set1 =");
      Serial.print(set1);
      Serial.println();

      Serial.print("set2 =");
      Serial.print(set2);
      Serial.println();
      
      Serial.print("set3 =");
      Serial.print(set3);
      Serial.println();

      Serial.print("set4 =");
      Serial.print(set4);
      Serial.println();

      Serial.print("set5 =");
      Serial.print(set5);
      Serial.println();

      Serial.print("set6 =");
      Serial.print(set6);
      Serial.println();

      Serial.print("set7 =");
      Serial.print(set7);
      Serial.println();

      Serial.print("set8 =");
      Serial.print(set8);
      Serial.println();
    }
    DCPwiperset();
    delay(600);
    DCPfeedback();
    delay(150);
    power_UP_module1();
    power_UP_module2();
    delay(300);
    serila_conn();
    delay(100);
    enable_output();

}


int setvaliditycheck(){

    if (set1 < 1) {
      set1 = 1;
    }

    if (set1 > 255) {
    set1 = 255;
    }



    if (set2 < 1) {
      set2 = 1;
    }

    if (set2 > 255) {
    set2 = 255;
    }



    if (set3 < 1) {
      set3 = 1;
    }

    if (set3 > 255) {
    set3 = 255;
    }



    if (set4 < 1) {
      set4 = 1;
    }

    if (set4 > 255) {
    set4 = 255;
    }



    if (set5 < 1) {
      set5 = 1;
    }

    if (set5 > 255) {
    set5 = 255;
    }



    if (set6 < 1) {
      set6 = 1;
    }

    if (set6 > 255) {
    set6 = 255;
    }



    if (set7 < 1) {
      set7 = 1;
    }

    if (set7 > 255) {
    set7 = 255;
    }



    if (set8 < 1) {
      set8 = 1;
    }

    if (set8 > 255) {
    set8 = 255;
    }


}

int DCPwiperset(){
    delay(100);
    Digipot1.WiperSetPosition(set1);
    delay(100);
    Digipot2.WiperSetPosition(set2);
    delay(100);
    Digipot3.WiperSetPosition(set3);
    delay(100);
    Digipot4.WiperSetPosition(set4);
    delay(100);
    Digipot5.WiperSetPosition(set5);
    delay(100);
    Digipot6.WiperSetPosition(set6);
    delay(100);
    Digipot7.WiperSetPosition(set7);
    delay(100);
    Digipot8.WiperSetPosition(set8);
    delay(100);

    
}


void DCPfeedback(){
      if(DEBUG) {
        Serial.println();
        Serial.print("DCP1 walue =");
        Serial.print(Digipot1.WiperGetPosition());
        

        Serial.println();
        Serial.print("DCP2 walue =");
        Serial.print(Digipot2.WiperGetPosition());
        

        Serial.println();
        Serial.print("DCP3 walue =");
        Serial.print(Digipot3.WiperGetPosition());
        

        Serial.println();
        Serial.print("DCP4 walue =");
        Serial.print(Digipot4.WiperGetPosition());
        

        Serial.println();
        Serial.print("DCP5 walue =");
        Serial.print(Digipot5.WiperGetPosition());
        

        Serial.println();
        Serial.print("DCP6 walue =");
        Serial.print(Digipot6.WiperGetPosition());
        

        Serial.println();
        Serial.print("DCP7 walue =");
        Serial.print(Digipot7.WiperGetPosition());
        

        Serial.println();
        Serial.print("DCP8 walue =");
        Serial.print(Digipot8.WiperGetPosition());
        
        
    }
    Digipot1.WiperSetPosition(set1);
    if(Digipot1.WiperGetPosition() != set1) {
        Serial.println();
        Serial.print("ERR at 1 == ");
        Serial.print(Digipot1.WiperGetPosition());
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("ERR: 1DCP NOT FOUND");
        while(1) {
          delay(1000);
        }
        DCPerror();
    }


    Digipot2.WiperSetPosition(set2);
    if(Digipot2.WiperGetPosition() != set2) {
        Serial.println();
        Serial.print("ERR at 2 == ");
        Serial.print(Digipot2.WiperGetPosition());
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("ERR: 2DCP NOT FOUND");
        while(1) {
          delay(1000);
        }
        DCPerror();
    }

    Digipot3.WiperSetPosition(set3);
    if(Digipot3.WiperGetPosition() != set3) {
        Serial.println();
        Serial.print("ERR at 3 == ");
        Serial.print(Digipot3.WiperGetPosition());
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("ERR: 3DCP NOT FOUND");
        while(1) {
          delay(1000);
        }
        DCPerror();
    }
    Digipot4.WiperSetPosition(set4);

    if(Digipot4.WiperGetPosition() != set4) {
        Serial.println();
        Serial.print("ERR at 4 == ");
        Serial.print(Digipot4.WiperGetPosition());
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("ERR: 4DCP NOT FOUND");
        while(1) {
          delay(1000);
        }
        DCPerror();
    }
    Digipot5.WiperSetPosition(set5);

    if(Digipot5.WiperGetPosition() != set5) {
        Serial.println();
        Serial.print("ERR at 5 == ");
        Serial.print(Digipot5.WiperGetPosition());
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("ERR: 5DCP NOT FOUND");
        while(1) {
          delay(1000);
        }
        DCPerror();
    }
    Digipot6.WiperSetPosition(set6);

    if(Digipot6.WiperGetPosition() != set6) {
        Serial.println();
        Serial.print("ERR at 6 == ");
        Serial.print(Digipot6.WiperGetPosition());
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("ERR: 6DCP NOT FOUND");
        while(1) {
          delay(1000);
        }
        DCPerror();
    }
    Digipot7.WiperSetPosition(set7);

    if(Digipot7.WiperGetPosition() != set7) {
        Serial.println();
        Serial.print("ERR at 7 == ");
        Serial.print(Digipot7.WiperGetPosition());
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("ERR: 7DCP NOT FOUND");
        while(1) {
          delay(1000);
        }
        DCPerror();
    }

    if(Digipot8.WiperGetPosition() != set8) {
        Serial.println();
        Serial.print("ERR at 8 == ");
        Serial.print(Digipot8.WiperGetPosition());
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("ERR: 8DCP NOT FOUND");
        while(1) {
          delay(1000);
        }
        DCPerror();
    }
    Digipot8.WiperSetPosition(set8);


    

}

void DCPerror(){
    
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("ERR: DCP NOT FOUND");
    while(1) {
    delay(1000);
    }
}

void power_UP_module1(){

  digitalWrite(PIN_relay6, LOW); //napajni modulu 1

}

void power_UP_module2(){
  
  digitalWrite(PIN_relay5, LOW); //napajni modulu 2
  
}


void power_DOWN_module1(){

  digitalWrite(PIN_relay6, HIGH); //napajni modulu 1

}

void power_DOWN_module2(){

  digitalWrite(PIN_relay5, HIGH); //napajni modulu 2
  
}

void disable_output(){

  digitalWrite(PIN_relay4, HIGH); //ovladani vystupu zdroje
  digitalWrite(PIN_relay3, HIGH);
  digitalWrite(PIN_relay2, HIGH);
  digitalWrite(PIN_relay1, HIGH);
  output = 0;
  
}

void enable_output(){

  digitalWrite(PIN_relay4, LOW); //ovladani vystupu zdroje
  output = 1;
  
}

//PIN_relay2 + PIN_relay3 = paraler
//PIN_relay1 = ser  

void serila_conn(){
  digitalWrite(PIN_relay2, LOW);
  digitalWrite(PIN_relay3, LOW);
}



/////////////////////////////////////////////////////////////////////////////////////////////// out voltage corection ///////////////////////////////////////////////////////////////////////////////////////////////


float applyVoltageCorrection(float voltage) {
    float correctionFactor = getCorrectionFactor(voltage);
    return voltage * correctionFactor;
}

float getCorrectionFactor(float voltage) {
    if (voltage <= requestedVoltages[0]) return requestedVoltages[0] / realVoltages[0];
    if (voltage >= requestedVoltages[tableSize - 1]) return requestedVoltages[tableSize - 1] / realVoltages[tableSize - 1];
    
    for (int i = 0; i < tableSize - 1; i++) {
        if (voltage >= requestedVoltages[i] && voltage <= requestedVoltages[i + 1]) {
            float factor1 = requestedVoltages[i] / realVoltages[i];
            float factor2 = requestedVoltages[i + 1] / realVoltages[i + 1];
            float t = (voltage - requestedVoltages[i]) / (requestedVoltages[i + 1] - requestedVoltages[i]);
            return factor1 + t * (factor2 - factor1);
        }
    }
    return 1.0; // no correction
}

/////////////////////////////////////////////////////////////////////////////////////////////// measured cur corection ///////////////////////////////////////////////////////////////////////////////////////////////

float get_Measured_Current_Correction_Factor(float current) {
    if (current <= ampermetr[0]) return zdroj[0] / ampermetr[0];
    if (current >= ampermetr[AtableSize - 1]) return zdroj[AtableSize - 1] / ampermetr[AtableSize - 1];

    for (int i = 0; i < AtableSize - 1; i++) {
        if (current >= ampermetr[i] && current <= ampermetr[i + 1]) {
            float factor1 = zdroj[i] / ampermetr[i];
            float factor2 = zdroj[i + 1] / ampermetr[i + 1];
            float t = (current - ampermetr[i]) / (ampermetr[i + 1] - ampermetr[i]);
            return factor1 + t * (factor2 - factor1);
        }
    }
    return 1.0; // no correction
}

float apply_Measured_Current_Correction(float current) {
    float correctionFactor = get_Measured_Current_Correction_Factor(current);
    return current * correctionFactor;
}

/////////////////////////////////////////////////////////////////////////////////////////////// out cur corection ///////////////////////////////////////////////////////////////////////////////////////////////

float getCorrectedSetValue(float targetCurrent) {
    if (targetCurrent <= realCurrent[0]) return setCurrent[0];
    if (targetCurrent >= realCurrent[A_Out_tableSize - 1]) return setCurrent[A_Out_tableSize - 1];

    for (int i = 0; i < A_Out_tableSize - 1; i++) {
        if (targetCurrent >= realCurrent[i] && targetCurrent <= realCurrent[i + 1]) {
            float t = (targetCurrent - realCurrent[i]) / (realCurrent[i + 1] - realCurrent[i]);
            return setCurrent[i] + t * (setCurrent[i + 1] - setCurrent[i]);
        }
    }
    return targetCurrent; // no correction
}






float get_Corrected_Output(float input) {
    if (input <= A_zdroj[0]) return A_ampermetr[0];
    if (input >= A_zdroj[A_AtableSize - 1]) return A_ampermetr[A_AtableSize - 1];

    for (int i = 0; i < A_AtableSize - 1; i++) {
        if (input >= A_zdroj[i] && input <= A_zdroj[i + 1]) {
            float t = (input - A_zdroj[i]) / (A_zdroj[i + 1] - A_zdroj[i]);
            return A_ampermetr[i] + t * (A_ampermetr[i + 1] - A_ampermetr[i]);
        }
    }
    return input; // Fallback
}




float C_get_Measured_Current_Correction_Factor(float current) {
    if (current <= A_measuredCurrent[0]) return A_setCurrent[0] / A_measuredCurrent[0];
    if (current >= A_measuredCurrent[C_tableSize - 1]) return A_setCurrent[C_tableSize - 1] / A_measuredCurrent[C_tableSize - 1];

    for (int i = 0; i < C_tableSize - 1; i++) {
        if (current >= A_measuredCurrent[i] && current <= A_measuredCurrent[i + 1]) {
            float factor1 = A_setCurrent[i] / A_measuredCurrent[i];
            float factor2 = A_setCurrent[i + 1] / A_measuredCurrent[i + 1];
            float t = (current - A_measuredCurrent[i]) / (A_measuredCurrent[i + 1] - A_measuredCurrent[i]);
            return factor1 + t * (factor2 - factor1);
        }
    }
    return 1.0; // Fallback
}

float C_apply_Measured_Current_Correction(float current) {
    float correctionFactor = C_get_Measured_Current_Correction_Factor(current);
    Serial.print("cor factor: "); Serial.print(correctionFactor);
    return current * correctionFactor;
}



















