# PainDrain Firmware — Workstation Setup

How to set up a development machine to build, flash, and work on the PainDrain
PSoC 6 BLE firmware.

> **Platform:** Windows only. PSoC Creator (the IDE/build system) does not run on
> macOS or Linux. A Windows 10/11 machine (or VM) is required.

---

## 1. What this project is

- **MCU / module:** Infineon (Cypress) **CYBLE-416045-02** — a PSoC 6 BLE module
  (dual-core Cortex-M0+ / Cortex-M4, 1 MB flash).
- **Two PSoC Creator projects:**
  - `Bootloader/Bootloader_BLE_App101.cydsn` — a minimal **non-BLE** chooser
    bootloader (CM0+). On reset it validates the two app slots and boots the
    newest valid one.
  - `PainDrain/PainDrain.cydsn` — the application (BLE, TENS/vibe/temp control,
    and the OTA engine). Built **twice** (once per A/B slot).
- **Dual-app A/B OTA:** the app receives a firmware image over BLE, writes it to
  the *inactive* slot, and resets; the bootloader then boots the new version.
- **Flash map (1 MB):**
  | Region | Range |
  |---|---|
  | Bootloader | `0x10000000–0x1000FFFF` (64 KB) |
  | App0 slot | `0x10010000–0x10087FFF` (480 KB), metadata `0x10087F00` |
  | App1 slot | `0x10088000–0x100FFFFF` (480 KB), metadata `0x100FFF00` |
  | Em_EEPROM (presets/settings) | work flash `0x14000000` (32 KB) — shared by both slots |

---

## 2. Tools to install

