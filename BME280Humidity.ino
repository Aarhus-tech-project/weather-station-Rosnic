#include <Adafruit_BME280.h>
#include <Wire.h>
#include "netw.h"
#include <WiFiS3.h>
#include <ArduinoMqttClient.h>
#include "Adafruit_TSL2591.h"
#include <Adafruit_Sensor.h>

const int buttonPin = 2;
int buttonState = 1;
bool pause = 0;

Adafruit_BME280 bme;
char ssid[] = SECRET_SSID;
char pswd[] = SECRET_NETPSWD;

Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591);

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char danSite[] = "192.168.102.254";
const char serverSite[] = "192.168.111.11";
const char testSite[] = "test.mosquitto.org";
const int port = 1883;
const char data_topic[] = "all_data";
const char temp_topic[] = "class_temperature";
const char hum_topic[] = "class_humidity";
const char press_topic[] = "class_air_pressure";
const char light_topic[] = "class_lux";

const long interval = 20000;
unsigned long previousMs = 0;

int count = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  //-----------------------------------------------------
  pinMode(buttonPin, INPUT_PULLUP);
  
  //-----------------------------------------------------
  Serial.println("Scanning for BME280..");

  if (!bme.begin(0x76)) {
    Serial.println("No BME280 device found");
    while(1);
  }

  Serial.println("BME280 sensor found\n");

  //-----------------------------------------------------

  Serial.println("Scanning for TSL2591..");

  if (!tsl.begin(0x29)) {
    Serial.println("No TSL2591 device found");
    while(1);
  }

  Serial.println("TSL2591 sensor found\n");

  tsl.setGain(TSL2591_GAIN_MED);
  tsl.setTiming(TSL2591_INTEGRATIONTIME_500MS);

  //-----------------------------------------------------

  Serial.print("Trying to connect to WPA SSID: ");
  Serial.print(ssid);
  while (WiFi.begin(ssid, pswd) != WL_CONNECTED) {
    Serial.print('.');
    delay(2500);
  }
  Serial.println("\nConnected\n");
  delay(3000);
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.gatewayIP());

  //-----------------------------------------------------
  
  Serial.println("Trying to connect to MQTT broker");

  while (!mqttClient.connected()) {
    if (!mqttClient.connect(serverSite, port)) {
      Serial.println("Mqtt connection failed.");
      Serial.println("Retrying in 5 seconds..");
    }
    delay(5000);
  }

  Serial.println("Connected to MQTT broker\n");
}

//-----------------------------------------------------

void loop() {
  mqttClient.poll();

  float temp = bme.readTemperature();
  float humidity = bme.readHumidity();
  float pressure = bme.readPressure() / 100.0F;
  if (temp > 100) {
    bme.begin();
    Serial.println("BMP Reset Complete");
  }

  uint16_t lux = tsl.getLuminosity(TSL2591_VISIBLE);

  String tempString;
  tempString = String(temp);

  String humString;
  humString = String(humidity);

  String pressString;
  pressString = String(pressure);

  String luxString;
  luxString = String(lux);
  
  String completeData = tempString + ',' + humString + ',' + pressString + ',' + luxString;

  unsigned long currentMs = millis();

  if (currentMs - previousMs >= interval) {

    Serial.println(completeData);

    previousMs = currentMs;

    Serial.println("Sending Data to broker");
    mqttClient.beginMessage(data_topic);
    mqttClient.print(completeData);
    mqttClient.endMessage();
  }
}


