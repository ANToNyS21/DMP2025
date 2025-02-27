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
  float desiredVoltage = 23.6;// Můžete měnit tuto hodnotu dle potřeby

  // Proměnné pro potenciometry
  int X, Y;
  bool found = false;  // Příznak, zda byly nalezeny odpovídající hodnoty

  // Proměnné pro sledování nejbližšího výsledku
  float closestVoltage = 0;
  int closestX = 0;
  int closestY = 0;
  float minDifference = maxVoltage;  // Počáteční minimální rozdíl je maximální napětí
  int casovac = millis();
  Serial.print("starting now!!!");
  Serial.print(casovac);

  // Iterace přes všechny možné kombinace X a Y
  for (X = 0; X <= 255; X++) {
    for (Y = 0; Y <= 255; Y++) {
      float voltage = calculateVoltage(X, Y);  // Výpočet napětí pro aktuální X a Y
      float difference = abs(voltage - desiredVoltage);  // Rozdíl mezi vypočteným a cílovým napětím

      if (difference < 0.01) {  // Pokud je rozdíl menší než 0.01V, považujeme to za nalezenou shodu
        Serial.print("Found exact values: X = ");
        Serial.print(X);
        Serial.print(", Y = ");
        Serial.print(Y);
        Serial.print(" for desired voltage: ");
        Serial.println(voltage);
        found = true;
        casovac = millis();
        Serial.print(casovac);
        break;  // Pokud najdeme přesné hodnoty, ukončíme vnitřní cyklus
      }

      // Uložení nejbližší hodnoty
      if (difference < minDifference) {
        minDifference = difference;
        closestVoltage = voltage;
        closestX = X;
        closestY = Y;
      }
    }
    if (found) break;  // Pokud najdeme přesné hodnoty, ukončíme i vnější cyklus
  }

  if (!found) {
    // Pokud jsme nenašli přesné hodnoty, vypíšeme nejbližší možnou kombinaci
    Serial.println("No exact matching values found.");
    Serial.print("Closest values: X = ");
    Serial.print(closestX);
    Serial.print(", Y = ");
    Serial.print(closestY);
    Serial.print(" with voltage: ");
    Serial.println(closestVoltage);
    casovac = millis();
    Serial.print(casovac);
  }

  
}
