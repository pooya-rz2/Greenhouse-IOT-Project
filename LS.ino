#include <SPI.h>
#include <LoRa.h>

// Define LoRa module pins connected to the ESP32
#define SS 5       // LoRa radio chip select (NSS)
#define RST 14     // LoRa radio reset
#define DIO0 4     // LoRa radio DIO0 (Interrupt)

// SPI pins for the ESP32
#define SCK 18     // LoRa SCK pin
#define MISO 19    // LoRa MISO pin
#define MOSI 23    // LoRa MOSI pin

void setup() {
  // Start the serial monitor for debugging
  Serial.begin(9600);
  while (!Serial);

  // Initialize LoRa
  Serial.println("LoRa Test - Transmitter");

  // Set the SPI and LoRa pins
  LoRa.setPins(SS, RST, DIO0);  // NSS, RESET, DIO0 pin
  
  // Start LoRa communication at 433 MHz
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa initialization failed!");
    while (1);
  } else {
    Serial.println("LoRa initialization successful!");
  }
}

void loop() {
  // Send a packet
  Serial.println("Sending packet...");

  // Start a new packet
  LoRa.beginPacket();
  LoRa.print("Test message from ESP32 with RA-02");
  LoRa.endPacket();  // Send the packet

  // Check for received packet (even without a receiver)
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // Get RSSI (Received Signal Strength Indicator)
    int rssi = LoRa.packetRssi();
    // Get SNR (Signal-to-Noise Ratio)
    float snr = LoRa.packetSnr();
    Serial.print("RSSI: ");
    Serial.print(rssi);
    Serial.print(" SNR: ");
    Serial.println(snr);
  } else {
    Serial.println("No packets received (no receiver)...");
  }

  delay(2000);  // Wait before sending the next packet
}

