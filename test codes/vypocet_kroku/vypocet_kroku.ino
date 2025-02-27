#include <SPI.h>

float desiredVoltage = 14.7;  // Například 24V

const int CS_PIN1 = 10;  // Chip Select pro první potenciometr
const int CS_PIN2 = 9;   // Chip Select pro druhý potenciometr

const float R_MAX = 100000.0;    // 100kΩ maximální odpor jednoho potenciometru
const float R_MAX_PARALLEL = 50000.0;  // Maximální výsledný odpor při paralelním zapojení (50kΩ)
const float V_MAX = 32.0;        // Maximální napětí zdroje

const int MAX_STEPS = 256;       // 256 kroků (0 až 255)

// Funkce pro nastavení digitálního potenciometru
void setPotentiometer(int csPin, int value) {
  digitalWrite(csPin, LOW);
  SPI.transfer(0); // Adresa registru (záleží na potenciometru)
  SPI.transfer(value); // Nastavená hodnota (0 - 255)
  digitalWrite(csPin, HIGH);
}

// Funkce pro výpočet výsledného odporu dvou potenciometrů zapojených paralelně
float calculateParallelResistance(float R1, float R2) {
  if (R1 == 0 || R2 == 0) return 0;  // Pokud je jeden z odporů 0, výsledný odpor je také 0
  return (R1 * R2) / (R1 + R2);
}

// Hledání optimální kombinace kroků pro dosažení požadovaného odporu
void findOptimalSteps(float desiredResistance, int &steps1, int &steps2) {
  float minDifference = R_MAX_PARALLEL; // Pro porovnání, nastaveno na vysokou hodnotu
  for (int s1 = 0; s1 < MAX_STEPS; s1++) {
    float R1 = R_MAX * s1 / (MAX_STEPS - 1);
    for (int s2 = 0; s2 < MAX_STEPS; s2++) {
      float R2 = R_MAX * s2 / (MAX_STEPS - 1);
      float parallelR = calculateParallelResistance(R1, R2);
      float difference = abs(parallelR - desiredResistance);
      
      if (difference < minDifference) {
        minDifference = difference;
        steps1 = s1;
        steps2 = s2;
      }
    }
  }
}

void setup() {
  // Inicializace SPI a pinů
  SPI.begin();
  pinMode(CS_PIN1, OUTPUT);
  pinMode(CS_PIN2, OUTPUT);
  
  Serial.begin(9600);
  
 
  float desiredResistance = R_MAX_PARALLEL * (desiredVoltage / V_MAX); // Požadovaný odpor
  
  int steps1 = 0, steps2 = 0;
  
  // Najde optimální kroky
  findOptimalSteps(desiredResistance, steps1, steps2);
  
  // Nastaví potenciometry
  setPotentiometer(CS_PIN1, steps1);
  setPotentiometer(CS_PIN2, steps2);
  
  Serial.print("Steps Pot 1: ");
  Serial.println(steps1+1);
  Serial.print("Steps Pot 2: ");
  Serial.println(steps2+1);
}

void loop() {
  // Prázdná smyčka
}
