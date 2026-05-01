#include <Wire.h>
#include <string.h>
#include <Adafruit_ADS1X15.h>
#include <Adafruit_GFX.h>  // Include core graphics library for the display
#include <Adafruit_SSD1306.h>  // Include Adafruit_SSD1306 library to drive the display
// #include <Fonts/FreeMonoBold12pt7b.h>  // Add a custom font
#include <Fonts/FreeMonoOblique9pt7b.h>

Adafruit_SSD1306 display(128, 64);  // Create display

Adafruit_ADS1115 ads;  // create an ADS1115 object

//Rather than powering the sensor through the 3.3V or 5V pins,
//we'll use a digital pin to power the sensor. This will
//prevent corrosion of the sensor as it sits in the soil.
int SENSOR_PWR_PIN = 7;
int TEST_PIN = 8;
unsigned long previous_millis = 0;
// unsigned long time_difference = 3600000;  // 1 Hour
unsigned long time_difference = 5000;
const int push_button_pin = 2;

const int AirValue = 18049;   //you need to replace this value with Value_1
const int WaterValue = 9746;  //you need to replace this value with Value_2
int intervals = (AirValue - WaterValue)/3;
long soilMoistureValue = 0;

int relay_pin = 10;//PIN for relay control




int read_soil_moisture();
void run_pump(bool button_press=false);
bool power_on_moisture_sensor();
bool power_off_moisture_sensor();
bool check_motor_on_push_button_state();
bool check_test_button_state();
bool idle_state_printer = true;

char sensor_value_string[10];  // Create a character array of 10 characters
String action_value_string = "";

void setup() {  
  // put your setup code here, to run once:
  delay(100);  // This delay is needed to let the display to initialize

  Serial.begin(9600);// open serial over USB

  Wire.begin();
  ads.begin();  // initialize the ADS1115

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // Initialize display with the I2C address of 0x3C
  display.clearDisplay();  // Clear the buffer
  display.setTextColor(WHITE);  // Set color of the text
  display.setRotation(0);  // Set orientation. Goes from 0, 1, 2 or 3
  display.setTextWrap(true);  // By default, long lines of text are set to automatically “wrap” back to the leftmost column.
                               // To override this behavior (so text will run off the right side of the display - useful for
                               // scrolling marquee effects), use setTextWrap(false). The normal wrapping behavior is restored
                               // with setTextWrap(true).
  display.dim(0);  //Set brightness (0 is maximun and 1 is a little dim)
  display.setFont(&FreeMonoOblique9pt7b);  // Set a custom font
  display.setTextSize(0);  // Set text size. We are using a custom font so you should always use the text size of 0

  pinMode(relay_pin, OUTPUT);//Set D2 as an OUTPUT
  digitalWrite(relay_pin, LOW);//Set to LOW so no power is flowing through the sensor

  pinMode(SENSOR_PWR_PIN, OUTPUT);//Set D7 as an OUTPUT
  digitalWrite(SENSOR_PWR_PIN, HIGH);//Set to LOW so no power is flowing through the sensor

  pinMode(push_button_pin, INPUT);
  pinMode(TEST_PIN, INPUT);

  previous_millis = millis();

}

