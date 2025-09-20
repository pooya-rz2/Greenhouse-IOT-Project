#include <Wire.h>
#include <BH1750.h>

#define SDA_PIN 21
#define SCL_PIN 22

BH1750 lightMeter;

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN); // SDA, SCL
  Serial.println("Initializing BH1750 sensor...");

  int attempts = 0;
  bool initialized = false;

  while (!initialized && attempts < 5) {
    if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
      initialized = true;
      Serial.println("BH1750 sensor initialized successfully!");
      delay(200); // کوتاه صبر کن تا سنسور آماده داده بشه
    } else {
      attempts++;
      Serial.print("Initialization failed, attempt ");
      Serial.println(attempts);
      delay(1000);
    }
  }

  if (!initialized) {
    Serial.println("BH1750 not found after 5 attempts. Check wiring!");
  }
}

void loop() {
  float lux = lightMeter.readLightLevel();

  if (lux < 0) {
    Serial.println("Error reading BH1750! Retrying read...");
    delay(200); // کمی صبر قبل از تلاش بعدی
  } else {
    Serial.print("Light Intensity: ");
    Serial.print(lux);
    Serial.println(" lx");
  }

  delay(1000);
}

