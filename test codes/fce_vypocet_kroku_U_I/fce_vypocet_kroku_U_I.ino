

void setup() {
  Serial.begin(9600); // Nastavení sériové komunikace
  initializeResistanceLookup_U();
  initializeResistanceLookup_I();

}

float resistanceLookup_U[256];
constexpr float MAX_VOLTAGE = 32.0;     // Maximální vstupní napětí
constexpr float RESISTOR_U_VALUE = 100000.0; // Hodnota odporu jednoho potenciometru
constexpr float TARGET_U_RESISTANCE = 50000.0; // Paralelní kombinace dvou potenciometrů
constexpr float TOLERANCE_U = 0.01;         // TOLERANCE_U pro shodu napětí
constexpr float DESIRED_U = 30.2;

float resistanceLookup_I[256];
constexpr float MAX_CURRENT = 4.00;     // Maximální vstupní napětí
constexpr float RESISTOR_I_VALUE = 5000; // Hodnota odporu jednoho potenciometru
constexpr float TARGET_I_RESISTANCE = 2500.0; // Paralelní kombinace dvou potenciometrů
constexpr float TOLERANCE_I = 0.01;         // TOLERANCE_I pro shodu napětí
constexpr float DESIRED_I = 3.5;




void loop() {
  vcalc();
  icalc();
  delay(5000);

}


int vcalc(){
  Serial.println("Starting voltage optimization...");


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
                Serial.print("Exact match found: X = ");
                Serial.print(X);
                Serial.print(", Y = ");
                Serial.print(Y);
                Serial.print(", Voltage = ");
                Serial.println(voltage);
                Serial.print("Time elapsed: ");
                Serial.print(millis() - startTime);
                Serial.println(" ms");
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



}

int icalc(){
  Serial.println("Starting current optimization...");


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
                Serial.print("Exact match found: X = ");
                Serial.print(X);
                Serial.print(", Y = ");
                Serial.print(Y);
                Serial.print(", Current = ");
                Serial.println(current);
                Serial.print("Time elapsed: ");
                Serial.print(millis() - startTime);
                Serial.println(" ms");
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


























