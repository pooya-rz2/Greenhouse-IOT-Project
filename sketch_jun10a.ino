#include <Wire.h>
#include <BH1750.h>
#include <DHT.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>
#include <esp_task_wdt.h>

#define SOIL_MOISTURE_SENSOR_PIN 34
#define DHTPIN 2
#define DHTTYPE DHT22
#define BUZZER_PIN 15
#define SDA_PIN 21
#define SCL_PIN 22

int dryValue = 3000;
int wetValue = 950;

const char* ssid = "pooya";
const char* password = "73373b12bb";
const char* scriptURL = "https://script.google.com/macros/s/AKfycbwLfL_Q-Uly8e5e-TLvVNW_SCdc_TV8k54wibtCueAVI9kDL856FJGwqHCAjwKryV6F/exec";

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3 * 3600 + 30 * 60;
const int daylightOffset_sec = 0;

BH1750 lightMeter;
DHT dht(DHTPIN, DHTTYPE);

unsigned long lastSendTime = 0;
const unsigned long storeInterval = 300000;

int failedStoreCount = 0;
bool buzzerOnDueToFailure = false;
bool sensorsInitialized = false;
bool restartPending = false;

void buzzOnce() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_PIN, LOW);
}

void signalFailure() {
  digitalWrite(BUZZER_PIN, HIGH);
  buzzerOnDueToFailure = true;
}

void clearFailureSignal() {
  if (buzzerOnDueToFailure) {
    digitalWrite(BUZZER_PIN, LOW);
    buzzerOnDueToFailure = false;
  }
}

String getCurrentTimestamp() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Time not synced, using millis()");
    return String(millis());
  }
  char buf[50];
  strftime(buf, sizeof(buf), "%d/%m/%Y %H:%M:%S", &timeinfo);
  return String(buf);
}

float robustAverage(float readings[], int count, float tolerance) {
  float sum = 0;
  for (int i = 0; i < count; i++) sum += readings[i];
  float avg = sum / count;
  for (int i = 0; i < count; i++) {
    if (abs(readings[i] - avg) > tolerance) return NAN;
  }
  return avg;
}

float sampleSensor(std::function<float()> sensorFunc, float tolerance, const char* label) {
  float readings[3];
  for (int i = 0; i < 3; i++) {
    readings[i] = sensorFunc();
    delay(150);
  }
  float avg = robustAverage(readings, 3, tolerance);
  if (isnan(avg)) {
    Serial.println(String(label) + " samples too far apart");
    return sensorFunc();
  }
  return avg;
}

float readDHT22Temperature() {
  return sampleSensor([](){ return dht.readTemperature(); }, 2.0, "Temp");
}

float readDHT22Humidity() {
  return sampleSensor([](){ return dht.readHumidity(); }, 3.0, "Humidity");
}

float readBH1750() {
  return sampleSensor([](){ return lightMeter.readLightLevel(); }, 15.0, "Lux");
}

float readSoilMoisture() {
  return sampleSensor([](){
    int val = analogRead(SOIL_MOISTURE_SENSOR_PIN);
    float percent = map(val, dryValue, wetValue, 0, 100);
    return constrain(percent, 0, 100);
  }, 5.0, "Moisture");
}

void sendSystemLog(String event, String desc, String error, bool restarted, float temp, float hum, float lux, float moisture) {
  if (WiFi.status() != WL_CONNECTED) return;
  HTTPClient http;
  http.begin(scriptURL);
  http.setConnectTimeout(5000);
  http.setTimeout(10000);
  http.addHeader("Content-Type", "application/json");

  String data = "{";
  data += "\"type\": \"log\",";
  data += "\"timestamp\": \"" + getCurrentTimestamp() + "\",";
  data += "\"event\": \"" + event + "\",";
  data += "\"desc\": \"" + desc + "\",";
  data += "\"error\": \"" + error + "\",";
  data += "\"restarted\": " + String(restarted ? "true" : "false") + ",";
  data += "\"heap\": " + String(ESP.getFreeHeap()) + ",";
  data += "\"millis\": " + String(millis()) + ",";
  data += "\"wifi\": " + String(WiFi.RSSI()) + ",";
  data += "\"temp\": " + String(isnan(temp) ? -999.0 : temp, 2) + ",";
  data += "\"hum\": " + String(isnan(hum) ? -999.0 : hum, 2) + ",";
  data += "\"lux\": " + String(isnan(lux) ? -999.0 : lux, 2) + ",";
  data += "\"moisture\": " + String(moisture, 2);
  data += "}";

  int code = http.POST(data);
  Serial.println("System Log HTTP code: " + String(code));
  Serial.println("System Log Response: " + http.getString());
  http.end();
}

