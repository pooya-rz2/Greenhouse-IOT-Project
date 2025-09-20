#include <Wire.h>
#include <BH1750.h>
#include <DHT.h>
#include <SPI.h>
#include <LoRa.h>

// Pin definitions
#define SOIL_MOISTURE_SENSOR_PIN 34  // ADC pin for soil moisture sensor (GPIO 34)
#define DHTPIN 2                     // DHT22 data pin (GPIO 2) changed
#define DHTTYPE DHT22                // DHT sensor type

// LoRa module pin definitions for ESP32
#define SS 5       // LoRa radio chip select (NSS)
#define RST 14     // LoRa radio reset
#define DIO0 4     // LoRa radio DIO0 (Interrupt)
#define SCK 18     // LoRa SCK pin
#define MISO 19    // LoRa MISO pin
#define MOSI 23    // LoRa MOSI pin

// Calibration values for soil moisture sensor
int dryValue = 3000;
int wetValue = 950; 

// Sensor objects
BH1750 lightMeter;
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  while (!Serial);

  // Initialize DHT22 sensor
  dht.begin();
  delay(2000);  // Allow sensor to stabilize

  // Initialize BH1750 light sensor
  Wire.begin(21, 22);  // Use GPIO 21 for SDA and GPIO 22 for SCL
  lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);

  // Initialize LoRa
  LoRa.setPins(SS, RST, DIO0);
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa initialization failed!");
    while (1);
  }
  Serial.println("LoRa initialization successful!");
}

void loop() {
  // Read sensors
  float temperature = readDHT22Temperature();
  float humidity = readDHT22Humidity();
  float lux = readBH1750();
  float moisturePercentage = readSoilMoisture();

  // Format sensor data into a single message
  String message = "Temp: " + String(temperature, 2) + "C, " +
                   "Humidity: " + String(humidity, 2) + "%, " +
                   "Light: " + String(lux, 2) + " lx, " +
                   "Soil Moisture: " + String(moisturePercentage, 2) + "%";

  // Print the data to the Serial Monitor
  Serial.println(message);

  // Send sensor data via LoRa
  LoRa.beginPacket();
  LoRa.print(message);
  LoRa.endPacket();  // Send the packet

  // Wait for 15 seconds before sending the next data
  delay(2000);
}

// Function to read temperature from DHT22
float readDHT22Temperature() {
  float temperature = dht.readTemperature();
  if (isnan(temperature)) {
    return 0.0;  // Return 0.0 if failed to read
  }
  return temperature;
}

// Function to read humidity from DHT22
float readDHT22Humidity() {
  float humidity = dht.readHumidity();
  if (isnan(humidity)) {
    return 0.0;  // Return 0.0 if failed to read
  }
  return humidity;
}

// Function to read light level from BH1750
float readBH1750() {
  float lux = lightMeter.readLightLevel();
  if (lux == -1.0) {
    return 0.0;  // Return 0.0 if failed to read
  }
  return lux;
}

// Function to read data from soil moisture sensor
float readSoilMoisture() {
  int sensorValue = analogRead(SOIL_MOISTURE_SENSOR_PIN);
  float moisturePercentage = map(sensorValue, dryValue, wetValue, 0, 100);
  return constrain(moisturePercentage, 0, 100);
}

