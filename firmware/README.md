# Watering system firmware

ESP32 Dev Module firmware using PlatformIO and ESP-IDF (C).

## Develop in VS Code

1. Open this `firmware` folder directly in VS Code (the folder containing
   `platformio.ini`).
2. Install the recommended **PlatformIO IDE** and **C/C++** extensions. When
   using WSL or Remote SSH, install them in the remote environment.
3. Run **Developer: Reload Window** from the command palette.
4. Run **PlatformIO: Build**, then **PlatformIO: Rebuild C/C++ Project Index**.

Open `src/main.c`. Completion inside `#include "driver/` should list headers;
Ctrl+Space triggers suggestions, and F12 on `GPIO_NUM_2` opens its definition.
This project uses ESP-IDF headers such as `driver/gpio.h` and
`freertos/FreeRTOS.h`.

Workspace settings select Microsoft C/C++ for IntelliSense and disable clangd
for this project. PlatformIO generates `.vscode/c_cpp_properties.json` with the
compiler, include paths, and defines; do not edit that file manually. Rebuild
the C/C++ project index after changing dependencies or the build environment.

Use the PlatformIO toolbar to build, upload, or open the serial monitor.
Equivalent commands in a PlatformIO terminal are:

```sh
pio run -e esp32dev
pio run -e esp32dev -t upload
pio device monitor -b 115200
```

If completion is still missing, run **C/C++: Reset IntelliSense Database**,
then **PlatformIO: Rebuild C/C++ Project Index**. Check that the language mode
for `main.c` is **C** and the selected C/C++ configuration is **PlatformIO**.
