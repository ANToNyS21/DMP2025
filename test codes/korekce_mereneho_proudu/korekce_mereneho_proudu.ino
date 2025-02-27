const int AtableSize = 40; // Počet hodnot v tabulce

float zdroj[AtableSize] = {0.38, 0.588, 0.8, 0.98, 1.18, 1.37, 1.57, 1.76, 1.94, 2.14, 2.34, 2.54, 2.73, 2.92, 3.11, 3.31, 3.49, 3.68, 3.86, 4.04, 4.23, 4.43, 4.63, 4.81, 4.99, 5.16, 5.36, 5.56, 5.72, 5.91, 6.09, 6.29, 6.49, 6.69, 6.89, 7.09, 7.32, 7.55, 7.76, 7.967};
float ampermetr[AtableSize] = {0.28, 0.6, 0.9, 1.18, 1.48, 1.77, 2.06, 2.36, 2.63, 2.93, 3.24, 3.53, 3.83, 4.13, 4.4, 4.71, 5.01, 5.31, 5.59, 5.86, 6.15, 6.47, 6.79, 7.1, 7.36, 7.63, 7.95, 8.32, 8.58, 8.89, 9.21, 9.55, 9.95, 10.29, 10.65, 10.95, 11.39, 11.75, 12.21, 12.57};

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
    return 1.0; // Fallback
}

float apply_Measured_Current_Correction(float current) {
    float correctionFactor = get_Measured_Current_Correction_Factor(current);
    return current * correctionFactor;
}

void setup() {
    Serial.begin(9600);
        float input = 0.1;
        float corrected = apply_Measured_Current_Correction(input);
        Serial.print("Input: "); Serial.print(input);
        Serial.print("A, Corrected: "); Serial.print(corrected);
        Serial.println("A");
}

void loop() {
}