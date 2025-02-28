
#include <EEPROM.h>
#include <Wire.h> // drát
#include <LiquidCrystal_I2C.h>
#include <ADS1X15.h> // ad converter
#define ON 1
#define OFF 0


#define programversion 1.00
#define rate_of_refreshing 350 // refreshrate hlavni obrazovky
#define DEBUG ON //    ON / OFF    1/0     debugovaci odesilani hodnot do seriove linky


////////////////////////////////////// piny digitálních potenciometrů

////////////////////////////////////// enkoder 1
#define PIN_DT  23 // napětí
#define PIN_CLK 22
#define PIN_BTN1 24
int lastStateDT;
int currentStateDT;
int stepCounter = 0;

int lastStateDT2;
int currentStateDT2;
int stepCounter2 = 0;

unsigned long lastDebounceTime = 0; // pamet zpoždění enkoderu // enkoder napeti hlavni obrazovka
const unsigned long debounceDelay = 5; // zpoždění enkoderu

unsigned long lastDebounceTime2 = 0; // pamet zpoždění enkoderu // enkoder u zadavani napeti
const unsigned long debounceDelay2 = 5; // zpoždění enkoderu

unsigned long lastDebounceTime3 = 0; // pamet zpoždění enkoderu // tlacitko u zadavani napeti
const unsigned long debounceDelay3 = 150; // zpoždění enkoderu

////////////////////////////////////// enkoder 2
#define PIN_DT2  25 // proud
#define PIN_CLK2 26




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
static int refreshrate = rate_of_refreshing ;



void setup() {
  proud = EEPROM.read(1)-1;
  ///////////////////////////////////////////enkoder 1
  pinMode(PIN_DT, INPUT_PULLUP);
  pinMode(PIN_CLK, INPUT_PULLUP);
  pinMode(PIN_BTN1, INPUT_PULLUP);
  lastStateDT = digitalRead(PIN_DT);
  


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
    
    lcd.setCursor(0, 0);
    
    int16_t valadc1 = ADS.readADC(1);
    int16_t valadc0 = ADS.readADC_Differential_0_2();
    

    
    float displvolt = (valadc0 * f ) * 13.269933; // vypocet vysupniho napeti
    float displcur = (valadc1 * f) / 0.228; // vypocet vysupniho proudu

    if (displcur < 0) {
    displcur = 0;
    }

    if (displvolt < 0) {
    displvolt = 0;
    }

    lcd.print("NAPETI :  ");
    if (displvolt > 10) {
    lcd.setCursor(9, 0);
    }else {
    lcd.setCursor(10, 0);
    }
    
    lcd.print(displvolt , 3); // funkci atestovano napeti
    lcd.setCursor(16, 0);
    lcd.print("V");

    lcd.setCursor(0, 2);
    lcd.print("PROUD :   ");
    if (displcur > 10) {
    lcd.setCursor(9, 2);
    }else {
    lcd.setCursor(10, 2);
    }
    
    lcd.print(displcur , 3); // funkci atestovano proud
    lcd.setCursor(16, 2);
    lcd.print("A");
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









































