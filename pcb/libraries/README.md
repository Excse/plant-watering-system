# KiCad project libraries

Open [`pcb.kicad_pro`](../pcb.kicad_pro) in KiCad 10. The project library tables
[`sym-lib-table`](../sym-lib-table) and [`fp-lib-table`](../fp-lib-table)
register the libraries below. `${KIPRJMOD}` resolves to the project's `pcb/`
directory, so these paths work across checkouts.

| Directory | Contents | KiCad library name |
| --- | --- | --- |
| [`custom/`](custom/) | `custom.kicad_sym` and `custom.pretty/` for project-specific parts | `custom` |
| [`EasyEDA/`](EasyEDA/) | `EasyEDA.kicad_sym`, `EasyEDA.pretty/` and `EasyEDA.3dshapes/` for imported parts | `EasyEDA` |

KiCad's standard symbols, footprints and 3D models come from the KiCad
installation. Both project libraries above are committed to this repository.
Preserve the directory names and capitalization, including `EasyEDA`.

## Add or update a part

- Store project-specific symbols and footprints in `custom/`, preserving the
  `custom` library name used by the schematic and board.
- Store imported EasyEDA/LCSC symbols, footprints and available models in
  `EasyEDA/`, preserving the `EasyEDA` library name.
- Use `${KIPRJMOD}/libraries/EasyEDA/EasyEDA.3dshapes/<filename>` for imported
  model paths so they resolve on other machines and in CI.
- Register additional libraries in the **project** library tables with
  `${KIPRJMOD}/libraries/...` paths.

After changing a library footprint, update the placed footprints in PCB Editor
as needed. Check the board's 3D view and run the
[hardware checks](../../docs/README.md#run-checks-and-previews-locally).

## Repair EasyEDA 3D model links

[`scripts/fix_links.py`](../../scripts/fix_links.py) replaces legacy
`${KICAD_3RD_PARTY}/EasyEDA.3dshapes/` paths with the project-local model path.
Run it with Python 3.12 or newer from the repository root:

```sh
python3 scripts/fix_links.py --dry-run
python3 scripts/fix_links.py
```

The helper checks EasyEDA library footprints and `.kicad_pcb` files directly in
`pcb/`. It validates referenced EasyEDA models before writing, and preserves
model filenames, transforms and file formatting. A missing model stops the
operation before any files are written. Repeating a successful run makes no
further changes.

When invoking the script by its path, it works from any directory. To operate
on another copy of the KiCad project, add `--pcb-dir PATH`.
