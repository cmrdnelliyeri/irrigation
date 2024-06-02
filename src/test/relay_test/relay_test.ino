int relay_pin = 10;//PIN for relay control
int count = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  // open serial over USB
  pinMode(relay_pin, OUTPUT);//Set D2 as an OUTPUT
  digitalWrite(relay_pin, LOW);//Set to LOW so no power is flowing through the sensor
}

void loop() {
  // put your main code here, to run repeatedly:
  //Toggle relay after 2s
  switch_relay();

  //Serial print the count to see whether aduino gets reset during the operation. Open serial monitor to observe count
  Serial.println(count++);

}

void switch_relay(){
  digitalWrite(relay_pin, HIGH);
  delay(2000);
  digitalWrite(relay_pin, LOW);
  delay(2000);
}