# Watering system firmware

Experimental firmware for the [Smart Plant Pot](../README.md), written in C and C++ using
ESP-IDF and PlatformIO. The current target is an **ESP32 Dev Module** (`esp32dev`).

The firmware connects to Wi-Fi and MQTT and exposes an LED on **GPIO 2**
to Home Assistant. It also reads an analog moisture sensor on **GPIO 34 (D34)**
and logs raw readings and calibrated percentages over serial. Pump control is
still planned.

## What you need

- An ESP32 development board compatible with the `esp32dev` target and a USB data cable.
- A Wi-Fi network the ESP32 can join.
- An MQTT broker reachable from that network, plus its credentials if required.
- VS Code with the **PlatformIO IDE** and **C/C++** extensions, or an existing
  PlatformIO Core installation for command-line use.
- Optional: Home Assistant with its MQTT integration connected to the same broker.

The current experiment uses GPIO 2 for the LED. Check your board's pinout if its
onboard LED is connected elsewhere; the pin is set by `LED_GPIO` in
[`src/main.cpp`](src/main.cpp).

## 1. Open the project

Clone this repository, then open its **`firmware` folder** directly in VS Code.
This is the folder containing `platformio.ini`.

Install the recommended extensions when prompted, then run **Developer: Reload
Window** from the command palette. With WSL or Remote SSH, install the extensions
in the remote environment and make sure that environment can access the board's
USB serial port.

Open a **PlatformIO Core CLI** terminal from PlatformIO in VS Code. Run all commands
below from the `firmware` directory. If your terminal starts at the repository root:

```sh
cd firmware
```

