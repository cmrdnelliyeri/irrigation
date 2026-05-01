#include <Wire.h>
#include <Adafruit_ADS1X15.h>

Adafruit_ADS1115 ads;  // create an ADS1115 object
int SENSOR_PWR_PIN = 7;

const int AirValue = 13200;   //you need to replace this value with Value_1
const int WaterValue = 6980;  //you need to replace this value with Value_2
int intervals = (AirValue - WaterValue)/3;
int soilMoistureValue = 0;

void setup() {

  Serial.begin(9600); // open serial port, set the baud rate to 9600 bps
  Wire.begin();
  ads.begin();  // initialize the ADS1115

  pinMode(SENSOR_PWR_PIN, OUTPUT);//Set D7 as an OUTPUT
  digitalWrite(SENSOR_PWR_PIN, HIGH);//Set to LOW so no power is flowing through the sensor

}
void loop() {
  soilMoistureValue = ads.readADC_SingleEnded(0);   //put Sensor insert into soil
  if(soilMoistureValue > WaterValue && soilMoistureValue < (WaterValue + intervals))
  {
    Serial.println("Very Wet");
  }
  else if(soilMoistureValue > (WaterValue + intervals) && soilMoistureValue < (AirValue - intervals))
  {
    Serial.println("Wet");
  }
  else if(soilMoistureValue < AirValue && soilMoistureValue > (AirValue - intervals))
  {
    Serial.println("Dry");
  }
  delay(100);
}