| Tool | Version | Notes |
|---|---|---|
| **PSoC Creator** | **4.4** | The IDE + bundled **ARM GCC 5.4.1** toolchain. Installs to `C:\Program Files (x86)\Cypress\PSoC Creator\4.4\`. |
| **PSoC PDL** | **3.1.7** | Peripheral Driver Library. Provides `cymcuelftool.exe` (used to sign images + make `.cyacd2`). Installs to `C:\Program Files (x86)\Cypress\PDL\3.1.7\`. |
| **Python** | **3.8+** | Runs the build scripts. **Standard library only — no `pip install` needed.** Put `python` on `PATH`. |
| **Git** | any | Version control. |
| **Hardware programmer** | MiniProg4 or on-board **KitProg** | Required to flash the bootloader (it can't update itself over the air). |
| **CySmart** | latest | (Optional) BLE host tool for testing OTA from a PC. The mobile app does the same job. |
| **USB–UART adapter** | 3.3 V | (Optional) to read debug output — see §6. |

PSoC Creator 4.4 and PDL 3.1.7 are downloaded from Infineon's site (search
"PSoC Creator 4.4" / "PSoC PDL"). Install PSoC Creator first, then PDL.

> The build scripts assume the **default install paths** above. If you installed
> PSoC Creator/PDL elsewhere, update the paths at the top of `build_slot.py`
> (`GCC_BIN`, `CYMCUELFTOOL`).

### LVGL (graphics library)
The app uses **LVGL v8.3** (for the display). See
`Instructions to set up LVGL on PSOC 6.txt` in the repo root and the
`PainDrain/PainDrain.cydsn/lvgl-release-v8.3` folder.

---

## 3. Repo layout

```
firmware/
├─ PainDrain.cywrk                 ← open THIS workspace in PSoC Creator
├─ Bootloader/Bootloader_BLE_App101.cydsn/   ← bootloader project
├─ PainDrain/PainDrain.cydsn/                 ← app project
├─ build_slot.py        ← relink the app per A/B slot; sign; emit .cyacd2 (+ CRC, preset-strip)
├─ bundle_ota.py        ← zip both slot .cyacd2 + manifest.json for the mobile app
├─ merge_hex.py         ← merge bootloader + app hex into one programmable image
├─ apply_ble_patches.py ← re-apply WCO + BLESS-LDO clock patches after a BLE regen
├─ SETUP.md             ← this file
└─ CE2xxxxx_*           ← Cypress example projects (reference only; not built)
```

**Gitignored but important:** PSoC Creator regenerates `Generated_Source/` and the
build output dirs (`CortexM0p/`, `CortexM4/`). The custom linker scripts
(`cy8c6xx7_*.ld`) and a couple of patched generated files are force-added to git,
so a clean checkout has what it needs once you Generate + build in the IDE.

---

## 4. First-time setup

1. **Clone** the repo and `cd firmware`.
2. **Install** the tools in §2 (PSoC Creator 4.4, PDL 3.1.7, Python 3, Git).
3. **Set up LVGL** per `Instructions to set up LVGL on PSOC 6.txt`.
4. **Open the workspace:** launch PSoC Creator → File → Open → Project/Workspace →
   `PainDrain.cywrk`. It contains both the Bootloader and PainDrain projects.
5. **Generate Application** for each project (right-click project → *Generate
   Application*) so `Generated_Source/` is created.
6. **If you ever change the BLE component** (services, MTU, clocks) and regenerate:
   run `python apply_ble_patches.py` afterward — it re-applies the WCO LFCLK and
   BLESS-LDO patches that BLE-after-bootloader-jump depends on. **Also verify the
   BLE "CPU core" stays "Dual core (Controller CM0+, Host+Profiles CM4)"** — a
   regen has been known to flip it to single-core (causes an `undefined reference
   to HostMain` link error).

---

## 5. Building & flashing

**Build order:** build the **Bootloader** first, then **PainDrain**.

1. **Bootloader:** select the Bootloader project → Build. Produces its hex.
2. **PainDrain:** select PainDrain → Build. Its post-build (`post_build.bat`):
   - merges bootloader + App0 into one programmable hex (`merge_hex.py`),
   - runs `build_slot.py 0` and `build_slot.py 1` → per-slot `.cyacd2` (with
     integrity CRC; Em_EEPROM rows stripped so OTA preserves presets),
   - runs `bundle_ota.py` → `PainDrain_ota_v<ver>.zip` (both slots + manifest).

**Flashing a device (factory / first time):** use PSoC Creator's Program button
with a **MiniProg/KitProg** attached. This writes **bootloader + App0**. The
bootloader cannot be updated over the air — any bootloader change must be
reprogrammed via the hardware programmer.

**Outputs of interest** (in `PainDrain/PainDrain.cydsn/CortexM4/ARM_GCC_541/Debug/`):
- `PainDrain.hex` — combined bootloader+App0 (program this).
- `PainDrain_slot0.cyacd2` / `PainDrain_slot1.cyacd2` — OTA images.
- `PainDrain_ota_v<ver>.zip` — the bundle the mobile app consumes.

---

## 6. Debug output

Both the bootloader and the app print over a **bit-bang UART on pin P0.5, 9600
8N1** (not a UART component). Connect a 3.3 V USB–UART adapter's RX to **P0.5** and
GND to GND, open a serial terminal at **9600**. You'll see the bootloader's slot
selection, the app boot log, OTA progress, etc.

---

## 7. OTA / release workflow

- **Versioning** lives in `PainDrain/PainDrain.cydsn/version.h`
  (`FIRMWARE_VERSION_MAJOR/MINOR/PATCH/BUILD`).
  - The integer version used for slot selection is `MAJOR*10000 + MINOR*100 +
    PATCH`. **An OTA only switches slots if the new version is strictly higher**,
    so a release MUST bump MAJOR/MINOR/PATCH.
  - **Rule:** when MAJOR/MINOR/PATCH changes, **reset `BUILD` to 0**. `BUILD` is a
    display/traceability counter only.
- **Build the OTA:** a normal PainDrain build emits the per-slot `.cyacd2` and the
  `PainDrain_ota_v<ver>.zip`. The mobile app reads the device's running slot (from
  the **DIS Firmware Revision String**, formatted `<version>/<slot>`) and uploads
  the *other* slot's `.cyacd2`.
- Tag releases in git as `vMAJOR.MINOR.PATCH` (e.g. `v1.1.1`).

---

## 8. Gotchas (read before you fight the toolchain)

- **PSoC Creator is Windows-only.** No Linux/macOS build.
- **After a BLE regen → run `apply_ble_patches.py`** and confirm dual-core (§4).
- **Custom linker scripts & some generated files are force-added** to git; don't
  delete them or "clean" them out.
- **`build_slot.py` paths are hard-coded** to the default tool install dirs — edit
  them if your install differs.
- **The bootloader can't self-update** — bootloader changes need a hardware
  programmer on every existing unit.
- **Don't store persistent user data in an app slot** — it must live in work flash
  (`0x14000000`, Em_EEPROM), which the OTA leaves untouched, or updates will wipe it.
