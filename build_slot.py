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
import subprocess
import sys

ROOT = os.path.dirname(os.path.abspath(__file__))
CYDSN = os.path.join(ROOT, "PainDrain", "PainDrain.cydsn")
GCC_BIN = r"C:\Program Files (x86)\Cypress\PSoC Creator\4.4\PSoC Creator\import\gnu\arm\5.4.1\bin"
GCC = os.path.join(GCC_BIN, "arm-none-eabi-gcc.exe")
OBJCOPY = os.path.join(GCC_BIN, "arm-none-eabi-objcopy.exe")
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
    0: dict(cm0="0x10010000", cm4="0x10030000", meta="0x10087F00", cm4_addr="0x10030000u"),
    1: dict(cm0="0x10088000", cm4="0x100A8000", meta="0x100FFF00", cm4_addr="0x100A8000u"),
}


def run(args, cwd=CYDSN):
    r = subprocess.run(args, cwd=cwd)
    if r.returncode != 0:
        sys.exit(f"ERROR: {os.path.basename(args[0])} failed ({r.returncode})")


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


def main():
    if len(sys.argv) < 2 or sys.argv[1] not in ("0", "1"):
        sys.exit("Usage: python build_slot.py <0|1> [--version N]")
    n = int(sys.argv[1])
    slot = SLOTS[n]
    version = int(sys.argv[sys.argv.index("--version") + 1]) if "--version" in sys.argv else (n + 1)
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

    out = os.path.join(CM4, f"PainDrain_{tag}.hex")
    subprocess.run([sys.executable, os.path.join(ROOT, "merge_hex.py"),
                    to_hex(cm0_elf), to_hex(cm4_elf), out], check=True)
    print(f"Slot {n} image (version {version}): {out}")


if __name__ == "__main__":
    main()
