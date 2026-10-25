# KiCad project library

Open [`pcb.kicad_pro`](../pcb.kicad_pro) in KiCad 10. Every symbol, footprint
and 3D model used by the design lives in the single project library `custom`,
registered in the project tables [`sym-lib-table`](../sym-lib-table) and
[`fp-lib-table`](../fp-lib-table). `${KIPRJMOD}` resolves to the project's
`pcb/` directory, so these paths work across checkouts.

| Path | Contents |
| --- | --- |
| [`custom/custom.kicad_sym`](custom/custom.kicad_sym) | Symbols |
| [`custom/custom.pretty/`](custom/custom.pretty/) | Footprints |
| [`custom/custom.3dshapes/`](custom/custom.3dshapes/) | 3D models, one per footprint, named after it |

The only external library is KiCad's built-in `power` symbol library
(`+3.3V`, `GND`, `PWR_FLAG`).

## Where the parts come from

- **KiCad standard library copies** (resistors, capacitors, LEDs, `D_SMA`,
  `SOT-23-3`, `L_Murata_DFE201610P`, the QFN-16 and the JST PH SMD headers) are
  unmodified apart from their 3D model path. Copying freezes them, so KiCad
  library updates cannot change the board.
- **Imported LCSC/EasyEDA parts** were renamed and restyled to the KiCad
  library conventions (0.12 mm silkscreen kept 0.2 mm from pads, F.Fab body
  outline, courtyard 0.25 mm around body and pads, triangle pin-1 marker):
  `USB_C_Receptacle_HX_TYPE-C-6P` and `Texas_VSON-HR-10_2x3mm_P0.5mm`.
- **Project-specific parts**: `ESP-32S_USB-C_CH340_DevBoard` and the generic
  symbols `R`, `C`, `L`, `LED`, `D_Schottky`, `D_TVS`, `Conn_01x02`,
  `Conn_01x03` and the BOM-only `Socket_01x15_BOM`.

The 3D models are STEP files, except for the USB-C receptacle and the JST PH
SMD headers. KiCad has no models for those headers, and the STEP files LCSC
provides for these three parts are not aligned to their footprints, so the
aligned VRML (`.wrl`) models from easyeda2kicad are used instead. Do not add a
`.step` file with the same name next to them: KiCad substitutes it during STEP
export.

## Electrical pin types

The imported symbols use electrical pin types for ERC, and the schematic's
embedded symbol definitions must stay synchronized with the library. Power
inputs/outputs, control inputs, open-drain status outputs and MOSFET gates are
typed according to their functions. Connectors, discrete diodes, MOSFET
source/drain terminals and ordinary passive components remain passive.

Resistor, capacitor and inductor pin numbers are hidden in the schematic and
matching library symbols to keep the drawing readable. Their numbered pins
remain available for netlists and PCB pad mapping. The `LED` symbol uses
pin 1 = cathode and pin 2 = anode, matching KiCad's LED footprints.

Some IC pins intentionally remain passive: TPS63802 L1/L2 are switching-stage
inductor connections. For BQ24075, BAT pin 2 and OUT pin 10 are power outputs;
their internally common companion pins 3 and 11 are passive, following KiCad's
standard charger-symbol convention. This represents each common terminal once
as a power source. Both pins of each pair still need to be connected.

Pin-function references: [TPS63802](https://www.ti.com/lit/ds/symlink/tps63802.pdf),
[BQ24075](https://www.ti.com/lit/ds/symlink/bq24075.pdf).

## Add or update a part

- Copy the symbol into `custom.kicad_sym` and the footprint into
  `custom.pretty/`, using KiCad library naming. For a KiCad standard part, copy
  it unchanged; restyle imported footprints to the conventions above.
- Put its 3D model into `custom.3dshapes/`, named after the footprint, and
  reference it as `${KIPRJMOD}/libraries/custom/custom.3dshapes/<name>.step`.
  Check its alignment in the 3D viewer: imported STEP files are not always
  aligned to their footprint.
- Set the symbol's `Footprint` field to `custom:<name>`.

After changing a library footprint, update the placed footprints in PCB Editor
(**Tools → Update Footprints from Library**). Check the board's 3D view and run
the [hardware checks](../../docs/README.md#run-checks-and-previews-locally);
DRC reports footprints that no longer match the library.

The custom `ESP-32S_USB-C_CH340_DevBoard` footprint uses 2.54 mm pin pitch,
1.0 mm holes for the female header sockets and 25.2 mm between the header rows.
Its antenna outline and label are centered between those rows at X = 12.6 mm.
It is excluded from the BOM and position files because the dev board is
plugged into two hand-soldered 1x15 sockets (`J6`, `J7` in the schematic).
Parts may be placed under the dev board: silkscreen and courtyard only enclose
the two sockets (KiCad `PinSocket_1x15` geometry), the free area between them
(21.7 x 39.1 mm, parts up to 8 mm high) is drawn on User.Comments, and a rule
area keeps copper and footprints out from under the antenna. The dev board
outline itself is on F.Fab.
Those sockets have no footprint of their own, so the footprint carries their
3D model twice (KiCad's `PinSocket_1x15_P2.54mm_Vertical`, offset 0 and
25.2 mm). There is no model of the dev board itself: the original
ESP32-C3-DevKitM-1 model was for a different board.
