# Hardware checks and previews

The editable KiCad design lives in [pcb/](../pcb/). This directory contains the
generated previews displayed in the [project README](../README.md):

| File | Contents |
| --- | --- |
| [schematic/pcb.svg](schematic/pcb.svg) | Full-size schematic |
| [pcb/front.svg](pcb/front.svg) | Front copper and silkscreen, cropped to the board |
| [pcb/back.svg](pcb/back.svg) | Back copper and silkscreen, mirrored to show the underside |
| [pcb/3d.png](pcb/3d.png) | Angled 3D PCB render; components appear where a model is assigned |

## Automated checks

The [KiCad checks and previews workflow](https://github.com/Excse/plant-watering-system/actions/workflows/kicad.yml)
runs electrical rule checks (**ERC**) and design rule checks (**DRC**) on pushes
and pull requests that change hardware files. It can also be started manually.
DRC checks unrouted connections and schematic/PCB consistency.

Errors and warnings fail the checks; project-configured exclusions are respected.
Each workflow run publishes violation counts in its summary and detailed JSON
reports in the **kicad-reports** artifact, retained for 30 days.

## Preview generation

The workflow uses [scripts/kicad.py](../scripts/kicad.py) with the
`kicad/kicad:10.0.6-full` Docker image for checks and exports.
On pushes and manual runs on the default branch, it generates the schematic,
front and back PCB layers, and a 3D render, then commits changed previews back to
the repository. Pull requests run checks without updating previews.

Previews are generated even when the checks find violations, so a preview does
not indicate that the design has passed ERC or DRC. The
[3D render hash](pcb/3d.sha256) is used to avoid regenerating an unchanged render.

Update the KiCad source files rather than editing generated previews directly.
