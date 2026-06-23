//Conectar el D0 del LM393 con el A0 del ESP8266
//¿Por qué? No se, pero funciona correctamente
#define yl_pin A0

void setup() {
  pinMode(yl_pin, INPUT);
  Serial.begin(115200);
}

void loop() {
  int yl_value = analogRead(yl_pin);
  Serial.println(yl_value);
  delay(2000);
}
