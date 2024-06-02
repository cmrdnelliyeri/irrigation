int relay_pin = 10;//PIN for relay control
// int raw_moisture_sensor_val = 0; //value for storing raw moisture value
// int smooth_moisture_sensor_val = 0; //value for storing filtered moisture value
float lpf_beta = 0.015; // 0<ß<1
int soil_pin = A0;//Declare a variable for the soil moisture sensor
int soil_power = 7;//Variable for Soil moisture Power
//Rather than powering the sensor through the 3.3V or 5V pins,
//we'll use a digital pin to power the sensor. This will
//prevent corrosion of the sensor as it sits in the soil.
unsigned long previous_millis = 0;
// int pump_current_millis = 0;

int read_soil_moisture();
int get_filtered_moisture_value(int raw_moisture_sensor_value, int previous_filtered_value);
void run_pump();
bool power_on_moisture_sensor();
bool power_off_moisture_sensor();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  // open serial over USB
  pinMode(relay_pin, OUTPUT);//Set D2 as an OUTPUT
  digitalWrite(relay_pin, LOW);//Set to LOW so no power is flowing through the sensor
  pinMode(soil_power, OUTPUT);//Set D7 as an OUTPUT
  digitalWrite(soil_power, LOW);//Set to LOW so no power is flowing through the sensor
  previous_millis = millis();
}

void loop() {

  // put your main code here, to run repeatedly:
  // Pseudocode
  //1. check moisture sensor once in 60 mins
  //2. if moisture value < 800
  //3. switch on pump for 20 seconds

    
  if(millis() - previous_millis > 3600000){
    //60mins = 3600000 milliseconds

    Serial.print("duration > 60 mins, duration = ");
    Serial.println(millis() - previous_millis);

    //Power on moisture sensor
    int filtered_moisture_value = 0;
    bool moisture_sensor_on = power_on_moisture_sensor();

    if (moisture_sensor_on){

      Serial.println("Inside action");
      // Check moisture sensor values for the count of 100

      int count = 0;      

      while(count < 300){
        int raw_moisture_value = read_soil_moisture();
        // Serial.print("raw value: ");
        // Serial.println(raw_moisture_value);
        filtered_moisture_value = get_filtered_moisture_value(raw_moisture_value, filtered_moisture_value);
        // Serial.print("filtered value: ");
        // Serial.println(filtered_moisture_value);
        count++;
      }
        
    }

    bool moisture_sensor_off = power_off_moisture_sensor();

    Serial.print("filtered_moisture_value: ");
    Serial.println(filtered_moisture_value);

    if (filtered_moisture_value < 600){

      //pump on for 20 seconds
      run_pump();

    }else{
      Serial.print("moisture value = ");
      Serial.print(filtered_moisture_value);
      Serial.println(" which is more than threshold value. No watering required !");
    }

    previous_millis = millis();

  }
    
}

bool power_on_moisture_sensor(){

  digitalWrite(soil_power, HIGH);//turn D7 "On"
  unsigned long moisture_current_millis = millis();
  do{
    //Do nothing
  }while((millis()-moisture_current_millis)<10);
  // wait 10 milliseconds  
  return true;

}

bool power_off_moisture_sensor(){

  digitalWrite(soil_power, LOW);//turn D7 "On"
  return true;
  
}

//This is a function used to get the soil moisture content
int read_soil_moisture()
{
  int raw_moisture_value = analogRead(soil_pin);//Read the SIG value form sensor    
  return raw_moisture_value;//send current moisture value
}

int get_filtered_moisture_value(int raw_moisture_sensor_value, int previous_filtered_value){

  int smooth_moisture_sensor_val = previous_filtered_value - (lpf_beta * (previous_filtered_value - raw_moisture_sensor_value));
  return smooth_moisture_sensor_val;

}

void run_pump(){

  unsigned long pump_current_millis = 0;
  pump_current_millis = millis();
  
  do{
    digitalWrite(relay_pin, HIGH);
    Serial.println("pump on");
    Serial.print("millis: ");
    Serial.println(millis());
    Serial.print("pump_current_millis: ");
    Serial.println(pump_current_millis);
    Serial.print("difference: ");
    Serial.println(millis()- pump_current_millis);
  }while((millis()-pump_current_millis)<10000);
  digitalWrite(relay_pin, LOW);
  Serial.println("pump off");

}
