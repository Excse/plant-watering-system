# Smart Plant Pot

A DIY, 3D-printable smart plant pot that monitors soil moisture and automatically pumps water when the plant needs it.

This is a personal project in its early experimental stage. I'm currently exploring the ESP32 and building the firmware foundations before adding moisture sensing, pump control, and a printable pot design.

## The idea

- Measure soil moisture to determine when watering is needed.
- Control a water pump to water the plant automatically.
- Bring the electronics and watering system together in a 3D-printable plant pot.

## Current progress

The repository currently contains an ESP32 firmware experiment written in C using ESP-IDF and PlatformIO. It connects to Wi-Fi and an MQTT broker, supports Home Assistant MQTT discovery, and lets Home Assistant switch an LED on GPIO 2.

Moisture sensing and automatic watering are not implemented yet, and there are no 3D-printable models in the repository yet.

## Roadmap

- [x] Set up the ESP32 firmware project.
- [x] Experiment with Wi-Fi, MQTT, and Home Assistant integration.
- [ ] Connect and calibrate a soil moisture sensor.
- [ ] Add pump control and automatic watering logic.
- [ ] Design and test the 3D-printable plant pot.
- [ ] Document the parts, wiring, and assembly.

## Firmware

The [firmware/](firmware/) directory contains the current ESP32 experiments. The target board is an **ESP32 Dev Module** (`esp32dev`).

See the [firmware README](firmware/README.md) for development setup, build, upload, and serial monitor instructions.
