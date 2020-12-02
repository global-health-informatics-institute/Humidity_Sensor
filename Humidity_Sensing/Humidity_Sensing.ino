#include <Wire.h>

#define si7021 0x40

double Humidity_In = 0.00;
double Humidity_Out = 0.00;
double Temp_In = 0.00;
double Temp_Out = 0.00;

const int MeasureHumid = 0xE5;
const int MeasureTemp = 0xE3;

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
  delay(100);
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
}


void loop() {

  Humidity_In = GetHumid(21, 22);
  Humidity_Out = GetHumid(26, 27);
  
  Serial.print("Humidity Inside % RH =" + String(Humidity_In ));
  Serial.println(" ,Humidity Outside % RH =" + String(Humidity_Out ));  
  delay(1000);
}
