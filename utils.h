#include <Arduino_BuiltIn.h>

#include "certs.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"

#define AWS_IOT_PUBLISH_TOPIC   "esp32/pub"
#define AWS_PUBLISH_TEMP  "esp32/pub/temp"
#define AWS_PUBLISH_HUM "esp32/pub/hum"
#define AWS_PUBLISH_TL  "esp32/pub/tl"
#define AWS_PUBLISH_LED "esp32/pub/led"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"

#define mqttLED 33
#define foodLED 32
#define waterLED 4

#define RedLed 14
#define YelLed 12
#define GreLed 13
#define DHT_SENSOR_PIN 26
#define DHT_SENSOR_TYPE DHT11
#define SensorOut 27
#define SCREEN_WIDTH 128 // OLED display width in pixels
#define SCREEN_HEIGHT 64 // OLED display hight in pixels

WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);

void messageHandler(char* topic, byte* payload, unsigned int length) {
  Serial.print("incoming: ");
  Serial.println(topic);
 
  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  const char* message = doc["message"];
  Serial.println(message);

  if (strcmp(message, "LEDon") == 0) {
    digitalWrite(mqttLED, HIGH); // Turn the LED on
    Serial.println("LED turned on");
  } else if (strcmp(message, "LEDoff") == 0) {
    digitalWrite(mqttLED, LOW); // Turn the LED off
    Serial.println("LED turned off");
  }  
  else if (strcmp(message, "water")==0){
    digitalWrite(waterLED, HIGH); // Turn off the LED
    delay(1500);
    digitalWrite(waterLED, LOW);
  }
  else if (strcmp(message, "food")==0){
    digitalWrite(foodLED, HIGH); // Turn off the LED
    delay(1500);
    digitalWrite(foodLED, LOW);
  } else {
    Serial.println("Invalid message received");
  }

}
void connectAWS() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
 
  Serial.println("Connecting to Wi-Fi");
 
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
 
  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);
 
  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.setServer(AWS_IOT_ENDPOINT, 8883);
 
  // Create a message handler
  client.setCallback(messageHandler);
 
  Serial.println("Connecting to AWS IOT");
 
  while (!client.connect(THINGNAME)) {
    Serial.print(".");
    delay(100);
  }
 
  if (!client.connected()) {
    Serial.println("AWS IoT Timeout!");
    return;
  }
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
  Serial.println("AWS IoT Connected!");
}
void publishMessage(int value, const char* dataType) {
  StaticJsonDocument<200> doc;

  if (strcmp(dataType, "temperature") == 0) {
    doc["temperature"] = value;
    char jsonBuffer[512];
    serializeJson(doc, jsonBuffer);
    client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer); // Publish to temperature_topic
  } else if (strcmp(dataType, "humidity") == 0) {
    doc["humidity"] = value;
    char jsonBuffer[512];
    serializeJson(doc, jsonBuffer);
    client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer); // Publish to humidity_topic
  } else if (strcmp(dataType, "traffic_lights_status") == 0) {
    doc["traffic_lights_status"] = value;
    char jsonBuffer[512];
    serializeJson(doc, jsonBuffer);
    client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer); // Publish to traffic_lights_topic
  } else {
    Serial.println("Invalid data type. Please choose from 'temperature', 'humidity', or 'traffic_lights_status'.");
    return;
  }
}
