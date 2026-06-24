//Librerías y definición de pines para los sensores
#include "DHT.h"
#include "ESP8266WiFi.h"
#define yl_pin A0
#define dht_pin 14
#define raindrop_pin 5

//Credenciales WiFi
const char* ssid = "ASTRID G";
const char* wifi_password = "82330693";

//Creación del objeto del sensor DHT e inicialización
DHT dht(dht_pin, DHT11);

void setup(){
  Serial.begin(115200);
  delay(500);
  Serial.println("");

  //Configuración de pines e inicialización de DHT11
  pinMode(yl_pin, INPUT);
  pinMode(raindrop_pin, INPUT);
  dht.begin();
  Serial.println("¡Configuración de pines e inicialización de sensores de manera exitosa!");

  //Conexión WiFi
  Serial.println("-------------------------------------------------------");
  Serial.print("Conectándose a la red WiFi \"");
  Serial.print(ssid);
  Serial.print("\"");
  WiFi.begin(ssid, wifi_password);
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("¡Conexión exitosa!");
  Serial.print("Dirección IP del NodeMCU: ");
  Serial.println(WiFi.localIP());
  Serial.println("-------------------------------------------------------");
  Serial.println("");
}

void loop(){
  //Variables que albergan datos leídos
  float air_humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  int soil_humidity = analogRead(yl_pin);
  int soil_percent = map(soil_humidity, 1024, 260, 0, 100);
  // soil_percent = constrain(soil_percent, 0, 100); (Esta linea es para que los valores no salgan de 0-100%. Descomentarla cuando haya una excelente calibración)
  bool rain_status = digitalRead(raindrop_pin);
  

  //Lectura YL-69
  Serial.print("Humedad en la tierra: ");
  Serial.print(soil_percent);
  Serial.println("%");

  //Lectura DHT11
  Serial.print("Temperatura ");
  Serial.print(temperature);
  Serial.println("°C ");
  Serial.print("Humedad en el aire: ");
  Serial.print(air_humidity);
  Serial.println("%");

  //Lectura raindrop
  Serial.print("¿Está lloviendo?: ");
  if(!rain_status){
    Serial.println("Sí");
  } else {
    Serial.println("No");
  }

  Serial.println("");
  delay(3000);
}