PlatformIO manages the ESP-IDF framework and toolchain for this project. Its first
run may take a while to download and install dependencies. See the
[PlatformIO ESP-IDF guide](https://docs.platformio.org/en/latest/frameworks/espidf.html)
for framework setup details.

## 2. Configure Wi-Fi and MQTT

Open the interactive ESP-IDF configuration menu through PlatformIO:

```sh
pio run -e esp32dev -t menuconfig
```

Select **Plant Watering System Configuration** and fill in these settings:

| Setting         | What to enter                                                                                                       |
| --------------- | ------------------------------------------------------------------------------------------------------------------- |
| Wi-Fi SSID      | Your wireless network name.                                                                                         |
| Wi-Fi password  | Your wireless network password.                                                                                     |
| MQTT broker URI | Your broker's address, for example `mqtt://192.168.1.100:1883`. Replace the existing default with your own address. |
| MQTT username   | Your broker username, or leave empty if authentication is not required.                                             |
| MQTT password   | Your broker password, or leave empty if authentication is not required.                                             |

Use the arrow keys to navigate and Enter to open or edit an item. Use the menu's
**Save** action, keep the suggested configuration filename, then **Exit**.

The broker address must be reachable **from the ESP32**. Use the broker machine's
LAN address or a resolvable hostname; `localhost` would refer to the ESP32 itself.

The configuration options are defined in
[`src/Kconfig.projbuild`](src/Kconfig.projbuild). Your saved values live in
`sdkconfig.esp32dev`, which is ignored by Git along with its backups. This file
contains your credentials, so keep it local. Settings are compiled into the
firmware: after changing them, build and upload again.

## 3. Build and flash

Build the firmware:

```sh
pio run -e esp32dev
```

Connect the ESP32 using a USB data cable, then upload:

```sh
pio run -e esp32dev -t upload
```

If PlatformIO cannot choose the correct serial port, list the available devices
and specify the port explicitly:

```sh
pio device list
pio run -e esp32dev -t upload --upload-port /dev/ttyUSB0
```

Replace `/dev/ttyUSB0` with your board's port, such as `/dev/ttyACM0` on Linux,
`/dev/cu.usbserial-...` on macOS, or `COM3` on Windows.

## 4. Check the serial output

Start the serial monitor at the baud rate configured in `platformio.ini`:

```sh
pio device monitor -b 115200
```

To select a port explicitly:

```sh
pio device monitor -b 115200 --port /dev/ttyUSB0
```

Press the board's reset button if you want to see the startup messages again.
A successful connection should include these messages, with log prefixes and
additional details:

```text
Startup..
Connecting to Wi-Fi...
Got IP: ...
Network ready
MQTT_EVENT_CONNECTED
```

The firmware waits for Wi-Fi before starting MQTT. Exit the monitor with **Ctrl+C**.

## 5. Test the LED

With Home Assistant's MQTT integration connected to the same broker and discovery
enabled, the firmware publishes discovery information for an **ESP32 Blinky**
device with an **LED** light entity. Toggle that entity to control GPIO 2.

You can also test directly with any MQTT client:

| Topic                                         | Purpose / payload                                                                                                  |
| --------------------------------------------- | ------------------------------------------------------------------------------------------------------------------ |
| `esp32/blinky/led/set`                        | Publish exactly `ON` or `OFF` to control the LED.                                                                  |
| `esp32/blinky/led/state`                      | Subscribe to receive reported `ON` / `OFF` state.                                                                  |
| `esp32/blinky/status`                         | Availability: `online`; the broker publishes the configured `offline` last will when it detects a lost connection. |
| `homeassistant/light/esp32_blinky_led/config` | Retained Home Assistant discovery configuration.                                                                   |

The MQTT topics and discovery identifiers are currently fixed in `src/main.cpp`.
Use one board at a time with these defaults, or give each board unique topics and
identifiers before running multiple copies against the same broker.

## Read and calibrate the moisture sensor

Connect the sensor's **analog output (AO)** to **D34 / GPIO 34** and connect
its ground to ESP32 GND. Power it according to its specifications; use 3.3 V
if supported, and ensure its analog output is safe for the ESP32 (never feed
5 V into GPIO 34). A digital threshold output (DO) cannot measure a percentage.

GPIO 34 is ADC1 channel 6 on this ESP32. The driver uses 12-bit readings and
12 dB attenuation; ADC1 works alongside Wi-Fi. See Espressif's
[ADC documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/adc.html)
and [oneshot driver guide](https://docs.espressif.com/projects/esp-idf/en/v5.4/esp32/api-reference/peripherals/adc_oneshot.html).

Build, upload, and open the serial monitor:

```sh
pio run -e esp32dev -t upload
pio device monitor -b 115200
```

Once per second, the firmware logs the average of 32 ADC samples (0–4095).
Readings start even if Wi-Fi is unavailable. Initially, both calibration
settings are zero, so the output looks like this (example only):

```text
I (...) moisture: GPIO34 raw=2874 (uncalibrated)
```

1. Place the probe in your dry reference soil at the intended insertion depth.
   Wait for the readings to settle and record the raw value as **0% (dry)**.
2. Measure the same soil when thoroughly watered and allowed to drain, at the
   same insertion depth. Record the stable raw value as **100% (wet)**.
   Keep the sensor electronics dry.
3. Exit the monitor, then run `pio run -e esp32dev -t menuconfig`.
   Under **Plant Watering System Configuration**, enter the two measurements
   in **Moisture sensor raw reading at 0% (dry)** and
   **Moisture sensor raw reading at 100% (wet)**. Save and exit.
4. Build and upload again, then reopen the monitor. It now shows both values:

```text
I (...) moisture: GPIO34 raw=2100 moisture=50%
```

The conversion is `100 × (raw − dry) / (wet − dry)`, rounded to the nearest
whole percent and clamped to 0–100%. Either endpoint may be larger. Equal
endpoints leave the sensor uncalibrated and keep raw logging enabled. A reading
stuck near 0 or 4095 in both conditions needs a wiring/output-range check before
calibration. Percentages describe your chosen soil reference conditions, not
an absolute volumetric water content measurement. Moisture readings currently
appear in the serial log; they are not published to MQTT.

To run the calibration conversion checks on the host:

```sh
c++ -std=c++11 -Wall -Wextra -Werror -Isrc test/test_moisture_calibration.cpp -o /tmp/pws-moisture-test
/tmp/pws-moisture-test
```

## Developing in VS Code

After the first build, run **PlatformIO: Rebuild C/C++ Project Index**. Open
`src/main.cpp`; Ctrl+Space should offer completions, and F12 on `GPIO_NUM_2` should
open its definition.

Workspace settings select Microsoft C/C++ for IntelliSense and disable clangd.
PlatformIO generates `.vscode/c_cpp_properties.json` with the compiler, include
paths, and defines; do not edit it manually. Rebuild the project index after
changing dependencies or the build environment.

## Troubleshooting

| Problem                                     | What to check                                                                                                                                                                                                                                              |
| ------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `pio` is not found                          | Open a PlatformIO Core CLI terminal in VS Code, or make sure your PlatformIO Core installation is on `PATH`.                                                                                                                                               |
| No serial port appears                      | Check the USB data cable, board connection, and USB serial driver. For WSL or remote development, check USB forwarding to that environment.                                                                                                                |
| Upload fails or the port is busy            | Close other serial monitors, confirm the port with `pio device list`, and check your user's serial port permissions. If the board does not enter the bootloader automatically, hold BOOT while the uploader connects, then release it once writing starts. |
| Wi-Fi keeps reconnecting                    | Check the SSID and password in `menuconfig`, network availability, and signal strength. Build and upload after changing settings.                                                                                                                          |
| Wi-Fi works but MQTT does not connect       | Check the broker URI, credentials, broker listener, and firewall. The ESP32 must be able to reach the broker's port.                                                                                                                                       |
| The LED does not change                     | Check the exact topic and uppercase `ON` / `OFF` payload, then verify that your board has an LED on GPIO 2.                                                                                                                                                |
| Home Assistant does not discover the device | Confirm `MQTT_EVENT_CONNECTED`, verify both use the same broker, and check that MQTT discovery is enabled with the `homeassistant` prefix.                                                                                                                 |
| Includes or completion are missing          | Build once, run **PlatformIO: Rebuild C/C++ Project Index**, then **C/C++: Reset IntelliSense Database** if needed. Check that `main.cpp` uses C++ language mode and the C/C++ configuration is PlatformIO.                                                    |
