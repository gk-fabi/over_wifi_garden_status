//----------Librerías, pines y credenciales WiFi----------
#include "DHT.h"
#include "ESP8266WiFi.h"
#define yl_pin A0
#define dht_pin 14
#define raindrop_pin 5
const char* ssid = "ASTRID G";
const char* wifi_password = "82330693";

//----------Creación de objetos y variables----------
DHT dht(dht_pin, DHT11);
WiFiServer server(80);
struct SensorsData{
  float air_humidity;
  float temperature;
  int soil_humidity;
  int soil_percent;
  bool rain_status;
};

//----------Leer sensores----------
SensorsData readSensors(){
  SensorsData data;

  data.air_humidity = dht.readHumidity();
  data.temperature = dht.readTemperature();
  
  data.soil_humidity = analogRead(yl_pin);
  data.soil_percent = map(data.soil_humidity, 1024, 260, 0, 100);
  //data.soil_percent = constrain(data.soil_percent, 0, 100); (Esta linea es para que los valores no salgan de 0-100%. Descomentarla cuando haya una excelente calibración)
  
  data.rain_status = digitalRead(raindrop_pin);

  return data;
}

//----------¿Está lloviendo?----------
String is_raining(bool rain_reading){
  return !rain_reading ? "Sí" : "No";
}

//----------Preparación página HTML----------
String prepareHTML(const SensorsData& data){
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

  page.replace("%TEMPERATURE%", String(data.temperature));
  page.replace("%AIR_HUMIDITY%", String(data.air_humidity));
  page.replace("%SOIL_HUMIDITY%", String(data.soil_percent));
  page.replace("%RAINING%", is_raining(data.rain_status));

  return page;
}

//----------Conexión y solicitud web----------
void handleWebClient(){
  WiFiClient client = server.available();
  if(client){
    while(client.connected() && !client.available()){
      delay(1);
    }
    while(client.available()){
      client.read();
    }
    SensorsData sensors = readSensors();

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();

    client.print(prepareHTML(sensors));
    client.flush();
    client.stop();
  }
}

void setup(){
  Serial.begin(115200);
  
  //----------Modos, sensores e inicializaciones----------
  WiFi.mode(WIFI_STA);
  pinMode(yl_pin, INPUT);
  pinMode(raindrop_pin, INPUT);
  dht.begin();

  //----------Conexión WiFi e inicialización del server----------
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
  handleWebClient();
}