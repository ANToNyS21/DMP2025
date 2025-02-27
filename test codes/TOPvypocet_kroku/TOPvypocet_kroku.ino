// Konstanty
const float maxVoltage = 32.0;    // Maximální vstupní napětí 32V
const float resistorValue = 100000.0;  // Hodnota odporu jednoho potenciometru
const float targetResistance = 50000.0;  // Paralelní kombinace dvou potenciometrů

// Funkce pro výpočet napětí dle X a Y
float calculateVoltage(int X, int Y) {
  float R1 = (resistorValue / 256) * X;  // Výpočet odporu prvního potenciometru
  float R2 = (resistorValue / 256) * Y;  // Výpočet odporu druhého potenciometru
  float R_parallel = (R1 * R2) / (R1 + R2);  // Výpočet paralelního odporu
  float voltage = (R_parallel / targetResistance) * maxVoltage;  // Výpočet napětí
  return voltage;
}

void setup() {
  Serial.begin(9600);  // Nastavení sériové komunikace
}

void loop() {
  // Zadané cílové napětí
  float desiredVoltage = 0.2;  // Můžete měnit tuto hodnotu dle potřeby

  // Proměnné pro potenciometry
  int X, Y;
  bool found = false;  // Příznak, zda byly nalezeny odpovídající hodnoty

  // Iterace přes všechny možné kombinace X a Y
  for (X = 0; X <= 255; X++) {
    for (Y = 0; Y <= 255; Y++) {
      float voltage = calculateVoltage(X, Y);  // Výpočet napětí pro aktuální X a Y
      if (abs(voltage - desiredVoltage) < 0.01) {  // Pokud je rozdíl menší než 0.01V
        Serial.print("Found values: X = ");
        Serial.print(X);
        Serial.print(", Y = ");
        Serial.print(Y);
        Serial.print(" for desired voltage: ");
        Serial.println(voltage);
        found = true;
        break;  // Pokud najdeme správné hodnoty, ukončíme vnitřní cyklus
      }
    }
    if (found) break;  // Pokud najdeme správné hodnoty, ukončíme i vnější cyklus
  }

  if (!found) {
    Serial.println("No matching values found.");
  }

  delay(1000);  // Pauza mezi iteracemi
}
