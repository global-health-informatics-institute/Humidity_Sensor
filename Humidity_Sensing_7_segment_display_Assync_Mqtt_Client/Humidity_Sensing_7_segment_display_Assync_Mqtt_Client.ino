/*
This example uses FreeRTOS softwaretimers as there is no built-in Ticker library
*/
#include <WiFi.h>
#include <Wire.h>
extern "C" {
	#include "freertos/FreeRTOS.h"
	#include "freertos/timers.h"
}
#include <AsyncMqttClient.h>

#define WIFI_SSID "Fadenlauf-2"
#define WIFI_PASSWORD "watchout"

#define MQTT_HOST IPAddress(192, 168, 0, 59)
#define MQTT_PORT 1883

#define si7021 0x40 //set humidity sensor address

double Humidity = 0.00;
double Temperature = 0.00;
long lastMsg = 0;

const int segment1_1 = 5;
const int segment1_2= 18; 
const int segment1_3 =19;
const int segment1_4 =2;
const int segment1_5 =4;
const int segment1_6 =17;
const int segment1_7 =16;
const int segment2_1 =32;
const int segment2_2 =33;
const int segment2_3 =25;
const int segment2_4 =26;
const int segment2_5 =27    ;
const int segment2_6 =12;
const int segment2_7 =14;
const int ground  = 15;
int seg1;
int seg2;

const int MeasureHumid = 0xE5; //Command to sent to the si7021 to read the humidity 

bool WireEnd() {
  unsigned char err;
  err = Wire.endTransmission();
  if( err ) {
    Serial.print("Error: ");
    Serial.println(err);
  }
  return err;
}

double GetHumid(int SDA_Pin, int SLC_pin) {
  unsigned int data[2];
  Wire.begin(SDA_Pin,SLC_pin,10000);
  Wire.beginTransmission(si7021);
    Wire.write(0xF5);
  WireEnd();
  delay(20);
  Wire.requestFrom(si7021, 2);
  if(Wire.available() == 2)
  {
    data[0] = Wire.read();
    data[1] = Wire.read();
  }
  unsigned int temp = ((data[0] << 8) + data[1]);
  float humidity = ((125.0 * temp) / 65536.0) - 6;
  return humidity;
}

AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;

void connectToWifi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void WiFiEvent(WiFiEvent_t event) {
    Serial.printf("[WiFi-event] event: %d\n", event);
    switch(event) {
    case SYSTEM_EVENT_STA_GOT_IP:
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
        connectToMqtt();
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        Serial.println("WiFi lost connection");
        xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
		xTimerStart(wifiReconnectTimer, 0);
        break;
    }
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");

  if (WiFi.isConnected()) {
    xTimerStart(mqttReconnectTimer, 0);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(segment1_1, OUTPUT);
  pinMode(segment1_2, OUTPUT);
  pinMode(segment1_3, OUTPUT);
  pinMode(segment1_4, OUTPUT);
  pinMode(segment1_5, OUTPUT);
  pinMode(segment1_6, OUTPUT);
  pinMode(segment1_7, OUTPUT);
  pinMode(segment2_1, OUTPUT);
  pinMode(segment2_2, OUTPUT);
  pinMode(segment2_3, OUTPUT);
  pinMode(segment2_4, OUTPUT);
  pinMode(segment2_5, OUTPUT);
  pinMode(segment2_6, OUTPUT);
  pinMode(segment2_7, OUTPUT);
  pinMode(ground,    OUTPUT);
  allPinsLOW();

  mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
  wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));

  WiFi.onEvent(WiFiEvent);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);

  connectToWifi();
}

void loop() {

  Humidity = GetHumid(22, 23);
  updateLCD(Humidity); 
  delay(20);
  alternate();  
  Serial.println("Humidity % RH =" + String(Humidity));
  
  long now = millis();
  if (now - lastMsg > 500) {
    lastMsg = now;
    // Convert the value to a char array
    char humString[8];
    dtostrf(Humidity, 1, 2, humString);
    mqttClient.publish("esp32/humidity", 1, true, humString);
  }
  
}

