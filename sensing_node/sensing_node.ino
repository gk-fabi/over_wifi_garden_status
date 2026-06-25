//Librerías y definición de pines para los sensores
#include "DHT.h"
#include "ESP8266WiFi.h"
#define yl_pin A0
#define dht_pin 14
#define raindrop_pin 5

//Credenciales WiFi
const char* ssid = "ASTRID G";
const char* wifi_password = "82330693";

//Creación se objetos
DHT dht(dht_pin, DHT11);
WiFiServer server(80);

void setup(){
  Serial.begin(115200);
  delay(500);

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
  //Web server request
  WiFiClient client = server.available();
  if(client){
    Serial.println(F("Cliente conectado al servidor web\n"));
    while(client.connected()){
      if(client.available()){
        String line = client.readStringUntil('\r'); //Lee línea por línea la petición del cliente
        Serial.print(line);
        if(line.length() == 1 && line[0] == '\n'){ //La finalización de la petición del cliente se marca con un salto de línea vacío
          //Página web a entregar
          //...
          //...
        break;
        }
      }
      //Se revisa si aún se reciben bytes del encabezado TCP de la petición del cliente, para asegurarse de no cortar la conexión muy pronto
      while(client.available()){
        client.read();
      }
      client.stop();
    }
  }

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