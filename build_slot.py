#!/usr/bin/env python3
"""Build a dual-app slot image of PainDrain by relinking the PSoC Creator build.

Execute-in-place A/B requires PainDrain to be linked once per slot. PSoC Creator
produces the object files (build PainDrain once in the IDE); this script reuses
them, recompiles only the two slot-specific sources with the slot's defines, and
relinks each core for the requested slot.

    Slot 0 (App0): CM0+ 0x10010000  CM4 0x10030000  metadata 0x10087F00
    Slot 1 (App1): CM0+ 0x10088000  CM4 0x100A8000  metadata 0x100FFF00

Output: <CM4 build>/PainDrain_slot<N>.hex  (CM0+ + CM4 + metadata)

Usage:  python build_slot.py <0|1> [--version N]
"""
import os
import re
import struct
import subprocess
import sys
import zlib

# CRC covers [slot_start, metadata) = the whole image excluding the 256-B metadata
# record. Both slots: META - START = 0x77F00. Matches the bootloader + dfu_user.c.
OTA_CRC_LEN = 0x77F00

ROOT = os.path.dirname(os.path.abspath(__file__))
CYDSN = os.path.join(ROOT, "PainDrain", "PainDrain.cydsn")
GCC_BIN = r"C:\Program Files (x86)\Cypress\PSoC Creator\4.4\PSoC Creator\import\gnu\arm\5.4.1\bin"
GCC = os.path.join(GCC_BIN, "arm-none-eabi-gcc.exe")
OBJCOPY = os.path.join(GCC_BIN, "arm-none-eabi-objcopy.exe")
CYMCUELFTOOL = r"C:\Program Files (x86)\Cypress\PDL\3.1.7\tools\win\elf\cymcuelftool.exe"
CM0P = os.path.join(CYDSN, "CortexM0p", "ARM_GCC_541", "Debug")
CM4 = os.path.join(CYDSN, "CortexM4", "ARM_GCC_541", "Debug")

COMMON = ["-g", "-D", "DEBUG", "-D", "CY_CORE_ID=0", "-D", "CY_PSOC_CREATOR_USED=1",
          "-D", "CYBLE_416045_02", "-Wall", "-ffunction-sections", "-ffat-lto-objects",
          "-Og", "-c"]
CM0P_ARCH = ["-mcpu=cortex-m0plus", "-mthumb"]
CM4_ARCH = ["-mcpu=cortex-m4", "-mfloat-abi=softfp", "-mfpu=fpv4-sp-d16", "-mthumb"]

# Base (App0) flash origins as they appear in the committed cy8c6xx7_*.ld.
BASE = dict(cm0="0x10010000", cm4="0x10030000", meta="0x10087F00")
SLOTS = {
    0: dict(cm0="0x10010000", cm4="0x10030000", meta="0x10087F00", cm4_addr="0x10030000u",
            app_id="0", verify_start="0x10010000"),
    1: dict(cm0="0x10088000", cm4="0x100A8000", meta="0x100FFF00", cm4_addr="0x100A8000u",
            app_id="1", verify_start="0x10088000"),
}


def run(args, cwd=CYDSN):
    r = subprocess.run(args, cwd=cwd)
    if r.returncode != 0:
        sys.exit(f"ERROR: {os.path.basename(args[0])} failed ({r.returncode})")


def fw_version():
    """Integer OTA version from version.h (MAJOR*10000 + MINOR*100 + PATCH) -- the
    same formula main_cm4.c's APP_VERSION default uses, so the IDE App0 image and
    these slot images share one monotonic version. Bump version.h per release."""
    txt = open(os.path.join(CYDSN, "version.h"), encoding="utf-8", errors="replace").read()
    def field(name):
        return int(re.search(name + r"\s+(\d+)", txt).group(1))
    return (field("FIRMWARE_VERSION_MAJOR") * 10000
            + field("FIRMWARE_VERSION_MINOR") * 100
            + field("FIRMWARE_VERSION_PATCH"))


def includes(core_dir):
    text = open(os.path.join(core_dir, "SOURCE_C__ARM_GCC_GENERIC.txt"),
                encoding="utf-8", errors="replace").read()
    out = []
    for d in re.findall(r"-I\s+'([^']+)'", text):
        out += ["-I", d]
    return out


