# PainDrain Firmware — Claude Context

## Project Overview

Embedded firmware for the PainDrain medical device (PSoC6 BLE).
- **Device**: CY8C6347BZI-BLD53
- **Toolchain**: Cypress PSoC Creator 4.4, GCC
- **Branch**: `dk_ota_updates` — OTA DFU integration in progress

---

## Two-Project OTA Architecture

- **App0 (bootloader)**: `Bootloader/Bootloader_BLE_App101.cydsn/` — CM0+, holds BLE DFU host
- **App1 (PainDrain)**: `PainDrain/PainDrain.cydsn/` — main application

### Flash Memory Layout (must match in both projects)

| Region | Start | Size |
|---|---|---|
| App0 CM0+ | 0x10000000 | 64KB |
| App0 CM4 | 0x10010000 | 192KB |
| App1 CM0+ | 0x10040000 | 200KB |
| App1 CM4 | 0x10072000 | 312KB |
| flash_storage | 0x100C0000 | 4KB |
| flash_boot_meta (DFU) | 0x100FFA00 | 1KB |

Defined in `Bootloader/Bootloader_BLE_App101.cydsn/bootload_common.ld` (force-added to git).

App1 flash starts with a 256-byte header (`.cy_app_header`) containing magic `0xA1B2C3D4` and version 1. Vector table at `0x10040100`.

### RAM Layout

| Region | Start | Size |
|---|---|---|
| ram_common (noinit) | 0x08000000 | 256B |
| App1 CM0+ RAM | 0x08000100 | ~128KB |
| App1 CM4 RAM | 0x08020000 | 128KB |

Boot magic at `0x08000000`; CM4 BLE status at `0x08000004`.

---

## Boot Sequence (CONFIRMED WORKING 2026-05-28)

1. Silicon ROM reads TOC2 at `0x16007C00` → jumps to App0 at `0x10000000`
2. App0 checks `0x10040000` for `APP1_META_MAGIC` (`0xA1B2C3D4`)
3. App0 sets NOINIT magic (`0xA15A5AA1`) in `ram_common` → `NVIC_SystemReset()`
4. App0 second boot: sees magic → `Cy_SysClk_FllDisable()`, sets `VTOR=0x10040100`, inline-asm `MSR MSP / BX` to App1 Reset_Handler
5. App1 startup: `SystemInit` (reconfigures clocks), `main()` → enables CM4, runs HostMain/BLE

### Combined Hex Generation

Post-build in the Bootloader project runs `post_build_core1.bat` then `merge_hex.py`, which merges App0 + PainDrain CM4 hex (App0 wins at duplicate addresses). Result replaces `Bootloader.hex` so the "Program" button programs both images.

**Rebuild order:**
1. Build PainDrain CM4 (also rebuilds CM0+)
2. Build Bootloader (generates combined.hex)
3. Program via Bootloader project "Program" button

---

## BLE Hardware Failure Root Cause & Fix (RESOLVED 2026-06-04)

**Symptom**: `HW_ERR 0x0C`, BLE never advertises after bootloader jump.

**Root cause**: OTA integration switched PainDrain from Default PDL to Custom PDL 3.1.7. This regenerated `cyfitter_cfg.c` to use PILO instead of WCO for LFCLK. The bootloader's `cyfitter_cfg.c` still uses WCO and enables **SIMO Buck**. The bootloader's direct BX-jump to App1 does NOT reset SRSS registers, so SIMO Buck stays enabled. `Cy_BLE_EcoStart()` detects SIMO Buck and enables Buck Out2 (VRF), but **SIMO Buck Out2 is not connected on this PCB** — the BLESS internal LDO supplies the RF section. BLESS never receives RF power → `CY_BLE_ERROR_HARDWARE_FAILURE`.

**Fix**: Patch `Generated_Source/PSoC6/pdl/middleware/ble/cy_ble_clk.c` to always use the BLESS internal LDO (bypass SIMO Buck Out2 logic):

1. Replace the `if(Cy_SysPm_SimoBuckIsEnabled())` block (~line 269) with just the `else` branch:
```c
BLE->BLESS.MISC_EN_CTRL |= (BLE_BLESS_MISC_EN_CTRL_ACT_REG_EN_CTRL_Msk |
                            BLE_BLESS_MISC_EN_CTRL_BUCK_EN_CTRL_Msk);
```

2. Replace the `if(!Cy_SysPm_SimoBuckOutputIsEnabled(CY_SYSPM_BUCK_VRF))` block (~line 331) with unconditional:
```c
temp |= BLE_BLESS_MT_CFG_ACT_LDO_NOT_BUCK_Msk;
```

> **Warning**: `cy_ble_clk.c` is PSoC Creator generated. Re-apply these patches after any code regeneration event. The patched file is committed to git.

**DO NOT disable SIMO Buck entirely** — Buck Out1 supplies VDDD on this hardware; disabling it causes an instant reset loop.

### Retry Loop (CM0+ `main_cm0p.c`)

Up to 5 attempts, 500ms apart:
- Each attempt: unconditionally clear `ENABLE_BLERD` and `INTR_MASK`, wait for `BLESS_STATE==0`
- `__DSB()` before writing result to `0x08000004` for CM4 visibility

---

## OTA Trigger (host_main.c)

- BLE write of `'u'` sets `ota_requested = true`
- IAS `HIGH_ALERT` also sets `ota_requested = true`
- On BLE stack shutdown: calls `Cy_DFU_ExecuteApp(0u)` → sets noinit `appId=0` → resets
- On reboot: bootloader sees `appId=0` → stays in DFU mode

---

## Key Gotchas

- PSoC Creator performs `SYSRESETREQ` (soft reset) after programming — App0's App1 presence check must run on ALL reset types.
- PSoC Programmer standalone requires hardware XRES — doesn't work here (no XRES line). Use PSoC Creator only.
- Linker scripts (`*.ld`) and `Generated_Source/*` are gitignored by default; these custom versions were force-added with `git add -f`.
- Do NOT override `Cy_OnResetUser` in App1 — any bit-bang UART there will use the wrong baud rate after `Cy_SysClk_FllDisable()` in App0.
- App1 CM0+ bit-bang UART diagnostic code was removed (2026-05-28); boot chain is confirmed working without it.
- DFU SDK files (`cy_dfu.c`, `cy_dfu.h`, `cy_dfu_bwc_macro.h`) were copied from `C:\Users\Dan\Downloads\PSoC6DfuBleApp101\...` to both projects' `Generated_Source/PSoC6/pdl/dfu/`.
