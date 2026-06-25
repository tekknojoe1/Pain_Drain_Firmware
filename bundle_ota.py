#!/usr/bin/env python3
"""Bundle both A/B slot .cyacd2 files + a manifest into one OTA zip for the app.

Execute-in-place A/B (Option A): the mobile app reads the device's running slot
(0 or 1) and uploads the .cyacd2 for the OTHER (inactive) slot. This zip is the
single distributable that carries everything the app needs:

    PainDrain_ota_v<version>.zip
      |- manifest.json            version + slot->file map + how to choose
      |- PainDrain_slot0.cyacd2   image linked for slot 0 (boots at 0x10010000)
      |- PainDrain_slot1.cyacd2   image linked for slot 1 (boots at 0x10088000)

App logic: read running slot R from the device -> upload slots[str(1 - R)].

Run after build_slot.py has produced both slot images (the post-build does this
automatically). Debug-config paths, matching build_slot.py.

Usage: python bundle_ota.py
"""
import json
import os
import re
import sys
import zipfile

ROOT = os.path.dirname(os.path.abspath(__file__))
CYDSN = os.path.join(ROOT, "PainDrain", "PainDrain.cydsn")
CM4 = os.path.join(CYDSN, "CortexM4", "ARM_GCC_541", "Debug")

SLOT_FILES = {0: "PainDrain_slot0.cyacd2", 1: "PainDrain_slot1.cyacd2"}


def fw_version():
    """(integer_version, 'MAJOR.MINOR.PATCH') from version.h -- the single source
    of truth shared with main_cm4.c APP_VERSION and build_slot.py."""
    txt = open(os.path.join(CYDSN, "version.h"), encoding="utf-8", errors="replace").read()
    def field(name):
        return int(re.search(name + r"\s+(\d+)", txt).group(1))
    maj = field("FIRMWARE_VERSION_MAJOR")
    minor = field("FIRMWARE_VERSION_MINOR")
    patch = field("FIRMWARE_VERSION_PATCH")
    build = field("FIRMWARE_VERSION_BUILD")
    # OTA ordering uses MAJOR*10000+MINOR*100+PATCH (build is display-only).
    return (maj * 10000 + minor * 100 + patch, f"{maj}.{minor}.{patch}.{build}")


def main():
    version, fwstr = fw_version()

    for n, fname in SLOT_FILES.items():
        if not os.path.isfile(os.path.join(CM4, fname)):
            sys.exit(f"ERROR: {fname} not found in {CM4} (run build_slot.py {n} first)")

    manifest = {
        "firmware_version": fwstr,                 # human string, e.g. "1.0.1"
        "version": version,                        # integer; matches each image's chooser metadata
        "slots": {str(n): f for n, f in SLOT_FILES.items()},
        "select": "Read the device running slot R (0/1); upload slots[str(1 - R)] (the inactive slot).",
    }
    man_path = os.path.join(CM4, "manifest.json")
    with open(man_path, "w", encoding="utf-8") as f:
        json.dump(manifest, f, indent=2)

    zip_path = os.path.join(CM4, f"PainDrain_ota_v{version}.zip")
    with zipfile.ZipFile(zip_path, "w", zipfile.ZIP_DEFLATED) as z:
        z.write(man_path, "manifest.json")
        for fname in SLOT_FILES.values():
            z.write(os.path.join(CM4, fname), fname)

    print(f"OTA bundle (v{version}): {zip_path}")


if __name__ == "__main__":
    main()
