#include <Wire.h>
#include <Adafruit_MCP4725.h>

Adafruit_MCP4725 dac;

void setup() {
  Serial.begin(9600);
  Serial.println("MCP4725 AOUT to 4.5V");
  float const desiredvout = 1.8;
  pinMode(52, OUTPUT);

  dac.begin(0x60); // Adresa I2C pro MCP4725 je obvykle 0x60

  // Předpokládáme, že MCP4725 je napájen 5V
  // 4095 odpovídá 5V, takže pro 4.5V:
  int output_value = ((desiredvout / 5.0) * 4095) - 20;

  dac.setVoltage(output_value, true);

  Serial.print("DAC output set to: ");
  Serial.print(desiredvout);
  Serial.println("V");
  digitalWrite(52, HIGH);
  
}

void loop() {
  // Nic se neděje v loop
}