def gen_linker(base_name, out_name, slot):
    text = open(os.path.join(CYDSN, base_name), encoding="utf-8", errors="replace").read()
    text = text.replace("ORIGIN = " + BASE["cm0"], "ORIGIN = " + slot["cm0"])
    text = text.replace("ORIGIN = " + BASE["cm4"], "ORIGIN = " + slot["cm4"])
    text = text.replace("ORIGIN = " + BASE["meta"], "ORIGIN = " + slot["meta"])
    # DFU/cymcuelftool symbols: this image's own app id + verify-region start.
    # The fixed __cy_app0/app1_verify_* map constants are NOT swapped.
    text = re.sub(r"__cy_app_id(\s*)=\s*0;",
                  r"__cy_app_id\g<1>= " + slot["app_id"] + ";", text)
    text = re.sub(r"__cy_app_verify_start(\s*)=\s*0x10010000;",
                  r"__cy_app_verify_start\g<1>= " + slot["verify_start"] + ";", text)
    open(os.path.join(CYDSN, out_name), "w", encoding="utf-8").write(text)


def relink(core_dir, base_obj, new_obj, base_ld, new_ld, tag):
    opt = open(os.path.join(core_dir, "LINK_options.txt"),
               encoding="utf-8", errors="replace").read().strip()
    for needed in (base_obj, "-T " + base_ld, "_link.elf"):
        if needed not in opt:
            sys.exit(f"ERROR: '{needed}' missing in {core_dir}\\LINK_options.txt")
    opt = opt.replace("\\" + base_obj + '"', "\\" + new_obj + '"')
    opt = opt.replace("-T " + base_ld, "-T " + new_ld)
    opt = opt.replace("_link.elf", "_" + tag + "_link.elf")
    opt = opt.replace("PainDrain.map", "PainDrain_" + tag + ".map")
    resp = os.path.join(core_dir, "LINK_options_" + tag + ".txt")
    open(resp, "w", encoding="utf-8").write(opt)
    elf = re.search(r'-o "([^"]*_link\.elf)"', opt).group(1)
    run([GCC, "@" + resp])
    return elf if os.path.isabs(elf) else os.path.join(CYDSN, elf)


def to_hex(elf):
    h = elf.replace(".elf", ".hex")
    run([OBJCOPY, "-O", "ihex", elf, h])
    return h


def patch_cyacd2_crc(cyacd2_path, slot_start):
    """Reconstruct the slot image from its .cyacd2, CRC-32 (zlib/IEEE) over
    [slot_start, metadata), and write {size, crc} into the metadata record in place
    so the bootloader (CRC-at-boot) and the DFU Verify step can check integrity.
    .cyacd2 rows are ':' + 4-byte little-endian address + 512 data bytes (no
    per-row checksum), so the metadata bytes patch in place. Also strips the
    work-flash (Em_EEPROM) rows so an OTA preserves user presets/configs. Returns
    the CRC."""
    SIZE = 0x78000
    meta_addr = slot_start + OTA_CRC_LEN          # the {magic,version,size,crc} record
    meta_row = meta_addr & ~0x1FF                 # 512-aligned row that contains it
    raw = open(cyacd2_path, encoding="utf-8").read().splitlines(keepends=True)
    buf = bytearray(b"\xFF" * SIZE)
    meta_i = None
    for i, line in enumerate(raw):
        s = line.strip()
        if not s.startswith(":"):
            continue
        a = s[1:9]
        addr = int(a[6:8] + a[4:6] + a[2:4] + a[0:2], 16)   # little-endian
        if slot_start <= addr < slot_start + SIZE:
            data = bytes.fromhex(s[9:])
            buf[addr - slot_start: addr - slot_start + len(data)] = data
        if addr == meta_row:
            meta_i = i
    if meta_i is None:
        sys.exit(f"ERROR: metadata row 0x{meta_row:08X} not found in {cyacd2_path}")
    crc = zlib.crc32(bytes(buf[:OTA_CRC_LEN])) & 0xFFFFFFFF
    struct.pack_into("<II", buf, OTA_CRC_LEN + 8, OTA_CRC_LEN, crc)   # size @+8, crc @+12
    off = meta_row - slot_start
    addr_hex = struct.pack("<I", meta_row).hex().upper()
    newline = raw[meta_i][len(raw[meta_i].rstrip("\r\n")):]
    raw[meta_i] = ":" + addr_hex + buf[off:off + 512].hex().upper() + newline

    # Strip work-flash rows (Em_EEPROM @ 0x14000000-0x14007FFF) so an OTA does NOT
    # overwrite persistent user data (presets/configs) stored there. The factory
    # .hex still seeds the Em_EEPROM defaults for a fresh device; OTA leaves it be.
    def _in_workflash(line):
        s = line.strip()
        if not s.startswith(":") or len(s) < 9:
            return False
        h = s[1:9]
        addr = int(h[6:8] + h[4:6] + h[2:4] + h[0:2], 16)
        return 0x14000000 <= addr < 0x14008000
    raw = [ln for ln in raw if not _in_workflash(ln)]

    open(cyacd2_path, "w", encoding="utf-8", newline="").write("".join(raw))
    return crc


