#include <Wire.h>
#include <Adafruit_ADS1X15.h>

Adafruit_ADS1115 ads;  // create an ADS1115 object

void setup() {
  Serial.begin(9600);
  Wire.begin();
  ads.begin();  // initialize the ADS1115
}

void loop() {
  int16_t raw = ads.readADC_SingleEnded(0);  // read the voltage on channel 0
  float voltage = raw * 0.1875 / 1000;  // convert the raw value to voltage (assuming a gain of 1 and a reference voltage of 3.3V)
  Serial.print("Raw: ");
  Serial.print(raw);
  Serial.print(" Voltage: ");
  Serial.print(voltage, 4);  // print the voltage with 4 decimal places
  Serial.println("V");
  delay(1000);  // wait for 1 second before taking another measurement
}