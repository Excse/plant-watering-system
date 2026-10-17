# KiCad libraries

The JLCPCB KiCad library currently in use is installed through KiCad's Plugin
and Content Manager (PCM).

Open [`../pcb.kicad_pro`](../pcb.kicad_pro) in KiCad. The project library tables
[`../sym-lib-table`](../sym-lib-table) and [`../fp-lib-table`](../fp-lib-table)
register the local libraries using `${KIPRJMOD}` paths, relative to `pcb/`.

| Directory | Contents | KiCad library name |
| --- | --- | --- |
| `custom/` | Project-specific `pcb.kicad_sym` and `pcb.pretty/` footprints | `pcb` |
| `easyeda/` | Symbols, footprints, and available 3D models imported from EasyEDA/LCSC | `easyeda2kicad` |

Keep new custom parts in `custom/`. Preserve the `pcb` library name for
project-specific parts. EasyEDA/LCSC components use the `EasyEDA` library;
their 3D models are stored in
`${KIPRJMOD}/libraries/EasyEDA/EasyEDA.3dshapes/`.

The existing `generated/` directory is reserved for generated libraries and is
currently empty and not registered.
The local `JLCPCB-Kicad-Library/` checkout is also not registered in the project;
keep its upstream directory structure intact.

KiCad's standard libraries are provided by the KiCad installation. To add
another local library, put it in a separate directory here and register it in
the appropriate project library table using a `${KIPRJMOD}/libraries/...` path.

## Repair EasyEDA 3D model links

The `EasyEDA/EasyEDA.pretty/` footprints and placed board footprints can contain
links under `${KICAD_3RD_PARTY}/EasyEDA.3dshapes/`. Repair these links to use the
models in `${KIPRJMOD}/libraries/EasyEDA/EasyEDA.3dshapes/`:

```sh
python3 scripts/fix_easyeda_model_links.py --dry-run
python3 scripts/fix_easyeda_model_links.py
```

The script uses Python's standard library and works from any working directory
when invoked by its path. It checks all EasyEDA model targets before writing,
updates the library footprints and all `.kicad_pcb` files directly in `pcb/`,
and preserves model filenames, transforms, and file formatting. Missing models
cause a nonzero exit status without writing any files. Running it again makes
no further changes. Use `--pcb-dir PATH` to repair another copy of the project.

## Import parts with easyeda2kicad

Install [easyeda2kicad](https://github.com/uPesy/easyeda2kicad.py) 1.0.1 or newer
and make its command available on `PATH`, for example with
`pipx install easyeda2kicad`. The helper can also use the package installed in
the Python environment running it.

From the repository root:

```sh
# Import symbols, footprints, and available 3D models for one or several LCSC IDs.
python3 scripts/import_easyeda.py C456012 C2040

# Update an existing part using fresh API data.
python3 scripts/import_easyeda.py C456012 --overwrite --no-cache

# Skip 3D downloads, or preview a command without changing any files.
python3 scripts/import_easyeda.py C456012 --no-3d
python3 scripts/import_easyeda.py C456012 --dry-run
```

Numeric IDs and lowercase `c` prefixes also work. Imports accumulate in
`easyeda/easyeda2kicad.kicad_sym`, `easyeda/easyeda2kicad.pretty/`, and
`easyeda/easyeda2kicad.3dshapes/`. Existing parts require `--overwrite` to replace.
The helper returns a nonzero exit status if any part fails to import.
Each part is converted in a temporary library and checked for conflicts before
being added, so conversion failures and conflicts preserve the existing library.
Identical shared footprints and models are reused; differing files require
`--overwrite`.

The helper always runs the converter from `pcb/`, so it works when invoked from
other directories and generates portable `${KIPRJMOD}/libraries/easyeda/...`
model paths. API downloads are cached in the Git-ignored `pcb/.easyeda_cache/`;
use `--no-cache` to bypass the cache and `--debug` for detailed converter logs.

The `easyeda2kicad` symbol and footprint libraries are already registered in the
project tables. Reopen the KiCad project after the first import to reload them.
