// Definice pinů pro enkodér
#define PIN_DT  23
#define PIN_CLK 22

// Proměnná pro sledování hodnoty enkodéru
int encoderValue = 0;

// Proměnné pro uložení posledního a aktuálního stavu
int lastStateDT;
int currentStateDT;


int stepCounter = 0;

// Čas posledního čtení (pro debounce)
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 5;  // Zpoždění pro debounce (v milisekundách)

void setup() {
  // Nastavení pinů jako vstupy
  pinMode(PIN_DT, INPUT_PULLUP);
  pinMode(PIN_CLK, INPUT_PULLUP);

 

  // Inicializace sériové komunikace
  Serial.begin(9600);

  // Uložení počátečního stavu pinu DT
  lastStateDT = digitalRead(PIN_DT);
}

void loop() {
 
  currentStateDT = digitalRead(PIN_DT);
  if (currentStateDT != lastStateDT && (millis() - lastDebounceTime) > debounceDelay) {
    lastDebounceTime = millis();  // reset  časovače
    stepCounter++;  

    // pouziti kazdeho druheho kroku
    if (stepCounter % 2 == 0) {
      if (digitalRead(PIN_CLK) != currentStateDT) {// určení směru
        napeti--;  // proti směru hodinových ručiček
      } else {
        napeti++;  // ve směru hodinových ručiček
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
  }

  lastStateDT = currentStateDT; // ulozeni do pameti
}
