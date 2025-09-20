#include <DHT.h>

// Define the pin connected to the DHT22 data pin
#define DHTPIN 14  // Change this to the GPIO pin you are using

// Define the DHT sensor type as DHT22
#define DHTTYPE DHT22

// Initialize the DHT sensor
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // Start the Serial Monitor at 115200 baud rate
  Serial.begin(115200);
  Serial.println("Starting DHT22 sensor setup...");

  // Start the DHT sensor
  dht.begin();
  Serial.println("DHT22 sensor initialized.");
  delay(2000);  // Give some time for the sensor to stabilize

  Serial.println("Setup complete. Beginning to read data from DHT22...");
}

void loop() {
  // Wait a few seconds between measurements
  delay(2000);

  // Attempt to read humidity
  Serial.println("Attempting to read humidity...");
  float humidity = dht.readHumidity();

  // Attempt to read temperature in Celsius
  Serial.println("Attempting to read temperature...");
  float temperature = dht.readTemperature();

  // Debug: Check if readings are valid
  if (isnan(humidity)) {
    Serial.println("Failed to read humidity from DHT sensor!");
  } else {
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
  }

  if (isnan(temperature)) {
    Serial.println("Failed to read temperature from DHT sensor!");
  } else {
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");
  }

  // Debug: Check for both readings
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("One or more reads failed from DHT sensor!");
  } else {
    Serial.println("Both temperature and humidity read successfully.");
  }
}

