#include <Wire.h>
#include <BH1750.h>
#include <DHT.h>

// Pin definitions
#define SOIL_MOISTURE_SENSOR_PIN 34  // ADC pin for soil moisture sensor (GPIO 34)
#define DHTPIN 5                     // DHT22 data pin (GPIO 5)
#define DHTTYPE DHT22                // DHT sensor type

// Calibration values for soil moisture sensor
int dryValue = 3000;  
int wetValue = 950;   

// Sensor objects
BH1750 lightMeter;
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  
  // Initialize the DHT22 sensor
  dht.begin();
  delay(2000);  // Allow sensor to stabilize

  // Initialize I2C communication for BH1750
  Wire.begin(21, 22);  // Use GPIO 21 for SDA and GPIO 22 for SCL
  lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);
}

void loop() {
  // Read sensors
  float temperature = readDHT22Temperature();
  float humidity = readDHT22Humidity();
  float lux = readBH1750();
  float moisturePercentage = readSoilMoisture();

  // Output data in the requested format
  Serial.print(temperature, 2);
  Serial.print(" - ");
  Serial.print(humidity, 2);
  Serial.print(" - ");
  Serial.print(lux, 2);
  Serial.print(" - ");
  Serial.println(moisturePercentage, 2);

  // Delay between each cycle
  delay(5000);
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

