# NilaOpti

NilaOpti is a water pH and temperature monitoring project using ESP-NOW with time synchronization implementation. This project employs a one-to-many method, where one master receives data from two slaves, each responsible for collecting water pH and temperature data. The master will display the current conditions based on the data received from the two slaves.

## Features

- **Water pH Monitoring**: Utilizes a pH sensor connected to one of the slaves to measure the water's acidity level.
- **Water Temperature Monitoring**: Utilizes a temperature sensor connected to the other slave to measure the water's temperature.
- **Time Synchronization**: Implements time synchronization to ensure the data received and displayed has an accurate timestamp.
- **ESP-NOW Communication**: Uses the ESP-NOW communication protocol for data transmission from slaves to the master with low latency.

## Required Components

- 1x ESP32 as Master
- 2x ESP32 as Slaves
- 1x pH Sensor
- 1x Temperature Sensor
- Wires and connectors
- Breadboard (optional)
- WiFi Module (built-in on ESP32)

## Installation and Setup

### 1. Hardware Preparation

1. Connect the pH sensor to one ESP32 Slave.
2. Connect the temperature sensor to the other ESP32 Slave.
3. Ensure both slaves are powered and ready for communication.

### 2. ESP32 Master Configuration

1. Download and install [Arduino IDE](https://www.arduino.cc/en/Main/Software) if not already installed.
2. Add support for ESP32 in Arduino IDE via the Boards Manager.
3. Download and open the `NilaOpti` project from this repository.
4. Open the `Master.ino` file and upload it to the ESP32 Master.

### 3. ESP32 Slave Configuration

1. Open the `Slave_pH.ino` file and upload it to the ESP32 Slave connected to the pH sensor.
2. Open the `Slave_Temperature.ino` file and upload it to the ESP32 Slave connected to the temperature sensor.

### 4. Time Synchronization

1. Ensure all devices are connected to the same WiFi network for time synchronization.
2. Set up NTP (Network Time Protocol) on the master for automatic time synchronization.

## Usage

1. Power on all devices (Master and both Slaves).
2. The master will start receiving data from both Slaves.
3. The received data will be displayed on the Arduino IDE serial monitor connected to the Master.
4. Monitor the water pH and temperature conditions in real-time via the serial monitor.

## License

This project is licensed under the MIT License. See the `LICENSE` file for more details.

---

If you have any further questions, please feel free to contact us. Happy monitoring!
