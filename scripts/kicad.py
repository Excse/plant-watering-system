#!/usr/bin/env python3

"""Generate documentation previews or run KiCad checks (requires KiCad 10)."""

from collections import Counter
from pathlib import Path

import xml.etree.ElementTree as ET
import subprocess
import argparse
import hashlib
import shutil
import json
import os
import re


def run(*args):
    return subprocess.run(["kicad-cli", *args], check=False).returncode


def normalize_svg(path, title, background=False):
    svg = path.read_text()
    svg = re.sub(r"<title>.*?</title>", f"<title>{title}</title>", svg, count=1)
    if background:
        # A fixed background keeps the copper and silkscreen readable in either
        # GitHub theme. SVGs stay vector-based and fit tightly around the board.
        root = ET.fromstring(svg)
        x, y, width, height = map(float, root.attrib["viewBox"].split())
        margin = max(width, height) * 0.025
        x, y = x - margin, y - margin
        width, height = width + 2 * margin, height + 2 * margin
        svg = re.sub(r'viewBox="[^"]+"', f'viewBox="{x} {y} {width} {height}"', svg, count=1)
        # Muted copper colors keep large filled zones from dominating the preview.
        for old, new in {"#C83434": "#356B5E", "#4D7FC4": "#426883",
                         "#F2EDA1": "#F5F3E8", "#000000": "#111827"}.items():
            svg = svg.replace(old, new)
        rect = (f'<rect x="{x}" y="{y}" width="{width}" height="{height}" '
                'fill="#111827" />')
        svg = svg.replace("</desc>", f"</desc>\n{rect}", 1)
    # KiCad emits trailing spaces in SVG markup and multiline style attributes.
    path.write_text(re.sub(r"[ \t]+$", "", svg, flags=re.MULTILINE))


def previews():
    Path("docs/schematic").mkdir(parents=True, exist_ok=True)
    Path("docs/pcb").mkdir(parents=True, exist_ok=True)
    code = run("sch", "export", "svg", "--output", "docs/schematic/",
               "--exclude-drawing-sheet", "--pages", "1", "pcb/pcb.kicad_sch")
    if code:
        return code
    normalize_svg(Path("docs/schematic/pcb.svg"), "Plant watering system schematic")
    for side, layers in [("front", "F.Cu,F.SilkS,Edge.Cuts"),
                         ("back", "B.Cu,B.SilkS,Edge.Cuts")]:
        path = Path(f"docs/pcb/{side}.svg")
        args = ["pcb", "export", "svg", "--output", str(path), "--layers", layers,
                "--mode-single", "--fit-page-to-board", "--exclude-drawing-sheet", "--check-zones"]
        if side == "back":
            args.append("--mirror")
        code = run(*args, "pcb/pcb.kicad_pcb")
        if code:
            return code
        normalize_svg(path, f"PCB {side} view", background=True)
    return render_3d()


def render_3d():
    # Ray tracing varies slightly between runs. Reuse the committed render when
    # its source files and renderer version match to avoid unnecessary commits.
    inputs = [Path(__file__), Path("pcb/pcb.kicad_pcb"), Path("pcb/pcb.kicad_pro")]
    inputs.extend(sorted(Path("pcb").rglob("*.stp")))
    inputs.extend(sorted(Path("pcb").rglob("*.step")))
    inputs.extend(sorted(Path("pcb").rglob("*.wrl")))
    digest = hashlib.sha256(subprocess.check_output(["kicad-cli", "version"]))
    for path in inputs:
        digest.update(str(path).encode() + b"\0" + path.read_bytes() + b"\0")
    fingerprint = digest.hexdigest() + "\n"
    stamp = Path("docs/pcb/3d.sha256")
    if Path("docs/pcb/3d.png").exists() and stamp.exists() and stamp.read_text() == fingerprint:
        print("3D render inputs are unchanged; reusing the existing preview.")
        return 0
    # The full KiCad image supplies standard component models. Project-local
    # models are resolved through ${KIPRJMOD} in the board file.
    os.environ.setdefault("KICAD10_3DMODEL_DIR", "/usr/share/kicad/3dmodels")
    code = run("pcb", "render", "--output", "docs/pcb/3d.png",
               "--width", "1000", "--height", "800", "--background", "transparent",
               "--quality", "high", "--rotate", "325,0,0", "--zoom", "0.95",
               "pcb/pcb.kicad_pcb")
    if code == 0:
        stamp.write_text(fingerprint)
    return code


def checks(output):
    output.mkdir(parents=True, exist_ok=True)
    # Headless KiCad does not populate the global tables on first launch.
    # Install the standard library tables; project-local tables still apply.
    config = Path(os.environ.get("XDG_CONFIG_HOME", str(Path.home() / ".config"))) / "kicad/10.0"
    config.mkdir(parents=True, exist_ok=True)
    for name in ["sym-lib-table", "fp-lib-table"]:
        source = Path("/usr/share/kicad/template") / name
        if not source.exists():
            raise RuntimeError("Checks require the KiCad full image with standard libraries")
        shutil.copyfile(source, config / name)

    summary = ["## KiCad checks", "", "| Check | Errors | Warnings | Result |",
               "| --- | ---: | ---: | --- |"]
    failed = False
    for name, command, source, extra in [
        ("ERC", ["sch", "erc"], "pcb/pcb.kicad_sch", []),
        ("DRC", ["pcb", "drc"], "pcb/pcb.kicad_pcb",
         ["--schematic-parity", "--refill-zones"]),
    ]:
        report = output / f"{name.lower()}.json"
        code = run(*command, "--format", "json", "--output", str(report),
                   "--severity-error", "--severity-warning", "--exit-code-violations",
                   *extra, source)
        failed |= code != 0
        if code not in (0, 5) or not report.exists():
            summary.append(f"| {name} | — | — | Tool failed (exit {code}) |")
            failed = True
            continue
        data = json.loads(report.read_text())
        violations = data.get("violations", []) + data.get("unconnected_items", []) + data.get("schematic_parity", [])
        for sheet in data.get("sheets", []):
            violations.extend(sheet.get("violations", []))
        counts = Counter(item["severity"] for item in violations)
        result = "Passed" if code == 0 else "Violations found"
        summary.append(f"| {name} | {counts['error']} | {counts['warning']} | {result} |")
    summary.extend(["", "Download **kicad-reports** for detailed JSON reports.",
                    "DRC includes unrouted connections and schematic/PCB parity; zones are refilled without saving the board.",
                    "Errors and warnings fail the checks. Project-configured exclusions are respected."])
    (output / "summary.md").write_text("\n".join(summary) + "\n")
    return int(failed)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("command", choices=["previews", "checks"])
    parser.add_argument("--output", type=Path, default=Path("reports/kicad"))
    args = parser.parse_args()
    raise SystemExit(previews() if args.command == "previews" else checks(args.output))
