


//PIN_relay5 = napajni modulu 1
//PIN_relay6 = napajni modulu 2
//PIN_relay4 = hlavní OUT vypinac

//PIN_relay2 + PIN_relay3 = paraler
//PIN_relay1 = ser


#define PIN_relay5 3
#define PIN_relay6 2



#define PIN_relay1 14
#define PIN_relay2 15
#define PIN_relay3 16
#define PIN_relay4 17

#define betweendelay 60


void setup() {
  pinMode(PIN_relay1, OUTPUT);
  digitalWrite(PIN_relay1, HIGH);
  pinMode(PIN_relay2, OUTPUT);
  digitalWrite(PIN_relay2, HIGH);
  pinMode(PIN_relay3, OUTPUT);
  digitalWrite(PIN_relay3, HIGH);
  pinMode(PIN_relay4, OUTPUT);
  digitalWrite(PIN_relay4, HIGH);

  pinMode(PIN_relay5, OUTPUT);
  digitalWrite(PIN_relay5, HIGH);

  pinMode(PIN_relay6, OUTPUT);
  digitalWrite(PIN_relay6, HIGH);
  


}

void loop() {
  //digitalWrite(PIN_relay4, LOW);

}