//Function to display temperature on LCD
void updateLCD(int temperature)
{
  
  seg1 = (temperature/10)%10; // Extracting the first digit from temperature
  seg2 = temperature%10; // Extracting the second digit from temperature

  //Print First digit
  if(seg1 == 0){
   digitalWrite(segment1_1, HIGH); 
   digitalWrite(segment1_2, HIGH);
   digitalWrite(segment1_3, HIGH);
   digitalWrite(segment1_4, HIGH);
   digitalWrite(segment1_5, HIGH);
   digitalWrite(segment1_6, HIGH);
   digitalWrite(segment1_7, LOW);
   digitalWrite(ground, LOW);
  }
  else if(seg1 == 1){
   digitalWrite(segment1_1, LOW); 
   digitalWrite(segment1_2, HIGH);
   digitalWrite(segment1_3, HIGH);
   digitalWrite(segment1_4, LOW);
   digitalWrite(segment1_5, LOW);
   digitalWrite(segment1_6, LOW);
   digitalWrite(segment1_7, LOW);
   digitalWrite(ground, LOW);
  }
  else if(seg1 == 2){
   digitalWrite(segment1_1, HIGH); 
   digitalWrite(segment1_2, HIGH);
   digitalWrite(segment1_3, LOW);
   digitalWrite(segment1_4, HIGH);
   digitalWrite(segment1_5, HIGH);
   digitalWrite(segment1_6, LOW);
   digitalWrite(segment1_7, HIGH);
   digitalWrite(ground, LOW);
  }
  else if(seg1 == 3){
   digitalWrite(segment1_1, HIGH); 
   digitalWrite(segment1_2, HIGH);
   digitalWrite(segment1_3, HIGH);
   digitalWrite(segment1_4, HIGH);
   digitalWrite(segment1_5, LOW);
   digitalWrite(segment1_6, LOW);
   digitalWrite(segment1_7, HIGH);
   digitalWrite(ground, LOW);
  }
  else if(seg1 == 4){
   digitalWrite(segment1_1, LOW); 
   digitalWrite(segment1_2, HIGH);
   digitalWrite(segment1_3, HIGH);
   digitalWrite(segment1_4, LOW);
   digitalWrite(segment1_5, LOW);
   digitalWrite(segment1_6, HIGH);
   digitalWrite(segment1_7, HIGH);
   digitalWrite(ground, LOW);
  }
  else if(seg1 == 5){
   digitalWrite(segment1_1, HIGH); 
   digitalWrite(segment1_2, LOW);
   digitalWrite(segment1_3, HIGH);
   digitalWrite(segment1_4, HIGH);
   digitalWrite(segment1_5, LOW);
   digitalWrite(segment1_6, HIGH);
   digitalWrite(segment1_7, HIGH);
   digitalWrite(ground, LOW);
  }
  else if(seg1 == 6){
   digitalWrite(segment1_1, HIGH); 
   digitalWrite(segment1_2, LOW);
   digitalWrite(segment1_3, HIGH);
   digitalWrite(segment1_4, HIGH);
   digitalWrite(segment1_5, HIGH);
   digitalWrite(segment1_6, HIGH);
   digitalWrite(segment1_7, HIGH);
   digitalWrite(ground, LOW);
  }
  else if(seg1 == 7){
   digitalWrite(segment1_1, HIGH); 
   digitalWrite(segment1_2, HIGH);
   digitalWrite(segment1_3, HIGH);
   digitalWrite(segment1_4, LOW);
   digitalWrite(segment1_5, LOW);
   digitalWrite(segment1_6, LOW);
   digitalWrite(segment1_7, LOW);
   digitalWrite(ground, LOW);
  }
  else if(seg1 == 8){
   digitalWrite(segment1_1, HIGH); 
   digitalWrite(segment1_2, HIGH);
   digitalWrite(segment1_3, HIGH);
   digitalWrite(segment1_4, HIGH);
   digitalWrite(segment1_5, HIGH);
   digitalWrite(segment1_6, HIGH);
   digitalWrite(segment1_7, HIGH);
   digitalWrite(ground, LOW);
  }
  else if(seg1 == 9){
   digitalWrite(segment1_1, HIGH); 
   digitalWrite(segment1_2, HIGH);
   digitalWrite(segment1_3, HIGH);
   digitalWrite(segment1_4, HIGH);
   digitalWrite(segment1_5, LOW);
   digitalWrite(segment1_6, HIGH);
   digitalWrite(segment1_7, HIGH);
   digitalWrite(ground, LOW);
  }

  //Print Second digit
  if(seg2 == 0){
   digitalWrite(segment2_1, HIGH); 
   digitalWrite(segment2_2, HIGH);
   digitalWrite(segment2_3, HIGH);
   digitalWrite(segment2_4, HIGH);
   digitalWrite(segment2_5, HIGH);
   digitalWrite(segment2_6, HIGH);
   digitalWrite(segment2_7, LOW);
   digitalWrite(ground, LOW);
  }
  else if(seg2 == 1){
   digitalWrite(segment2_1, LOW); 
   digitalWrite(segment2_2, HIGH);
   digitalWrite(segment2_3, HIGH);
   digitalWrite(segment2_4, LOW);
   digitalWrite(segment2_5, LOW);
   digitalWrite(segment2_6, LOW);
   digitalWrite(segment2_7, LOW);
   digitalWrite(ground, LOW);
  }
  else if(seg2 == 2){
   digitalWrite(segment2_1, HIGH); 
   digitalWrite(segment2_2, HIGH);
   digitalWrite(segment2_3, LOW);
   digitalWrite(segment2_4, HIGH);
   digitalWrite(segment2_5, HIGH);
   digitalWrite(segment2_6, LOW);
   digitalWrite(segment2_7, HIGH);
   digitalWrite(ground, LOW);
  }
  else if(seg2 == 3){
   digitalWrite(segment2_1, HIGH); 
   digitalWrite(segment2_2, HIGH);
   digitalWrite(segment2_3, HIGH);
   digitalWrite(segment2_4, HIGH);
   digitalWrite(segment2_5, LOW);
   digitalWrite(segment2_6, LOW);
   digitalWrite(segment2_7, HIGH);
   digitalWrite(ground, LOW);
  }
  else if(seg2 == 4){
   digitalWrite(segment2_1, LOW); 
   digitalWrite(segment2_2, HIGH);
   digitalWrite(segment2_3, HIGH);
   digitalWrite(segment2_4, LOW);
   digitalWrite(segment2_5, LOW);
   digitalWrite(segment2_6, HIGH);
   digitalWrite(segment2_7, HIGH);
   digitalWrite(ground, LOW);
  }
  else if(seg2 == 5){
   digitalWrite(segment2_1, HIGH); 
   digitalWrite(segment2_2, LOW);
   digitalWrite(segment2_3, HIGH);
   digitalWrite(segment2_4, HIGH);
   digitalWrite(segment2_5, LOW);
   digitalWrite(segment2_6, HIGH);
   digitalWrite(segment2_7, HIGH);
   digitalWrite(ground, LOW);
  }
  else if(seg2 == 6){
   digitalWrite(segment2_1, HIGH); 
   digitalWrite(segment2_2, LOW);
   digitalWrite(segment2_3, HIGH);
   digitalWrite(segment2_4, HIGH);
   digitalWrite(segment2_5, HIGH);
   digitalWrite(segment2_6, HIGH);
   digitalWrite(segment2_7, HIGH);
   digitalWrite(ground, LOW);
  }
  else if(seg2 == 7){
   digitalWrite(segment2_1, HIGH); 
   digitalWrite(segment2_2, HIGH);
   digitalWrite(segment2_3, HIGH);
   digitalWrite(segment2_4, LOW);
   digitalWrite(segment2_5, LOW);
   digitalWrite(segment2_6, LOW);
   digitalWrite(segment2_7, LOW);
   digitalWrite(ground, LOW);
  }
  else if(seg2 == 8){
   digitalWrite(segment2_1, HIGH); 
   digitalWrite(segment2_2, HIGH);
   digitalWrite(segment2_3, HIGH);
   digitalWrite(segment2_4, HIGH);
   digitalWrite(segment2_5, HIGH);
   digitalWrite(segment2_6, HIGH);
   digitalWrite(segment2_7, HIGH);
   digitalWrite(ground, LOW);
  }
  else if(seg2 == 9){
   digitalWrite(segment2_1, HIGH); 
   digitalWrite(segment2_2, HIGH);
   digitalWrite(segment2_3, HIGH);
   digitalWrite(segment2_4, HIGH);
   digitalWrite(segment2_5, LOW);
   digitalWrite(segment2_6, HIGH);
   digitalWrite(segment2_7, HIGH);
   digitalWrite(ground, LOW);
  }  
}

