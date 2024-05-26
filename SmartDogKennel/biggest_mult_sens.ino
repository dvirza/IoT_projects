#include <stdio.h>
#include <string.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "utils.h"



//TEMPERATURE AND HUMIDITY MEASURE + UPDATE DISPLAY APPLICATION's VARIABLES
bool obstacle_detect = false;
bool last_check = false;
bool ledState = false;
int tl = 1;
char conditionsString[50];  // Adjust the size based on your needs
char tempString[10];
char humidityString[10];
char printscreen[100];

//I/O WITH LIBRARIES DECLARATION
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
DHT dht_sensor(DHT_SENSOR_PIN,DHT_SENSOR_TYPE);

void setup() {
  //DHT11 Setup//
  dht_sensor.begin();
  delay(2000);
  
  //Display Setup//
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)){
    Serial.println(F("SSD1306 allocation failed"));
  }
  delay(1500);
  display.clearDisplay();
  
  //Ports I/O setup//
  pinMode(RedLed, OUTPUT);
  pinMode(YelLed, OUTPUT);
  pinMode(GreLed, OUTPUT);
  pinMode(SensorOut, INPUT);
  pinMode(DHT_SENSOR_PIN, INPUT);
  pinMode(mqttLED,OUTPUT);
  pinMode(foodLED,OUTPUT);
  pinMode(waterLED,OUTPUT);

  // Set software serial baud to 115200;
  Serial.begin(115200);
  delay(1000);
  connectAWS();

}

/////////////////////////////////////// FUNCTIONS /////////////////////////////////////////
void check_obstacle(){
  last_check = obstacle_detect;
  obstacle_detect = digitalRead(SensorOut);
}
void updateDisplay(const char* text) {
  // Clear the display
  display.clearDisplay();

  // Set text size and color
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Set the text position
  display.setCursor(0, 0);

  // Update the text content based on the function argument
  display.print(text);

  // Display the updated content
  display.display();
}
void GreenLight(){
  if (last_check == true){
    digitalWrite(YelLed, HIGH);
    delay(2000);
    digitalWrite(RedLed, LOW);
    digitalWrite(YelLed, LOW);
  }
  digitalWrite(GreLed, HIGH);
}
void RedLight(){
  if (last_check == false){
    digitalWrite(GreLed, LOW);
    delay(500);
    digitalWrite(GreLed, HIGH);
    delay(500);
    digitalWrite(GreLed, LOW);
    delay(500);
    digitalWrite(GreLed, HIGH);
    delay(500);
    digitalWrite(GreLed, LOW);
    delay(500);
    digitalWrite(GreLed, HIGH);
    delay(500);
    digitalWrite(GreLed,LOW);
    digitalWrite(YelLed,HIGH);
    delay(1200);
    digitalWrite(YelLed,LOW);
  }
  digitalWrite(RedLed, HIGH);
}
void TrafficLights(){
  check_obstacle();
  if (obstacle_detect == false){
    GreenLight();
      tl = 1;
      client.publish("esp32/trafficlights", "green");

  }
  else {
    RedLight();
    tl = 0;
    client.publish("esp32/trafficlights", "red");
  }

}
void check_temp(){
  float temp = dht_sensor.readTemperature();
  float humidity = dht_sensor.readHumidity();
  delay(1000);
  sprintf(tempString, "%.2f", temp);
  sprintf(humidityString, "%.2f", humidity);
    // Concatenate the formatted strings with the original string
  sprintf(conditionsString, "Temperature: %s\nHumidity:%s", tempString, humidityString);
}

//// CREATING TEMP AND HUM PANEL

void loop() {
  check_temp();
  updateDisplay(conditionsString);
  TrafficLights();
  client.loop();
  delay(100);

  // Create a JSON object
  StaticJsonDocument<256> doc;

  // Add temperature, humidity, traffic_lights_status, and timestamp fields to the JSON object
  doc["temperature"] = dht_sensor.readTemperature();
  doc["humidity"] = dht_sensor.readHumidity();
  doc["traffic_lights_status"] = tl;
  doc["timestamp"] = millis();

// Get current time from system clock in milliseconds since epoch
  // Serialize the JSON object to a buffer
  char jsonBuffer[256];
  serializeJson(doc, jsonBuffer);

  // Publish the JSON string to the MQTT topic
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);

  delay(2500); // Publish every 2 seconds
}

