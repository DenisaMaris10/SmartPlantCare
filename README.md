# SmartPlantCare
## Description
SmartPlantCare is an Arduino-based automated plant watering system designed to simplify plant care. It is ideal for people who travel, have busy schedules or for office environments where manual watering is inconvenient.
## System Functionality
The system periodically checks whether the plant needs watering based on a predefined time interval. This is achieved using **NTP (Network Time Protocol)** to track elapsed time since the last check. To ensure accuracy, the system updates the real-time clock by reconnecting to the NTP server after each verification.
The system uses:
- **Humidity Sensor** - measures soil moisture through its conductivity and sends data to the Arduino
- **Relay Module** - acts as a switch to control the water pump
- **Water Pump** - supplies water when needed
- **NTP (Network Time Protocol)** - provides the exact real-time clock and ensures accurate scheduling of moisture checks.
If the soil moisture falls below 20%, the relay activates the water pump. The pump runs for 2 seconds before stopping to allow the sensor to reassess the moisture level. If necessary, the cycle repeats until the desired moisture is achieved. Users can also manually trigger watering via an IoT switch in the application.
## Circuit Diagram
![circuit diagram](https://github.com/user-attachments/assets/02d965f5-00b6-4f7a-b03a-4a219a3c8d2b)

## Components Used
- Arduino Uno Rev4 WiFi
- Humidity Sensor
- 1-Channel Relay Module
- Mini Water Pump
- 3x AA Battery Holder (4.5V in series)
- AA Batteries (1.5V each)
- Barrel Jack Cable
## IoT Interface
- Switch: Allows the user to water the plant anytime
