//Librerías para DHT11 y definición de pines para los sensores
#include "DHT.h"
#define yl_pin A0
#define dht_pin 14
#define raindrop_pin 5

//Creación del objeto del sensor DHT e inicialización
DHT dht(dht_pin, DHT11);

void setup(){
  pinMode(yl_pin, INPUT);
  pinMode(raindrop_pin, INPUT);
  Serial.begin(115200);
  Serial.println("¡Configuración de modos de pines y serial iniciada!");
  Serial.println();
  dht.begin();
}

void loop(){
  //Variables que albergan datos leídos
  float air_humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  int soil_humidity = analogRead(yl_pin);
  int soil_percent = map(soil_humidity, 1024, 260, 0, 100);
  // soil_percent = constrain(soil_percent, 0, 100); (Esta linea es para que los valores no salgan de 0-100%. Decomentarla cuando haya una excelente calibración)
  bool rain_status = digitalRead(raindrop_pin);
  

  //Lectura YL-69
  Serial.print("Humedad en la tierra: ");
  Serial.println(soil_percent);

  //Lectura DHT11
  Serial.print("Temperatura y humedad en el aire: ");
  Serial.print(temperature);
  Serial.print("°C ");
  Serial.print(air_humidity);
  Serial.println("%");

  //Lectura raindrop
  if(!rain_status){
    Serial.println("Y, esta lloviendo.");
  } else {
    Serial.println("Y, no esta lloviendo.");
  }

  Serial.println("");
  Serial.println("");
  delay(3000);
}