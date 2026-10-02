# Watering system firmware

Experimental firmware for the [Smart Plant Pot](../README.md), written in C and C++ using
ESP-IDF and PlatformIO. The current target is an **ESP32 Dev Module** (`esp32dev`).

The firmware connects to Wi-Fi and MQTT and exposes an LED on **GPIO 2** and a
soil moisture sensor on **GPIO 34 (D34)** to Home Assistant. It logs raw readings
and calibrated percentages over serial and publishes the percentage over MQTT.
Pump control is still planned.

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

Clone this repository with its submodules, then open its **`firmware` folder** directly in VS Code.
This is the folder containing `platformio.ini`.

```sh
git clone --recurse-submodules git@github.com:Excse/plant-watering-system.git
```

For an existing checkout, run `git submodule update --init --recursive` from the
repository root. The Home Assistant discovery library lives in
[`lib/rhadar`](lib/rhadar) as a submodule of
[Excse/rhadar](https://github.com/Excse/rhadar).

Work on the standalone library under `/home/timo/development/rhadar`. After
pushing a library commit, update the firmware checkout with
`git -C lib/rhadar fetch origin` and `git -C lib/rhadar checkout <commit>`, then
commit the new submodule pointer in the parent repository.

Run the library's host tests independently of the firmware:

```sh
cmake -S lib/rhadar -B /tmp/rhadar-build
cmake --build /tmp/rhadar-build
ctest --test-dir /tmp/rhadar-build --output-on-failure
```

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
enabled, the firmware publishes discovery information for a **PWS pws_<MAC>**
device with an **LED** light entity and a **Soil moisture** sensor. Toggle the LED
entity to control GPIO 2. The moisture sensor displays the calibrated percentage.

You can also test directly with any MQTT client:

| Topic                                         | Purpose / payload                                                                                                  |
| --------------------------------------------- | ------------------------------------------------------------------------------------------------------------------ |
| `pws_<MAC>/led/set` | Publish exactly `ON` or `OFF` to control the LED. |
| `pws_<MAC>/led/state` | Retained `ON` / `OFF` state. |
| `pws_<MAC>/moisture/state` | Retained integer percentage (`0`–`100`), or `None` when unknown. |
| `pws_<MAC>/status` | Availability: `online`; the broker publishes the `offline` last will when it detects a lost connection. |
| `homeassistant/light/pws_<MAC>/config` | Retained LED discovery configuration. |
| `homeassistant/sensor/pws_<MAC>/moisture/config` | Retained soil moisture discovery configuration. |

Replace `<MAC>` with the board's 12 lowercase hexadecimal MAC digits, without
separators. The complete device identifier is printed over serial at MQTT startup.
Each board gets distinct topics and entity identifiers automatically.

Moisture is published every five seconds while connected and immediately on MQTT
connection or reconnection. Both entities share the device's availability topic.
Retained discovery and state messages let Home Assistant restore them after a
restart. Missing or invalid calibration and failed ADC reads are reported as
`None`, which Home Assistant displays as **Unknown** according to its
[MQTT sensor documentation](https://www.home-assistant.io/integrations/sensor.mqtt/#state_topic).

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
an absolute volumetric water content measurement. Raw and filtered readings appear
in the serial log; the calibrated percentage is also published to MQTT and shown
by the discovered Home Assistant soil moisture sensor.

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

## VS Code run profiles

Open `firmware.code-workspace` for the firmware and rhadar host-test debug
profiles. This keeps the custom profiles separate from PlatformIO’s generated
`.vscode/launch.json`. **Ctrl+Shift+B** builds the ESP32 firmware.

Use **Terminal → Run Task** for firmware build, upload, serial monitoring,
upload-and-monitor, clean, and ESP-IDF configuration. The `rhadar` tasks provide
Debug, Release, and shared-library builds, tests, local installation, clean, and
Doxygen documentation. Host tests run on your computer and require no ESP32.
The firmware debug profile uses your configured PlatformIO debug probe.
