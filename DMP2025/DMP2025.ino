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

int modulepower1 = 0;
int modulepower2 = 0;
int serparrelaymode = 0; // 0 off    1 serial    2 paraler
int output = 0; // 0 off     1 out on

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


int set1 = 230; //CC 1 
int set2 = 230; //CC 1 
int set3 = 15; //CV 1
int set4 = 15; //CV 1
int set5 = 230; //CC 2
int set6 = 230; //CC 2
int set7 = 15; //CV 2
int set8 = 15; //CV 2


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
  /////////////////////////////////////////// adc
  Wire.begin();
  ADS.begin();
  ADS.setGain(0);
  /////////////////////////////////////////// vypocty
  initializeResistanceLookup_U();
  initializeResistanceLookup_I();
  
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



  int stav_tlacitka_2 = digitalRead(PIN_BTN2);
    static int stav_tlac_predchozi_2 = HIGH;

    
    if (stav_tlacitka_2 == LOW && stav_tlac_predchozi_2 == HIGH && (millis() - lastDebounceTime3) > debounceDelay3) {
      zadani_proudu();
      lastDebounceTime3 = millis();
    }
    stav_tlac_predchozi_2 = stav_tlacitka_2;

  
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
      lcd.clear();
      lcd.setCursor(5, 1);
      lcd.print("Loading...");
      lcd.setCursor(5, 2);
      lcd.print("Please wait");
      DESIRED_U = napeti;
      vcalc();
      goto exit_point;  // Přejdeme na label exit_point a ukončíme funkci
    }
    
    
  }
  exit_point:
  lcd.clear();

}

void zadani_proudu() {
    
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
      lcd.write(1);
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
      lcd.clear();
      lcd.setCursor(5, 1);
      lcd.print("Loading...");
      lcd.setCursor(5, 2);
      lcd.print("Please wait");
      DESIRED_I = proud;
      icalc();
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
          Serial.println("Starting voltage optimization...");
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



































