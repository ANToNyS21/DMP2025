const int tableSize = 32;
float requestedVoltages[tableSize] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32};
float realVoltages[tableSize] = {1.46, 2.39, 3.32, 4.2, 5.1, 6.1, 7, 8, 8.8, 9.8, 10.7, 11.6, 12.6, 13.5, 14.4, 15.3, 16.3, 17.2, 18.1, 19.09, 20, 20.9, 21.8, 22.7, 23.7, 24.6, 25.5, 26.4, 27.4, 28.3, 29.2, 30.1};

float getCorrectionFactor(float voltage) {
    if (voltage <= requestedVoltages[0]) return requestedVoltages[0] / realVoltages[0];
    if (voltage >= requestedVoltages[tableSize - 1]) return requestedVoltages[tableSize - 1] / realVoltages[tableSize - 1];
    
    for (int i = 0; i < tableSize - 1; i++) {
        if (voltage >= requestedVoltages[i] && voltage <= requestedVoltages[i + 1]) {
            float factor1 = requestedVoltages[i] / realVoltages[i];
            float factor2 = requestedVoltages[i + 1] / realVoltages[i + 1];
            float t = (voltage - requestedVoltages[i]) / (requestedVoltages[i + 1] - requestedVoltages[i]);
            return factor1 + t * (factor2 - factor1);
        }
    }
    return 1.0; // Safety fallback
}

float applyCorrection(float voltage) {
    float correctionFactor = getCorrectionFactor(voltage);
    return voltage * correctionFactor;
}

void setup() {
    Serial.begin(9600);
        int input = 14.7;
        float corrected = applyCorrection(input);
        Serial.print("Input: "); Serial.print(input);
        Serial.print("V, Corrected: "); Serial.print(corrected);
        Serial.println("V");
}

void loop() {
}