void alternate()
{
   digitalWrite(segment1_1, LOW); 
   digitalWrite(segment1_2, LOW);
   digitalWrite(segment1_3, LOW);
   digitalWrite(segment1_4, LOW);
   digitalWrite(segment1_5, LOW);
   digitalWrite(segment1_6, LOW);
   digitalWrite(segment1_7, LOW);
   digitalWrite(segment2_1, LOW); 
   digitalWrite(segment2_2, LOW);
   digitalWrite(segment2_3, LOW);
   digitalWrite(segment2_4, LOW);
   digitalWrite(segment2_5, LOW);
   digitalWrite(segment2_6, LOW);
   digitalWrite(segment2_7, LOW);
   digitalWrite(ground, HIGH);
}

void allPinsLOW()
{
   digitalWrite(segment1_1, LOW); 
   digitalWrite(segment1_2, LOW);
   digitalWrite(segment1_3, LOW);
   digitalWrite(segment1_4, LOW);
   digitalWrite(segment1_5, LOW);
   digitalWrite(segment1_6, LOW);
   digitalWrite(segment1_7, LOW);
   digitalWrite(segment2_1, LOW); 
   digitalWrite(segment2_2, LOW);
   digitalWrite(segment2_3, LOW);
   digitalWrite(segment2_4, LOW);
   digitalWrite(segment2_5, LOW);
   digitalWrite(segment2_6, LOW);
   digitalWrite(segment2_7, LOW);
   digitalWrite(ground, LOW);
}
