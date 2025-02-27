const int C_tableSize = 15; // Počet hodnot v nové tabulce

float A_setCurrent[C_tableSize] = {0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1, 1.1, 1.2, 1.3, 1.4};
float A_measuredCurrent[C_tableSize] = {0, 0.2, 0.25, 0.3, 0.4, 0.45, 0.5, 0.55, 0.62, 0.73, 0.82, 0.93, 1, 1.12, 1.21};

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



void setup() {
    Serial.begin(9600);
    float input = 0.6; // Změň hodnotu pro testování
    float corrected = C_apply_Measured_Current_Correction(input);
    float diference = corrected - input;
    Serial.print("Input: "); Serial.print(input);
    Serial.print("A, Corrected: "); Serial.print(corrected);
    Serial.println();
    Serial.print("diference: "); Serial.print(diference);
    Serial.println("A");
}

void loop() {
}
