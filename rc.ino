#include <SPI.h>
#include <LoRa.h>

void setup() {
  Serial.begin(9600);     // Initialize serial communication
  while (!Serial);        // Wait for serial port to connect
  Serial.println("LoRa Receiver");

  LoRa.begin(433E6);     // Set frequency to 433MHz
}

void loop() {
  int packetSize = LoRa.parsePacket();  // Check for incoming packets
  if (packetSize) {
    String incoming = "";  // Create a string to hold the incoming message
    while (LoRa.available()) {
      incoming += (char)LoRa.read();  // Read the incoming message
    }
    Serial.print("Received: ");
    Serial.println(incoming);  // Print the received message
  }
}

