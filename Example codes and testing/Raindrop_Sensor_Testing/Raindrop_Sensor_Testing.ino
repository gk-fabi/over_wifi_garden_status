//El sensor de lluvia funciona correctamente de la siguiente manera:
//D0 del LM393 conectado a un pin digital del ESP8233, potenciómetro del módulo en toda la mitad
#define rainPin 14

void setup() {
  pinMode(rainPin, INPUT);
  Serial.begin(115200);
}

void loop() {
  int rainStatus = digitalRead(rainPin);
  Serial.println(rainStatus);
  delay(2000);
}
