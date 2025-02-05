
#define PIN_relay5 3
#define PIN_relay6 2



#define PIN_relay1 14
#define PIN_relay2 15
#define PIN_relay3 16
#define PIN_relay4 17

#define betweendelay 60

int serparrelaymode = 2; // 0 nic    1 serial    2 paraler

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

//PIN_relay5 = napajni modulu 2 (dole)
//PIN_relay6 = napajni modulu 1 (nahore)
//PIN_relay4 = hlavní OUT vypinac

//PIN_relay2 + PIN_relay3 = paraler
//PIN_relay1 = ser

void loop() {
  
  switch (serparrelaymode) {
   case 0:
      digitalWrite(PIN_relay1, HIGH);
      digitalWrite(PIN_relay2, HIGH);
      digitalWrite(PIN_relay3, HIGH);
        break;
   case 1:
      digitalWrite(PIN_relay1, LOW);
      digitalWrite(PIN_relay2, HIGH);
      digitalWrite(PIN_relay3, HIGH);
        break;
   case 2:
      digitalWrite(PIN_relay1, HIGH);
      digitalWrite(PIN_relay2, LOW);
      digitalWrite(PIN_relay3, LOW);
        break;
  }
  
/*
  delay(200);
  digitalWrite(PIN_relay1, LOW);
  delay(betweendelay);
  digitalWrite(PIN_relay1, HIGH);
  delay(betweendelay);
  digitalWrite(PIN_relay1, LOW);
  delay(betweendelay);
  digitalWrite(PIN_relay1, HIGH);
  delay(betweendelay);

  digitalWrite(PIN_relay2, LOW);
  delay(betweendelay);
  digitalWrite(PIN_relay2, HIGH);
  delay(betweendelay);
  digitalWrite(PIN_relay2, LOW);
  delay(betweendelay);
  digitalWrite(PIN_relay2, HIGH);
  delay(betweendelay);

  digitalWrite(PIN_relay3, LOW);
  delay(betweendelay);
  digitalWrite(PIN_relay3, HIGH);
  delay(betweendelay);
  digitalWrite(PIN_relay3, LOW);
  delay(betweendelay);
  digitalWrite(PIN_relay3, HIGH);
  delay(betweendelay);

  digitalWrite(PIN_relay4, LOW);
  delay(betweendelay);
  digitalWrite(PIN_relay4, HIGH);
  delay(betweendelay);
  digitalWrite(PIN_relay4, LOW);
  delay(betweendelay);
  digitalWrite(PIN_relay4, HIGH);
  delay(betweendelay);

  digitalWrite(PIN_relay5, LOW);
  delay(betweendelay);
  digitalWrite(PIN_relay5, HIGH);
  delay(betweendelay);
  digitalWrite(PIN_relay5, LOW);
  delay(betweendelay);
  digitalWrite(PIN_relay5, HIGH);
  delay(betweendelay);

  digitalWrite(PIN_relay6, LOW);
  delay(betweendelay);
  digitalWrite(PIN_relay6, HIGH);
  delay(betweendelay);
  digitalWrite(PIN_relay6, LOW);
  delay(betweendelay);
  digitalWrite(PIN_relay6, HIGH);
  delay(betweendelay);

  digitalWrite(PIN_relay1, LOW);
  digitalWrite(PIN_relay2, LOW);
  digitalWrite(PIN_relay3, LOW);
  digitalWrite(PIN_relay4, LOW);
  digitalWrite(PIN_relay5, LOW);
  digitalWrite(PIN_relay6, LOW);

  delay(5000);

  digitalWrite(PIN_relay1, HIGH);
  digitalWrite(PIN_relay2, HIGH);
  digitalWrite(PIN_relay3, HIGH);
  digitalWrite(PIN_relay4, HIGH);
  digitalWrite(PIN_relay5, HIGH);
  digitalWrite(PIN_relay6, HIGH);*/

}
