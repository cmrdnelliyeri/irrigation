int raw_moisture_sensor_val = 0; //value for storing raw moisture value
int smooth_moisture_sensor_val = 0; //value for storing filtered moisture value
float lpf_beta = 0.025; // 0<ß<1
int soilPin = A0;//Declare a variable for the soil moisture sensor
int soilPower = 7;//Variable for Soil moisture Power
//Rather than powering the sensor through the 3.3V or 5V pins,
//we'll use a digital pin to power the sensor. This will
//prevent corrosion of the sensor as it sits in the soil.

int readSoil();

void setup() {
  Serial.begin(9600);
  // open serial over USB
  pinMode(soilPower, OUTPUT);//Set D7 as an OUTPUT
  digitalWrite(soilPower, LOW);//Set to LOW so no power is flowing through the sensor
}

void loop()
{
  //This 1 second timefrme is used so you can test the sensor and see it change in real-time.
  //For in-plant applications, you will want to take readings much less frequently.
  //get soil moisture value from the function below
  raw_moisture_sensor_val = readSoil();
  // LPF: Y(n) = (1-ß)*Y(n-1) + (ß*X(n))) = Y(n-1) - (ß*(Y(n-1)-X(n)));
  smooth_moisture_sensor_val = smooth_moisture_sensor_val - (lpf_beta * (smooth_moisture_sensor_val - raw_moisture_sensor_val));
  Serial.print(raw_moisture_sensor_val);
  Serial.print("  ");
  Serial.print(smooth_moisture_sensor_val);
  delay(1000);//take a reading every second
}

//This is a function used to get the soil moisture content
int readSoil()
{
  digitalWrite(soilPower, HIGH);//turn D7 "On"
  delay(10);//wait 10 milliseconds
  raw_moisture_sensor_val = analogRead(soilPin);//Read the SIG value form sensor
  digitalWrite(soilPower, LOW);//turn D7 "Off"
  return raw_moisture_sensor_val;//send current moisture value
}