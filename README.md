# 🌱 Smart Plant Pot

An ESP32 plant watering project that brings soil moisture monitoring, Home Assistant, and custom electronics together—with automatic watering and a 3D-printable pot planned next.

**ESP32 · ESP-IDF · PlatformIO · MQTT · Home Assistant · KiCad**

[Firmware setup](firmware/README.md) · [Schematic](#schematic) · [PCB previews](#pcb-previews) · [KiCad project](pcb/) · [Roadmap](#roadmap)

> **Work in progress:** moisture sensing and MQTT integration are implemented. Pump control and the printable pot are still in development.

## How it works

The ESP32 reads an analog soil moisture sensor on **GPIO 34**, converts calibrated readings to a percentage, and publishes them over Wi-Fi using MQTT. Home Assistant discovers the sensor automatically and displays its moisture level and availability.

The goal is to use those readings to control a water pump and bring the electronics into a single printable pot.

## Project status

| Area | Current progress |
| --- | --- |
| Firmware | C/C++ with ESP-IDF and PlatformIO, targeting an ESP32 Dev Module |
| Moisture sensing | Raw readings, filtering, and dry/wet calibration |
| Home Assistant | MQTT discovery, moisture updates, and device availability |
| Electronics | KiCad schematic and PCB design files in [pcb/](pcb/) |
| Automatic watering | Pump control and watering logic planned |
| Enclosure | 3D-printable pot planned; no models yet |

## Schematic

[![Plant watering system schematic](docs/schematic/pcb.svg)](docs/schematic/pcb.svg)

*Click the preview to open the full-size SVG.*

Open [pcb/pcb.kicad_pro](pcb/pcb.kicad_pro) in KiCad to explore the editable design. GitHub Actions keeps the preview updated when the schematic or project files change on the default branch.

## PCB previews

<p align="center">
  <a href="docs/pcb/3d.png"><img src="docs/pcb/3d.png" alt="Angled 3D render of the PCB with component models" width="400"></a>
</p>

*3D view of the current design. Click to enlarge; components appear where a 3D model is assigned.*

<table>
  <tr><th>Front</th><th>Back</th></tr>
  <tr>
    <td><a href="docs/pcb/front.svg"><img src="docs/pcb/front.svg" alt="Front PCB copper and silkscreen" width="280"></a></td>
    <td><a href="docs/pcb/back.svg"><img src="docs/pcb/back.svg" alt="Back PCB copper and silkscreen, viewed from underneath" width="280"></a></td>
  </tr>
</table>

*Copper and silkscreen, cropped to the board. Click either preview to enlarge; the back is mirrored to show the underside.*

The [KiCad checks and previews workflow](https://github.com/Excse/plant-watering-system/actions/workflows/schematic.yml) runs **ERC** and **DRC** on pushes and pull requests that change the hardware files. DRC also checks unrouted connections and schematic/PCB consistency. Errors and warnings fail the checks; detailed JSON reports are available in the run's **kicad-reports** artifact, with counts in its summary. Previews update on the default branch even while the design has outstanding violations.

## Getting started

Clone the repository with its library submodule:

```sh
git clone --recurse-submodules git@github.com:Excse/plant-watering-system.git
cd plant-watering-system/firmware
```

Follow the [firmware guide](firmware/README.md) to configure Wi-Fi and MQTT, build and flash the ESP32, wire and calibrate the moisture sensor, and connect it to Home Assistant.

| Directory | Contents |
| --- | --- |
| [firmware/](firmware/) | ESP32 source, PlatformIO configuration, and setup instructions |
| [pcb/](pcb/) | KiCad project, schematic, PCB, and local component libraries |
| [docs/](docs/) | Automatically generated schematic and PCB previews |

## Roadmap

- [x] Set up the ESP32 firmware project.
- [x] Connect Wi-Fi, MQTT, and Home Assistant discovery.
- [x] Implement moisture readings and calibration support.
- [x] Add an automatically generated schematic preview.
- [x] Add compact PCB previews and automated ERC/DRC reports.
- [ ] Validate sensor calibration in the intended soil and pot.
- [ ] Add pump control and automatic watering logic.
- [ ] Build and test the custom electronics.
- [ ] Design and test the 3D-printable plant pot.
- [ ] Document the parts, wiring, and assembly.
