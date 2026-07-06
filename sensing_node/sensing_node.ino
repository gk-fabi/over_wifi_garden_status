//Librerías y definición de pines para los sensores
#include "DHT.h"
#include "ESP8266WiFi.h"
#include "webpage.h"
#define yl_pin A0
#define dht_pin 14
#define raindrop_pin 5


//Credenciales WiFi
const char* ssid = "ASTRID G";
const char* wifi_password = "82330693";


//Creación de objetos
DHT dht(dht_pin, DHT11);
WiFiServer server(80);


//¿Está lloviendo? (Función)
String is_raining(bool rain_reading){
  if(!rain_reading){
    return "Sí";
  } else {
    return "No";
  }
}


//Preparación página HTML
String prepareHTML(float temp, float air, float soil, float rain){
  String page = "<!DOCTYPE html><html><head><meta charset='UTF-8'><meta http-equiv='refresh' content='5'><title>Garden Status</title></head><body>";
  page += "<h1>Garden Status</h1>";
  page += "<p>Temperatura: ";
  page += String(temp, 1);
  page += " °C</p>";
  page += "<p>Humedad del aire: ";
  page += String(air, 1);
  page += " %</p>";
  page += "<p>Humedad de la tierra: ";
  page += String(soil);
  page += " %</p>";
  page += "<p>¿Está lloviendo?: ";
  page += is_raining(rain);
  page += "</p></body></html>";
  return page;
}


void setup(){
  Serial.begin(115200);
  delay(500);

  WiFi.mode(WIFI_STA);

  //Configuración de pines e inicialización de DHT11
  pinMode(yl_pin, INPUT);
  pinMode(raindrop_pin, INPUT);
  dht.begin();
  Serial.println(F("\n¡Configuración de pines e inicialización de sensores de manera exitosa!"));

  //Conexión WiFi e inicialización del server
  Serial.printf("Conectándose a la red WiFi \"%s\"", ssid);
  WiFi.begin(ssid, wifi_password);
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  Serial.print(F("\n¡Conexión exitosa! Dirección IP del servidor: "));
  Serial.print(WiFi.localIP());
  Serial.println("");
  server.begin();
}





void loop(){
  //Variables que albergan datos leídos
  float air_humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  int soil_humidity = analogRead(yl_pin);
  int soil_percent = map(soil_humidity, 1024, 260, 0, 100);
  // soil_percent = constrain(soil_percent, 0, 100); (Esta linea es para que los valores no salgan de 0-100%. Descomentarla cuando haya una excelente calibración)
  bool rain_status = digitalRead(raindrop_pin);
  

  //Web server request
  WiFiClient client = server.available();
  if(client){
    Serial.println(F("Cliente conectado al servidor web"));

    while(client.connected() && !client.available()){
      delay(1);
    }

    while(client.available()){
      client.read();
    }

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    client.print(prepareHTML(dht.readTemperature(), dht.readHumidity(), map(analogRead(yl_pin), 1024, 260, 0, 100), digitalRead(raindrop_pin)));
    client.flush();
    client.stop();
    Serial.println(F("Página web enviada al cliente"));
  }
  

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