def sign(elf):
    """cymcuelftool --sign: CRC the .cy_boot_metadata + write the app signature
    (the same step PSoC Creator runs in its post-build). Returns (elf, hex)."""
    signed = elf.replace("_link.elf", "_signed.elf")
    h = signed.replace(".elf", ".hex")
    run([CYMCUELFTOOL, "--sign", elf, "--output", signed, "--hex", h])
    return signed, h


def main():
    if len(sys.argv) < 2 or sys.argv[1] not in ("0", "1"):
        sys.exit("Usage: python build_slot.py <0|1> [--version N]")
    n = int(sys.argv[1])
    slot = SLOTS[n]
    version = int(sys.argv[sys.argv.index("--version") + 1]) if "--version" in sys.argv else fw_version()
    tag = f"slot{n}"

    if not os.path.exists(os.path.join(CM4, "main_cm4.o")):
        sys.exit("ERROR: build PainDrain once in PSoC Creator first.")

    print(f"== Building slot {n} (version {version}) ==")
    gen_linker("cy8c6xx7_cm0plus.ld", f"cy8c6xx7_cm0plus_{tag}.ld", slot)
    gen_linker("cy8c6xx7_cm4_dual.ld", f"cy8c6xx7_cm4_dual_{tag}.ld", slot)

    run([GCC] + CM0P_ARCH + includes(CM0P) + ["-D", "APP_CM4_ADDR=" + slot["cm4_addr"]] +
        COMMON + [os.path.join(CYDSN, "main_cm0p.c"),
                  "-o", os.path.join(CM0P, f"main_cm0p_{tag}.o")])
    run([GCC] + CM4_ARCH + includes(CM4) + ["-D", f"APP_VERSION={version}u"] +
        COMMON + [os.path.join(CYDSN, "main_cm4.c"),
                  "-o", os.path.join(CM4, f"main_cm4_{tag}.o")])

    cm0_elf = relink(CM0P, "main_cm0p.o", f"main_cm0p_{tag}.o",
                     "cy8c6xx7_cm0plus.ld", f"cy8c6xx7_cm0plus_{tag}.ld", tag)
    cm4_elf = relink(CM4, "main_cm4.o", f"main_cm4_{tag}.o",
                     "cy8c6xx7_cm4_dual.ld", f"cy8c6xx7_cm4_dual_{tag}.ld", tag)

    # Sign each core (CRC metadata + app signature), as PSoC Creator's post-build does.
    cm0_signed, cm0_hex = sign(cm0_elf)
    cm4_signed, cm4_hex = sign(cm4_elf)

    # Combined app hex (both cores) for direct programming / sanity.
    out = os.path.join(CM4, f"PainDrain_{tag}.hex")
    subprocess.run([sys.executable, os.path.join(ROOT, "merge_hex.py"),
                    cm0_hex, cm4_hex, out], check=True)

    # .cyacd2 the DFU host (CySmart / mobile) sends: merge both signed cores into
    # one app elf, then patch it to a cyacd2 covering the whole slot.
    merged = os.path.join(CM4, f"PainDrain_{tag}_merged.elf")
    run([CYMCUELFTOOL, "--merge", cm4_signed, cm0_signed, "--output", merged])
    cyacd2 = os.path.join(CM4, f"PainDrain_{tag}.cyacd2")
    run([CYMCUELFTOOL, "--patch", merged, "--output", cyacd2])

    # Store the integrity CRC in the .cyacd2's metadata so the bootloader/DFU-verify
    # can reject a corrupt or half-written slot. (The .hex above keeps size=0 -> the
    # bootloader boots it on magic alone, which is fine for trusted direct-programming.)
    crc = patch_cyacd2_crc(cyacd2, int(slot["cm0"], 16))

    print(f"Slot {n} image (version {version}):\n  hex:    {out}\n  cyacd2: {cyacd2}"
          f"\n  crc:    0x{crc:08X} over {OTA_CRC_LEN:#x} bytes (stored in slot metadata)")


if __name__ == "__main__":
    main()
