#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "pooya";
const char* password = "73373b12bb";
const int buttonPin = 4;
int buttonState = 0;
const char* serverName = "http://192.168.1.63:8080"; 

void setup() {
  
  Serial.begin(115200);

  pinMode(buttonPin, INPUT);
  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());

 

    //http.end(); // Free resources

}
void loop() {

  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) {
    while(buttonState == HIGH){
      buttonState = digitalRead(buttonPin);
      //wait to release
    }
      // Send HTTP POST request
      if (WiFi.status() == WL_CONNECTED) {
          HTTPClient http;
          Serial.print("Connecting to ");
          Serial.println(serverName);
          http.begin(serverName); 
          http.addHeader("Content-Type", "application/x-www-form-urlencoded");
          int httpResponseCode = http.POST("clicked");
        if(httpResponseCode > 0) {
            String payload = http.getString();
            Serial.println("Response from server:");
            Serial.println(payload);
        }else {
            Serial.print("Error on sending POST request: ");
            Serial.println(httpResponseCode);
        }
      }
   }

    delay(100); 
  
}
