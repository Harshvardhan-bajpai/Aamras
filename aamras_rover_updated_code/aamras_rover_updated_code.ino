#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <NewPing.h>
#include <SimpleDHT.h>

#define DHTPIN 2
SimpleDHT11 dht11(DHTPIN);

#define SOIL_PIN A0

#define BATTERY_PIN A1
const float MAX_BATTERY_VOLTAGE = 4.2;
const float MIN_BATTERY_VOLTAGE = 3.0;

SoftwareSerial Bluetooth(12, 13);
SoftwareSerial GPS(10, 11);
TinyGPSPlus gps;

#define TRIGGER_PIN A2
#define ECHO_PIN A3
NewPing sonar(TRIGGER_PIN, ECHO_PIN, 200);

#define NPK_RX 4
#define NPK_TX 5
SoftwareSerial npkSerial(NPK_RX, NPK_TX);

int enA = 5;
int in1 = 2;
int in2 = 3;
int enB = 6;
int in3 = 7;
int in4 = 8;

int receiver_pins[] = {A4, A5, A6};
int receiver_values[] = {0, 0, 0};
int res_min = 1100;
int res_max = 1900;
int working_range = 255;
boolean robot_on = false;

void setup() {
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  Serial.begin(9600);
  Bluetooth.begin(9600);
  GPS.begin(9600);
  npkSerial.begin(9600);
}

void loop() {
  handleBluetooth();
  handleGPS();
  obstacleAvoidance();
}

void handleBluetooth() {
  if (Bluetooth.available()) {
    char command = Bluetooth.read();
    if (command == '1') {
      sendSensorData();
    } else if (command == '2') {
      sendBatteryPercentage();
    } else if (command == '3') {
      sendNPKData();
    }
  }
}

void sendSensorData() {
  byte temp = 0, humidity = 0;
  dht11.read(&temp, &humidity, NULL);
  int soilMoisture = analogRead(SOIL_PIN);

  Serial.print("Temp:");
  Serial.print(temp);
  Serial.println("C");
  Serial.print("Humidity:");
  Serial.print(humidity);
  Serial.println("%");
  Serial.print("Soil Moisture:");
  Serial.println(soilMoisture);

  Bluetooth.print("Temp:");
  Bluetooth.print(temp);
  Bluetooth.println("C");
  Bluetooth.print("Humidity:");
  Bluetooth.print(humidity);
  Bluetooth.println("%");
  Bluetooth.print("Soil Moisture:");
  Bluetooth.println(soilMoisture);
}

void sendBatteryPercentage() {
  int analogValue = analogRead(BATTERY_PIN);
  float voltage = analogValue * 5.0 / 1023.0;
  int percentage = map(voltage * 100, MIN_BATTERY_VOLTAGE * 100, MAX_BATTERY_VOLTAGE * 100, 0, 100);
  percentage = constrain(percentage, 0, 100);

  Serial.print("Battery: ");
  Serial.print(percentage);
  Serial.println("%");
  Bluetooth.print("Battery: ");
  Bluetooth.print(percentage);
  Bluetooth.println("%");
}

void sendNPKData() {
  if (npkSerial.available()) {
    String npkData = "";
    while (npkSerial.available()) {
      char c = npkSerial.read();
      npkData += c;
    }
    Serial.print("NPK Sensor Data: ");
    Serial.println(npkData);
    Bluetooth.print("NPK Sensor Data: ");
    Bluetooth.println(npkData);
  }
}

void handleGPS() {
  while (GPS.available()) {
    gps.encode(GPS.read());
    if (gps.location.isUpdated()) {
      Serial.print("Lat: "); Serial.println(gps.location.lat(), 6);
      Serial.print("Lon: "); Serial.println(gps.location.lng(), 6);
      Bluetooth.print("Lat: "); Bluetooth.println(gps.location.lat(), 6);
      Bluetooth.print("Lon: "); Bluetooth.println(gps.location.lng(), 6);
    }
  }
}

void obstacleAvoidance() {
  int distance = sonar.ping_cm();
  if (distance > 0 && distance < 30) {
    stopMotors();
  }
}

void stopMotors() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}
