#include <SoftwareSerial.h>
#include <SimpleDHT.h>
#include <LiquidCrystal.h>

#define DHTPIN 2
SimpleDHT11 dht11(DHTPIN);

#define SOIL_PIN A0
#define BATTERY_PIN A1
const float MAX_BATTERY_VOLTAGE = 4.2;
const float MIN_BATTERY_VOLTAGE = 3.0;

SoftwareSerial Bluetooth(12, 13);
SoftwareSerial npkSerial(4, 5);

LiquidCrystal lcd(7, 6, 3, 4, 5, 8);

void setup() {
  Serial.begin(9600);
  Bluetooth.begin(9600);
  npkSerial.begin(9600);
  lcd.begin(16, 2);
}

void loop() {
  handleBluetooth();
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

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temp);
  lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(humidity);
  lcd.print("%");
  delay(2000);

  lcd.clear();
  lcd.print("Soil Moisture:");
  lcd.setCursor(0, 1);
  lcd.print(soilMoisture);
  delay(2000);

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

  lcd.clear();
  lcd.print("Battery: ");
  lcd.setCursor(0, 1);
  lcd.print(percentage);
  lcd.print("%");
  delay(2000);

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

    lcd.clear();
    lcd.print("NPK Data:");
    lcd.setCursor(0, 1);
    lcd.print(npkData);
    delay(2000);

    Serial.print("NPK Sensor Data: ");
    Serial.println(npkData);
    Bluetooth.print("NPK Sensor Data: ");
    Bluetooth.println(npkData);
  }
}
