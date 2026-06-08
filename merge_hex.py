#!/usr/bin/env python3
"""Merge App0 (Bootloader) and App1 (PainDrain) Intel HEX files.

Resolves duplicate addresses (e.g. TOC2 at 0x16007C00 present in both) by
letting App0 win — its TOC2 points silicon ROM to 0x10000000, which is correct.

Usage:  python merge_hex.py
Output: combined.hex in the same directory — program via PSoC Programmer.
"""
import os
import sys

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
APP0_HEX = os.path.join(SCRIPT_DIR, 'Bootloader', 'Bootloader_BLE_App101.cydsn',
                         'CortexM0p', 'ARM_GCC_541', 'Debug', 'Bootloader_BLE_App101.hex')
APP1_HEX = os.path.join(SCRIPT_DIR, 'PainDrain', 'PainDrain.cydsn',
                         'CortexM4', 'ARM_GCC_541', 'Debug', 'PainDrain.hex')
OUT_HEX  = os.path.join(SCRIPT_DIR, 'combined.hex')


def load_hex(path):
    """Parse Intel HEX into {byte_address: byte_value}."""
    mem = {}
    base = 0
    with open(path) as f:
        for line in f:
            line = line.strip()
            if not line.startswith(':'):
                continue
            count  = int(line[1:3], 16)
            offset = int(line[3:7], 16)
            rtype  = int(line[7:9], 16)
            if rtype == 0:   # data
                data = bytes.fromhex(line[9:9 + count * 2])
                for i, b in enumerate(data):
                    mem[base + offset + i] = b
            elif rtype == 4: # extended linear address
                base = int(line[9:13], 16) << 16
            elif rtype == 1: # EOF
                break
    return mem


def checksum(byte_list):
    return (-sum(byte_list)) & 0xFF


def write_hex(mem, path):
    """Write {address: byte} dict as a clean Intel HEX file."""
    addrs = sorted(mem.keys())
    with open(path, 'w') as f:
        cur_ela = None
        i = 0
        while i < len(addrs):
            addr = addrs[i]
            ela  = (addr >> 16) & 0xFFFF

            if ela != cur_ela:
                cur_ela = ela
                d = [0x02, 0x00, 0x00, 0x04, (ela >> 8) & 0xFF, ela & 0xFF]
                f.write(f':02000004{ela:04X}{checksum(d):02X}\n')

            # Collect up to 16 contiguous bytes in the same ELA segment
            chunk      = []
            chunk_addr = addr
            while len(chunk) < 16 and i < len(addrs):
                if addrs[i] == chunk_addr + len(chunk) and (addrs[i] >> 16) == ela:
                    chunk.append(mem[addrs[i]])
                    i += 1
                else:
                    break

            ofs = chunk_addr & 0xFFFF
            rec = [len(chunk), (ofs >> 8) & 0xFF, ofs & 0xFF, 0x00] + chunk
            f.write(f':{len(chunk):02X}{ofs:04X}00'
                    + ''.join(f'{b:02X}' for b in chunk)
                    + f'{checksum(rec):02X}\n')

        f.write(':00000001FF\n')


for path, label in [(APP0_HEX, 'App0'), (APP1_HEX, 'App1')]:
    if not os.path.exists(path):
        print(f'ERROR: {label} hex not found:\n  {path}', file=sys.stderr)
        sys.exit(1)

app0 = load_hex(APP0_HEX)
app1 = load_hex(APP1_HEX)

# Merge: App1 base, App0 overwrites at any duplicate address
merged = {**app1, **app0}

write_hex(merged, OUT_HEX)
print(f'App0:     {len(app0):,} bytes')
print(f'App1:     {len(app1):,} bytes')
print(f'Combined: {len(merged):,} bytes  ({len(app0)+len(app1)-len(merged):,} bytes overlap resolved)')
print(f'Written:  {OUT_HEX}')
