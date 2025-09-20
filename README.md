Greenhouse IoT Monitoring System(MohammadMahdi Rezazadeh's bachelor final project, supervisors: DR. Aminian & DR. Shekarian at university of Guilan)
This repository contains the full code and documentation for an IoT project designed to monitor and manage a greenhouse environment. The project is built around an ESP32 microcontroller that collects real-time data from various sensors and logs it to the cloud.

The project is structured into two parallel implementations: a stable, final version for direct cloud logging and a separate branch dedicated to an experimental edge processing architecture.

The Main Project (main branch)
This is the primary implementation of the project, focusing on simplicity and reliability. It is designed to be a self-contained system that directly sends sensor data to a Google Sheet for logging and visualization.

Key Features
Real-Time Data Logging: Collects and logs environmental data every 5 minutes.

Comprehensive Sensor Suite: Measures temperature, humidity, light intensity, and soil moisture.

Google Sheets Integration: Utilizes a Google Apps Script to securely and reliably store all collected data in a spreadsheet.

Fail-Safe Mechanism: Includes a mechanism to handle internet disconnections and trigger a device restart after multiple failed data uploads, ensuring data integrity.

Hardware Components
Microcontroller: ESP32-WROOM

Sensors:

DHT22 (Temperature & Humidity)

BH1750 (Light)

Capacitive Soil Moisture Sensor v2

Getting Started
To use this project, you need to:

Set up your Google Sheets spreadsheet and Apps Script.

Configure your Wi-Fi credentials in the Arduino sketch.

Upload the code to your ESP32 board.

The full details and step-by-step instructions are available in the project's official report, which is also included in this repository.

Test Code Files
In addition to the main project code, this repository includes individual test sketches for each sensor. These files were created for initial setup and calibration, helping to ensure each component functioned correctly before integration. You can find the test codes in the sensor-tests folder.

Edge Processing Demo (edge-processing-demo branch)
This branch contains a parallel project that was developed to test a more advanced, multi-node architecture using edge processing.

The core idea was to demonstrate how data could be collected from multiple sensor nodes, preprocessed locally on a central edge processor, and then sent to a central database.

Demo Functionality
In this demo, a simple sensor node was simulated to count button presses (upcount) and send this data to a local edge processor. Once the data was received, the edge processor sent a notification to the user's phone. This setup proved the feasibility of a local data pipeline.

LoRa Communication Tests
This implementation includes dedicated code for testing the LoRa communication modules (RA-02). You can find separate sketches for a sender node and a receiver node, which were essential for validating wireless data transmission over long distances. The LoRa test codes are located within the lora-tests folder of this branch.

Future Vision
This demo lays the groundwork for a more scalable and intelligent system. In a future implementation, the simple upcount data would be replaced with real sensor readings. The edge processor could then:

Perform preprocessing tasks on the data from multiple nodes.

Aggregate the data into a centralized knowledge base.

Run machine learning models for predictive analysis or automation.

This architecture would be ideal for a large-scale commercial greenhouse, where real-time, local decision-making is critical to avoid delays.

