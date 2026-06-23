#!/usr/bin/env python3
"""Merge bootloader and PainDrain Intel HEX files for dual-app programming.

Resolves duplicate addresses by letting bootloader win — its TOC2 points
silicon ROM to 0x10000000, which is correct for the bootloader.

Usage:  python merge_hex.py <bootloader_hex> <app_hex> <output_hex>
Output: Combined hex file ready for PSoC Programmer.
"""
import os
import sys


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
    """Calculate Intel HEX checksum."""
    return (-sum(byte_list)) & 0xFF


MAIN_FLASH_START = 0x10000000
MAIN_FLASH_END   = 0x10100000   # exclusive (1 MB)
FLASH_ROW_SIZE   = 512          # PSoC6 programs main flash in 512-byte rows


def row_align_main_flash(mem):
    """Pad every 512-byte main-flash row that contains data out to a full row,
    filling the gaps with 0xFF (the erased state, so this is harmless).

    PSoC6 programs main flash row-by-row and the programmer rejects a row that
    is not exactly 512 bytes ("Data size is not equal to Row size"). Small
    objects such as the slot metadata (256 B) otherwise leave a partial row."""
    rows = set()
    for a in mem:
        if MAIN_FLASH_START <= a < MAIN_FLASH_END:
            rows.add(a - (a % FLASH_ROW_SIZE))
    for r in rows:
        for off in range(FLASH_ROW_SIZE):
            mem.setdefault(r + off, 0xFF)
    return mem


def write_hex(mem, path):
    """Write {address: byte} dict as a clean Intel HEX file."""
    row_align_main_flash(mem)
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


if __name__ == '__main__':
    if len(sys.argv) != 4:
        print("Usage: python merge_hex.py <bootloader_hex> <app_hex> <output_hex>")
        sys.exit(1)

    bootloader_hex = sys.argv[1]
    app_hex = sys.argv[2]
    output_hex = sys.argv[3]

    # Check files exist
    for path, label in [(bootloader_hex, 'Bootloader'), (app_hex, 'App')]:
        if not os.path.exists(path):
            print(f'ERROR: {label} hex not found:\n  {path}', file=sys.stderr)
            sys.exit(1)

    # Load hex files
    boot = load_hex(bootloader_hex)
    app = load_hex(app_hex)

    # Merge: App base, bootloader overwrites at duplicate addresses
    merged = {**app, **boot}

    # Write merged file
    write_hex(merged, output_hex)
    print(f'Bootloader: {len(boot):,} bytes')
    print(f'App:        {len(app):,} bytes')
    print(f'Combined:   {len(merged):,} bytes  ({len(boot)+len(app)-len(merged):,} bytes overlap resolved)')
    print(f'Written:    {output_hex}')