void loop() {

  // put your main code here, to run repeatedly:
  // Pseudocode
  //1. check moisture sensor once in 60 mins
  //2. if moisture value < threshold
  //3. switch on pump for 20 seconds

  // int temp_moisture_val = ads.readADC_SingleEnded(1);
  // Serial.print("raw value: ");
  // Serial.println(temp_moisture_val); 

  if (check_motor_on_push_button_state()){
    run_pump(true);
    return;
  }else{
    digitalWrite(relay_pin, LOW);
  }

  if (check_test_button_state()){
    time_difference = 5000;  //5s = 5000 milliseconds    
  }else{
    time_difference = 3600000; //60mins = 3600000 milliseconds
  }
  // Serial.print("duration = ");
  // Serial.println(time_difference);

  if(millis() - previous_millis > time_difference){    

    // Serial.print("duration > 60 mins, duration = ");
    Serial.println(millis() - previous_millis);

    //Power on moisture sensor
    int filtered_moisture_value = 0;
    bool moisture_sensor_on = power_on_moisture_sensor();
    Serial.println("moisture_sensor_on");

    if (moisture_sensor_on){

      Serial.println("Inside action");
      // Check moisture sensor values for the count of 100

      int count = 0;      
      int temp_moisture_val = 0;
      while(count < 300){
        // Serial.println("checkk");
        // soilMoistureValue = read_soil_moisture();
        // soilMoistureValue = ads.readADC_SingleEnded(0);
        temp_moisture_val = ads.readADC_SingleEnded(1);
        soilMoistureValue = soilMoistureValue + temp_moisture_val;
        Serial.print("raw value: ");
        Serial.println(temp_moisture_val);        
        count++;
      }
        
    }

    bool moisture_sensor_off = power_off_moisture_sensor();

    soilMoistureValue = soilMoistureValue/300;

    Serial.print("average moisture value = ");
    Serial.println(soilMoistureValue);

    if(soilMoistureValue > WaterValue && soilMoistureValue < (WaterValue + intervals))
    {
      Serial.print("moisture value = ");
      Serial.print(soilMoistureValue);
      Serial.println(" very wet. no watering required !");
      // Print text:
      action_value_string = "very wet";
    }
    else if(soilMoistureValue > (WaterValue + intervals) && soilMoistureValue < (AirValue - intervals))
    {
      Serial.print("moisture value = ");
      Serial.print(soilMoistureValue);
      Serial.println(" wet. no watering required !");
      // Print text:
      action_value_string = "wet";
    }
    else if(soilMoistureValue < AirValue && soilMoistureValue > (AirValue - intervals))
    {
      Serial.println("dry, switchin on pump");
      //pump on for 20 seconds
      // Print text:
      action_value_string = "dry, pumping..";
      run_pump();
    }
    else{
        Serial.println("No action performed");
        action_value_string = "no case";
    }
    idle_state_printer = true;
    previous_millis = millis();

  }else{
    if(idle_state_printer){
        Serial.println("controller sleeping");
        idle_state_printer = false;
      }
  }

  display.clearDisplay();
  // Convert float to a string:
  dtostrf(soilMoistureValue, 3, 0, sensor_value_string);  // (<variable>,<amount of digits we are going to use>,<amount of decimal digits>,<string name>)

  display.setCursor(0, 15);  // (x,y)
  display.println("m = ");
  display.setCursor(40, 15);  // (x,y)
  display.println(sensor_value_string);

  display.setCursor(0, 40);  // (x,y)
  display.println(action_value_string); 

  display.display();  // Print everything we set previously
    
}

bool power_on_moisture_sensor(){

  digitalWrite(SENSOR_PWR_PIN, HIGH);//turn D7 "On"
  unsigned long moisture_current_millis = millis();
  do{
    //Do nothing
  }while((millis()-moisture_current_millis)<100);
  // wait 10 milliseconds  
  return true;

}

bool power_off_moisture_sensor(){

  digitalWrite(SENSOR_PWR_PIN, LOW);//turn D7 "On"
  return true;
  
}

//This is a function used to get the soil moisture content
int read_soil_moisture()
{
  int raw_moisture_value = ads.readADC_SingleEnded(0); ;//Read the SIG value form sensor 
  Serial.println(raw_moisture_value);
  return raw_moisture_value;//send current moisture value
}

void run_pump(bool button_press){

  if(button_press){
    digitalWrite(relay_pin, HIGH);
    return;
  }

  unsigned long pump_current_millis = 0;
  pump_current_millis = millis();
  
  do{
    digitalWrite(relay_pin, HIGH);
    // Serial.println("pump on");
    // Serial.print("millis: ");
    // Serial.println(millis());
    // Serial.print("pump_current_millis: ");
    // Serial.println(pump_current_millis);
    // Serial.print("difference: ");
    // Serial.println(millis()- pump_current_millis);
  }while((millis()-pump_current_millis)<10000);
  digitalWrite(relay_pin, LOW);
  Serial.println("pump off");

}

bool check_motor_on_push_button_state(){
  return digitalRead(push_button_pin);
}

bool check_test_button_state(){
  return digitalRead(TEST_PIN);
}