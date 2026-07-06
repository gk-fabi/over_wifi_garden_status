//Librerías, pines y credenciales WiFi
#include "DHT.h"
#include "ESP8266WiFi.h"
#include "webpage.h"
#define yl_pin A0
#define dht_pin 14
#define raindrop_pin 5
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
  String page = F(R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <meta charset="UTF-8">
      <meta http-equiv="refresh" content="5">
      <title>Garden Status</title>
    </head>
    <body>
      <h1>Garden Status</h1>
      <p>Temperatura: %TEMPERATURE%°C</p>
      <p>Humedad del aire: %AIR_HUMIDITY%%</p>
      <p>Humedad de la tierra: %SOIL_HUMIDITY%%</p>
      <p>¿Está lloviendo?: %RAINING%</p>
    </body>
    </html>
  )rawliteral");
  page.replace("%TEMPERATURE%", String(temp));
  page.replace("%AIR_HUMIDITY%", String(air));
  page.replace("%SOIL_HUMIDITY%", String(soil));
  page.replace("%RAINING%", is_raining(rain));
  return page;
}


void setup(){
  Serial.begin(115200);
  delay(500);
  

  //Modos, sensores e inicializaciones
  WiFi.mode(WIFI_STA);
  pinMode(yl_pin, INPUT);
  pinMode(raindrop_pin, INPUT);
  dht.begin();


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