void sendSensorDataToSheet(float temp, float hum, float lux, float moisture, String timestamp, int wifiRSSI) {
  bool sent = false;
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(scriptURL);
    http.setConnectTimeout(10000);
    http.setTimeout(15000);
    http.addHeader("Content-Type", "application/json");

    String postData = "{";
    postData += "\"type\": \"sensor\",";
    postData += "\"timestamp\": \"" + timestamp + "\",";
    postData += "\"temp\": " + String(isnan(temp) ? -999.0 : temp, 2) + ",";
    postData += "\"hum\": " + String(isnan(hum) ? -999.0 : hum, 2) + ",";
    postData += "\"lux\": " + String(isnan(lux) ? -999.0 : lux, 2) + ",";
    postData += "\"moisture\": " + String(moisture, 2) + ",";
    postData += "\"wifi\": " + String(wifiRSSI);
    postData += "}";

    int code = http.POST(postData);
    Serial.println("Sensor HTTP code: " + String(code));
    Serial.println("Sensor Response: " + http.getString());

    if (code >= 200 && code < 400) {
      sent = true;
    }
    http.end();
  }

  if (sent) {
    buzzOnce();
    failedStoreCount = 0;
    clearFailureSignal();
  } else {
    failedStoreCount++;
    if (failedStoreCount >= 4 && !buzzerOnDueToFailure) signalFailure();
    if (failedStoreCount >= 8) {
      sendSystemLog("System Alert", "Too many failures. Restarting.", "Restart Triggered", true, NAN, NAN, NAN, NAN);
      delay(2000);
      ESP.restart();
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  WiFi.begin(ssid, password);
  unsigned long wifiStart = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (millis() - wifiStart > 30000) {
      sendSystemLog("WiFi Connection", "Failed to connect.", "Timeout", false, NAN, NAN, NAN, NAN);
      break;
    }
  }

  delay(1000);
  sendSystemLog("Boot", "Device just powered on", "", true, NAN, NAN, NAN, NAN);

  if (WiFi.status() == WL_CONNECTED) {
    sendSystemLog("WiFi Connection", "Connected.", "", false, NAN, NAN, NAN, NAN);
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    delay(2000);
    sendSystemLog("NTP", "Time synced.", "", false, NAN, NAN, NAN, NAN);
  }

  dht.begin();
  sendSystemLog("Sensor Init", "DHT22 initialized.", "", false, NAN, NAN, NAN, NAN);

  Wire.begin(SDA_PIN, SCL_PIN);
  delay(300);
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    sendSystemLog("Sensor Init", "BH1750 initialized.", "", false, NAN, NAN, NAN, NAN);
  } else {
    sendSystemLog("Sensor Init", "BH1750 not detected.", "I2C NACK", false, NAN, NAN, NAN, NAN);
  }

  esp_task_wdt_deinit();
  esp_task_wdt_config_t twdt_config = {
    .timeout_ms = 10000,
    .idle_core_mask = (1 << 0),
    .trigger_panic = false
  };
  esp_task_wdt_init(&twdt_config);
  esp_task_wdt_add(NULL);

  sensorsInitialized = true;
  sendSystemLog("Setup Complete", "Ready.", "", false, NAN, NAN, NAN, NAN);
}

void loop() {
  esp_task_wdt_reset();

  unsigned long now = millis();
  if (now - lastSendTime >= storeInterval) {
    lastSendTime = now;
    float temperature = readDHT22Temperature();
    float humidity = readDHT22Humidity();
    float lux = readBH1750();
    float moisture = readSoilMoisture();
    String timestamp = getCurrentTimestamp();
    int wifiRSSI = WiFi.RSSI();

    sendSensorDataToSheet(temperature, humidity, lux, moisture, timestamp, wifiRSSI);
    sendSystemLog("Data Send", "Sensor data sent.", "", false, temperature, humidity, lux, moisture);
  }

  delay(10);
}

