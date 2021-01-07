/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

#define si7021 0x40 //set humidity sensor address

double Humidity = 0.00;
double Temperature = 0.00;

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

// Replace the next variables with your SSID/Password combination
const char* ssid = "Spark mobile";
const char* password = "87654321z";

// Add your MQTT Broker IP address, example:
//const char* mqtt_server = "192.168.1.144";
const char* mqtt_server = "192.168.43.50";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

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

void setup() {
  Serial.begin(115200);
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);

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
  delay(300);//Wait before accessing Sensor
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {

  Humidity = GetHumid(22, 23);
  updateLCD(Humidity); 
  delay(20);
  alternate();  
  Serial.println("Humidity % RH =" + String(Humidity));
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  /*if (now - lastMsg > 500) {
    lastMsg = now;

    // Convert the value to a char array
    char humString[8];
    dtostrf(Humidity, 1, 2, humString);
    client.publish("esp32/humidity", humString);
  }*/
  // Convert the value to a char array
    char humString[8];
    dtostrf(Humidity, 1, 2, humString);
    client.publish("esp32/humidity", humString);

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
