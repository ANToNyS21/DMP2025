const int A_Out_tableSize = 41; // Počet hodnot v tabulce

float setCurrent[A_Out_tableSize] = {0, 0.2, 0.4, 0.6, 0.8, 1, 1.2, 1.4, 1.6, 1.8, 2, 2.2, 2.4, 2.6, 2.8, 3, 3.2, 3.4, 3.6, 3.8, 4, 4.2, 4.4, 4.6, 4.8, 5, 5.2, 5.4, 5.6, 5.8, 6, 6.2, 6.4, 6.6, 6.8, 7, 7.2, 7.4, 7.6, 7.8, 8};
float realCurrent[A_Out_tableSize] = {0, 0.38, 0.588, 0.8, 0.98, 1.18, 1.2, 1.4, 1.6, 1.8, 2, 2.2, 2.4, 2.6, 2.8, 3, 3.2, 3.4, 3.6, 3.8, 4, 4.2, 4.4, 4.6, 4.8, 5, 5.2, 5.4, 5.6, 5.8, 6, 6.2, 6.4, 6.6, 6.8, 7, 7.2, 7.4, 7.6, 7.8, 8};

// Funkce najde nejbližší korekční faktor
float getCorrectedSetValue(float targetCurrent) {
    if (targetCurrent <= realCurrent[0]) return setCurrent[0];
    if (targetCurrent >= realCurrent[A_Out_tableSize - 1]) return setCurrent[A_Out_tableSize - 1];

    for (int i = 0; i < A_Out_tableSize - 1; i++) {
        if (targetCurrent >= realCurrent[i] && targetCurrent <= realCurrent[i + 1]) {
            float t = (targetCurrent - realCurrent[i]) / (realCurrent[i + 1] - realCurrent[i]);
            return setCurrent[i] + t * (setCurrent[i + 1] - setCurrent[i]);
        }
    }
    return targetCurrent; // Bez korekce jako fallback
}

void setup() {
    Serial.begin(9600);

    float targetCurrent = 5.345; // Testovací hodnota, kterou chceme na výstupu
    float correctedSet = getCorrectedSetValue(targetCurrent);
    
    Serial.print("Target Current: ");
    Serial.print(targetCurrent);
    Serial.print("A, Corrected Set Value: ");
    Serial.print(correctedSet);
    Serial.println("A");
}

void loop() {
}
