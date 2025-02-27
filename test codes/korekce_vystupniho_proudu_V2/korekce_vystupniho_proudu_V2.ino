const int A_AtableSize = 40; // Počet hodnot v tabulce

float A_ampermetr[A_AtableSize] = {0, 0.2, 0.4, 0.6, 0.8, 1.0, 1.2, 1.4, 1.6, 1.8, 2.0, 2.2, 2.4, 2.6, 2.8, 3.0, 3.2, 3.4, 3.6, 3.8, 4.0, 4.2, 4.4, 4.6, 4.8, 5.0, 5.2, 5.4, 5.6, 5.8, 6.0, 6.2, 6.4, 6.6, 6.8, 7.0, 7.2, 7.4, 7.6, 7.8};
float A_zdroj[A_AtableSize] = {0, 0.38, 0.58, 0.79, 0.98, 1.17, 1.37, 1.57, 1.76, 1.94, 2.14, 2.33, 2.53, 2.74, 2.92, 3.12, 3.31, 3.50, 3.69, 3.86, 4.05, 4.24, 4.43, 4.63, 4.81, 4.99, 5.19, 5.38, 5.56, 5.73, 5.91, 6.09, 6.26, 6.50, 6.68, 6.88, 7.08, 7.29, 7.51, 7.70};

float get_Corrected_Output(float input) {
    if (input <= A_zdroj[0]) return A_ampermetr[0];
    if (input >= A_zdroj[A_AtableSize - 1]) return A_ampermetr[A_AtableSize - 1];

    for (int i = 0; i < A_AtableSize - 1; i++) {
        if (input >= A_zdroj[i] && input <= A_zdroj[i + 1]) {
            float t = (input - A_zdroj[i]) / (A_zdroj[i + 1] - A_zdroj[i]);
            return A_ampermetr[i] + t * (A_ampermetr[i + 1] - A_ampermetr[i]);
        }
    }
    return input; // Fallback
}

void setup() {
    Serial.begin(9600);
    
    
        float input = 0.1;
        float corrected = get_Corrected_Output(input);
        Serial.print("Input: "); Serial.print(input);
        Serial.print("A, Corrected: "); Serial.print(corrected);
        Serial.println("A");
    
}

void loop() {}
