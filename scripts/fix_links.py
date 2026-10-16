#!/usr/bin/env python3

"""Make EasyEDA 3D model links project-relative."""

from pathlib import Path

import argparse
import re


PCB_DIR = Path(__file__).resolve().parents[1] / "pcb"
FOOTPRINT_DIR = Path("libraries/EasyEDA/EasyEDA.pretty")
MODEL_DIR = Path("libraries/EasyEDA/EasyEDA.3dshapes")

OLD_PREFIX = b"${KICAD_3RD_PARTY}/EasyEDA.3dshapes/"
NEW_PREFIX = b"${KIPRJMOD}/libraries/EasyEDA/EasyEDA.3dshapes/"

MODEL_LINK = re.compile(rb'(?m)^[ \t]*\(model\s+"([^"\r\n]+)"')


def fix_links(data: bytes, pcb_dir: Path) -> tuple[bytes, int]:
    """Rewrite model paths while preserving formatting."""

    changes = 0

    def replace(match: re.Match[bytes]) -> bytes:
        nonlocal changes
        path = match.group(1)

        if path.startswith(OLD_PREFIX):
            filename = path[len(OLD_PREFIX):]
        elif path.startswith(NEW_PREFIX):
            filename = path[len(NEW_PREFIX):]
        else:
            return match.group(0)

        model = pcb_dir / MODEL_DIR / filename.decode("utf-8")
        if not model.is_file():
            raise ValueError(f"model file does not exist: {model}")

        new_path = NEW_PREFIX + filename
        if path == new_path:
            return match.group(0)

        changes += 1
        return match.group(0).replace(path, new_path, 1)

    return MODEL_LINK.sub(replace, data), changes


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--dry-run", action="store_true", help="report changes without writing files")
    parser.add_argument("--pcb-dir", type=Path, default=PCB_DIR, help="KiCad project directory (default: repository's pcb/)")
    args = parser.parse_args(argv)

    pcb_dir = args.pcb_dir.resolve()
    footprints = sorted((pcb_dir / FOOTPRINT_DIR).glob("*.kicad_mod"))

    if not footprints:
        parser.exit(1, f"No EasyEDA footprints found in {pcb_dir / FOOTPRINT_DIR}\n")

    files = footprints + sorted(pcb_dir.glob("*.kicad_pcb"))
    updates = []

    # Validate all models before modifying any files.
    for path in files:
        try:
            data, count = fix_links(path.read_bytes(), pcb_dir)
        except (OSError, ValueError) as error:
            parser.exit(1, f"{path}: {error}\nNo files were changed.\n")

        if count:
            updates.append((path, data, count))

    total = 0
    for path, data, count in updates:
        total += count

        if not args.dry_run:
            try:
                path.write_bytes(data)
            except OSError as error:
                parser.exit(1, f"{path}: {error}\n")

        print(f"{"Would update" if args.dry_run else "Updated"} {path.relative_to(pcb_dir)} ({count} model links)")

    print(f"{"Would fix" if args.dry_run else "Fixed"} {total} model links in {len(updates)} files.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
