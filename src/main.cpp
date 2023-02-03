#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
#include <ArduinoJson.h>

// Relays
#define pumpPin 27
#define lightAPin 26
#define lightBPin 25
#define lightCPin 33


// Sensors
#define rainAnalog 35
#define rainDigital 34
#define soilMoisture 36
#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "spacex-2.4GHz@unifi";
const char* password = "@y^hk0+8a_s~}0/*";

// This is ESP32 Sensors
const char* sensors_api_key = "49ui35hb4dc6w1udq635";

// This is ESP32 Control
const char* control_api_key = "xeedwod9r7hta0bk8fjb";



void sendDataToPlatform(const char *api_key, float field1, float field2, float field3, float field4, float field5) {
  HTTPClient http;
  
  String write_url = "https://kennedywee.pythonanywhere.com/api/data/write/";
  write_url += "?api_key=";
  write_url += api_key;
  write_url += "&field1=";
  write_url += field1;
  write_url += "&field2=";
  write_url += field2;
  write_url += "&field3=";
  write_url += field3;
  write_url += "&field4=";
  write_url += field4;
  write_url += "&field5=";
  write_url += field5;

  http.begin(write_url);

  int httpCode = http.GET();
  if (httpCode > 0) {
    String response = http.getString();
    Serial.println(httpCode);
    Serial.println(response);
  } else {
    Serial.println("Error on HTTP request");
  }

  http.end();
}

String sendReadRequest(const char *api_key) {
  String response;
  HTTPClient http;
  
  String read_url = "https://kennedywee.pythonanywhere.com/api/data/read/";
  read_url += "?api_key=";
  read_url += api_key;

  http.begin(read_url);

  int httpCode = http.GET();
  if (httpCode > 0) {
    response = http.getString();
    Serial.println(httpCode);
    Serial.println(response);
  } else {
    Serial.println("Error on HTTP request");
  }

  return response;

  http.end();
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(pumpPin, OUTPUT);
  pinMode(lightAPin, OUTPUT);
  pinMode(lightBPin, OUTPUT);
  pinMode(lightCPin, OUTPUT);

  digitalWrite(pumpPin, LOW);
  digitalWrite(lightAPin, LOW);
  digitalWrite(lightBPin, LOW);
  digitalWrite(lightCPin, LOW);

  pinMode(rainDigital, INPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  
  String dataFromCloud = sendReadRequest(control_api_key);
  StaticJsonDocument<200> data;
  DeserializationError error = deserializeJson(data, dataFromCloud);

  int pump = data["field1"];
  int lightA = data["field2"];
  int lightB = data["field3"];
  int lightC = data["field4"];


  if (pump == 1) digitalWrite(pumpPin, HIGH);
  else digitalWrite(pumpPin, LOW);

  if (lightA == 1) digitalWrite(lightAPin, HIGH);
  else digitalWrite(lightAPin, LOW);

  if (lightB == 1) digitalWrite(lightBPin, HIGH);
  else digitalWrite(lightBPin, LOW);

  if (lightC == 1) digitalWrite(lightCPin, HIGH);
  else digitalWrite(lightCPin, LOW);


  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  int rainAnalogVal = analogRead(rainAnalog);
  int rainDigitalVal = digitalRead(rainDigital);
  int soilMoistureVal = analogRead(soilMoisture);

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Serial.print("Humidity: ");
  // Serial.print(humidity);
  // Serial.print(" %\t");
  // Serial.print("Temperature: ");
  // Serial.print(temperature);
  // Serial.println(" *C");
  
  // Serial.print("Rain Analog: ");
  // Serial.print(rainAnalogVal);
  // Serial.print("\t");
  // Serial.print("Rain Digital: ");
  // Serial.println(rainDigitalVal);

  // Serial.print("Soil Moisture: ");
  // Serial.println(soilMoistureVal);

  // Serial.println("field1: " + String(field1));
  // Serial.println("field2: " + String(field2));
  // Serial.println("field3: " + String(field3));
  // Serial.println("field4: " + String(field4));
  // Serial.println("field5: " + String(field5));

  

  sendDataToPlatform(sensors_api_key, temperature, humidity, soilMoistureVal, rainDigitalVal, rainAnalogVal );

  delay(3000);
  
}