#include <Arduino.h>
#include <FS.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DHT.h>
#include <Adafruit_BMP280.h>
#include <SPIFFS.h>

// BMP280 Sensor
Adafruit_BMP280 bmp;

// DHT22 Sensor
DHT dht_sensor(5, DHT11);

const unsigned long interval = 1000; // Abrufintervall der Sensordaten in ms
unsigned long currentMillis = millis();
unsigned long lastMillis = millis();

// WiFi einstellungen
boolean hasWiFiLogin = true; // ob Wifi login data is stored
char defaultSSID[32] = "ESPWetterstation";
char defaultPassword[32] = "ESPWetterstation";

// MQTT Server einstellungen
const char *mqtt_server = "YOUR_MQTT_BROKER_IP_ADDRESS";
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

// Sensordaten
float temperature = 0;
float humidity = 0;
float pressure = 0;

// Webserver einstellungen
AsyncWebServer server(80);
AsyncEventSource events("/events");
IPAddress Ip(192, 168, 137, 137);
IPAddress NMask(255, 255, 255, 0);

int counter = 0;

void webServerSetup(){
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html"); });
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/script.js"); });
  server.on("/chart.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/chart.js"); });
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/style.css"); });
  server.on("/history.svg", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/history.svg"); });
  server.on("/home.svg", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/home.svg"); });
  server.on("/settings.svg", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/settings.svg"); });
  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);
  server.begin();
}

void setup()
{
  Serial.begin(115200);

  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  File wifiCredantials = SPIFFS.open("/wifiCredantials.txt");
  boolean afterSep = false;
  String ssid = "";
  String password = "";
  while (wifiCredantials.available())
  {
    int temp = wifiCredantials.read();
    if (temp == 59)
    {
      afterSep = true;
    }
    else
    {
      if (afterSep)
      {
        password += char(temp);
      }
      else
      {
        ssid += char(temp);
      }
    }
  }
  wifiCredantials.close();
  Serial.println(ssid);
  Serial.println(password);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());
  if(WiFi.status() != WL_CONNECTED)
  {
    WiFi.disconnect();
    WiFi.mode(WIFI_AP);
    WiFi.softAP(defaultSSID, defaultPassword);
    WiFi.softAPConfig(Ip, Ip, NMask);
    WiFi.begin();
  }
  
  webServerSetup();

  client.setServer(mqtt_server, 1883);

  if (!bmp.begin())
  {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or try a different address!"));
  }
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                  Adafruit_BMP280::SAMPLING_X2,
                  Adafruit_BMP280::SAMPLING_X16,
                  Adafruit_BMP280::FILTER_X16,
                  Adafruit_BMP280::STANDBY_MS_500);
}

void reconnect()
{
  /* verbindet sich mit MQTT Broker wenn die Verbindung unterbrochen wird */
  if (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("WiFiClient"))
    {
      Serial.println("connected");
    }
    else
    {
      Serial.print(client.state());
      delay(5000);
    }
  }
}

void sendMQTTData()
{
  /* sendet Sensordaten über MQTT an MQTT Broker */
  char tempString[8];
  dtostrf(temperature, 1, 2, tempString);
  client.publish("esp32/temperature", tempString);

  char humString[8];
  dtostrf(humidity, 1, 2, tempString);
  client.publish("esp32/humidity", humString);

  char presString[8];
  dtostrf(pressure, 1, 2, tempString);
  client.publish("esp32/pressure", presString);
}

void writeDataToFS(){
  File file = SPIFFS.open("/sensorData.csv", FILE_APPEND);
  if(!file){
     Serial.println("There was an error opening the file for writing");
     return;
  }
  if(file.println(String(temperature) + ";" + String(humidity) + ";" + String(pressure))){
      Serial.println("− message appended");
   } else {
      Serial.println("− append failed");
   }
}

void readDataFromLog(){
  Serial.println("Reading data from Log");
  File file = SPIFFS.open("/sensorData.csv");
  while(file.available()){
      Serial.write(file.read());
   }
}

void loop()
{
  //if (!client.connected())
  //{
  //  reconnect();
  //}
  //client.loop();

  currentMillis = millis();
  if (currentMillis - lastMillis >= interval)
  {
    // sensor Daten lesen
    humidity = dht_sensor.readHumidity();
    temperature = dht_sensor.readTemperature();
    pressure = bmp.readPressure();
    Serial.print(humidity);
    Serial.print(temperature);
    Serial.println(pressure);
    //sendMQTTData();
    //writeDataToFS();

    lastMillis = currentMillis;
    counter++;
  }
  if(counter == 10){
    counter = 0;
    //readDataFromLog();
  }
}