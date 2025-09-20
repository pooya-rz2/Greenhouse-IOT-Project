// Define the ADC pin connected to AOUT pin of the soil moisture sensor
#define SOIL_MOISTURE_SENSOR_PIN 34  // ADC pin (GPIO 34)

// Example calibration values (adjust these according to your calibration)
int dryValue = 3000;  // Replace with actual dry calibration value
int wetValue = 950;  // Replace with actual wet calibration value

void setup() {
  // Start the Serial Monitor at 115200 baud rate
  Serial.begin(115200);
  Serial.println("Capacitive Soil Moisture Sensor Test");
}

void loop() {
  // Read the analog value from the sensor
  int sensorValue = analogRead(SOIL_MOISTURE_SENSOR_PIN);

  // Convert the analog value to a percentage
  float moisturePercentage = map(sensorValue, dryValue, wetValue, 0, 100);

  // Ensure the percentage is within 0-100%
  moisturePercentage = constrain(moisturePercentage, 0, 100);

  // Print the moisture percentage to the Serial Monitor
  Serial.print("Soil Moisture Level: ");
  Serial.print(moisturePercentage);
  Serial.println("%");

  // Add a delay to avoid flooding the Serial Monitor
  delay(1000);
}

