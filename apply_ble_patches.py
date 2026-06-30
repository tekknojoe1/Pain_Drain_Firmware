#!/usr/bin/env python3
"""Re-apply the WCO-LFCLK and BLESS-LDO patches to PainDrain's Generated_Source.

PSoC Creator regenerates Generated_Source whenever the BLE design (.cysch/.cydwr)
changes, wiping the two patches that make BLE work after the bootloader jump
(see memory project_ble_after_jump_resolved). Set this as the PainDrain project's
PRE-BUILD command so the patches are re-applied automatically before every
compile. It is idempotent -- running it when already patched is a no-op.

  Patch 1 (cyfitter_cfg.c ClockInit): LFCLK source PILO -> WCO.
            (If you instead set ClkLf=WCO in the .cydwr Clocks editor, this
             patch's anchor disappears and it is simply skipped -- harmless.)
  Patch 2 (cy_ble_clk.c Cy_BLE_EcoStart): force the BLESS internal LDO for BLE RF
            instead of SIMO Buck Out2 (not connected on this board).

Set as pre-build command (PSoC Creator -> Build Settings -> ... -> User Commands
-> Pre-Build) e.g.:   python "${ProjectDir}\\..\\..\\apply_ble_patches.py"
"""
import os
import sys

ROOT = os.path.dirname(os.path.abspath(__file__))
GEN = os.path.join(ROOT, "PainDrain", "PainDrain.cydsn", "Generated_Source", "PSoC6")
CYFITTER = os.path.join(GEN, "cyfitter_cfg.c")
CYBLECLK = os.path.join(GEN, "pdl", "middleware", "ble", "cy_ble_clk.c")

MARKER = "PATCHED (dual-app"


def apply(path, label, replacements):
    """Apply a list of (old, new) replacements to a file, idempotently."""
    if not os.path.exists(path):
        print(f"  [{label}] file not found: {path}")
        return
    text = open(path, encoding="utf-8", errors="replace").read()
    if MARKER in text:
        print(f"  [{label}] already patched")
        return
    applied = 0
    for old, new in replacements:
        if old in text:
            text = text.replace(old, new, 1)
            applied += 1
    if applied == len(replacements):
        open(path, "w", encoding="utf-8").write(text)
        print(f"  [{label}] patched ({applied} edits)")
    else:
        print(f"  [{label}] anchor(s) not found ({applied}/{len(replacements)}) "
              f"-- PDL may have changed, or design already uses WCO; left unmodified")


# --- Patch 1: cyfitter_cfg.c -- LFCLK PILO -> WCO (tabs match the generated file) ---
WCO_OLD = (
    "\t/* Enable all source clocks */\n"
    "\tCy_SysClk_PiloEnable();\n"
    "\tCy_SysClk_ClkLfSetSource(CY_SYSCLK_CLKLF_IN_PILO);"
)
WCO_NEW = (
    "\t/* Enable all source clocks */\n"
    "\t/* PATCHED (dual-app + BLE): use the WCO 32.768 kHz crystal for LFCLK\n"
    "\t * instead of PILO. BLE requires an accurate LFCLK; PILO calibration fails\n"
    "\t * (CY_BLE_EVT_HARDWARE_ERROR, code 0x0D). The bootloader already runs LFCLK\n"
    "\t * on WCO, so switching to PILO across the jump breaks BLESS. The board has a\n"
    "\t * WCO crystal (the bootloader enables it successfully). Falls back to PILO\n"
    "\t * if WCO ever fails to start. */\n"
    "\tstatus = Cy_SysClk_WcoEnable(500000u);\n"
    "\tif (status != CY_SYSCLK_SUCCESS)\n"
    "\t{\n"
    "\t\tCy_SysClk_PiloEnable();\n"
    "\t\tCy_SysClk_ClkLfSetSource(CY_SYSCLK_CLKLF_IN_PILO);\n"
    "\t}\n"
    "\telse\n"
    "\t{\n"
    "\t\tCy_SysClk_ClkLfSetSource(CY_SYSCLK_CLKLF_IN_WCO);\n"
    "\t}"
)

# --- Patch 2: cy_ble_clk.c -- force BLESS LDO (spaces match the generated file) ---
LDO1_OLD = (
    "                /* If user uses SIMO Buck, enable Buck2 in hardware mode for BLE */\n"
    "                if(Cy_SysPm_SimoBuckIsEnabled())\n"
    "                {\n"
    "                    Cy_SysPm_SimoBuckSetVoltage2(CY_SYSPM_SIMO_BUCK_OUT2_VOLTAGE_1_3V, true);\n"
    "                    Cy_SysPm_SimoBuckSetHwControl(true);\n"
    "                }\n"
    "                else    /* Configure BLE LDO if SIMO Buck2 is not enabled */\n"
    "                {\n"
    "                    BLE->BLESS.MISC_EN_CTRL |= (BLE_BLESS_MISC_EN_CTRL_ACT_REG_EN_CTRL_Msk |\n"
    "                                                BLE_BLESS_MISC_EN_CTRL_BUCK_EN_CTRL_Msk);\n"
    "                }"
)
LDO1_NEW = (
    "                /* PATCHED (dual-app bootloader): always configure the BLESS\n"
    "                 * internal LDO for BLE RF, regardless of the SRSS SIMO Buck\n"
    "                 * state. On this board SIMO Buck Out1 powers VDDD (so the buck\n"
    "                 * reads as \"enabled\"), but BLE RF is fed by the BLESS internal\n"
    "                 * LDO -- Buck Out2 (VRF) is not connected. Because the bootloader\n"
    "                 * jumps to the app without a full SRSS reset, the buck is still\n"
    "                 * enabled at Cy_BLE_EcoStart() time; taking the buck path here\n"
    "                 * routes BLE RF to the unconnected Out2 -> CY_BLE_EVT_HARDWARE_ERROR.\n"
    "                 * Forcing the LDO path matches a cold (hardware-reset) boot. */\n"
    "                BLE->BLESS.MISC_EN_CTRL |= (BLE_BLESS_MISC_EN_CTRL_ACT_REG_EN_CTRL_Msk |\n"
    "                                            BLE_BLESS_MISC_EN_CTRL_BUCK_EN_CTRL_Msk);"
)
LDO2_OLD = (
    "                    if(!Cy_SysPm_SimoBuckOutputIsEnabled(CY_SYSPM_BUCK_VRF))\n"
    "                    {\n"
    "                        temp |= BLE_BLESS_MT_CFG_ACT_LDO_NOT_BUCK_Msk;\n"
    "                    }"
)
LDO2_NEW = (
    "                    /* PATCHED (dual-app bootloader): always select the BLESS LDO\n"
    "                     * (not Buck Out2) for the active regulator -- see note above. */\n"
    "                    temp |= BLE_BLESS_MT_CFG_ACT_LDO_NOT_BUCK_Msk;"
)


def main():
    print("apply_ble_patches: re-applying WCO + LDO patches...")
    apply(CYFITTER, "WCO LFCLK", [(WCO_OLD, WCO_NEW)])
    apply(CYBLECLK, "BLESS LDO", [(LDO1_OLD, LDO1_NEW), (LDO2_OLD, LDO2_NEW)])
    return 0


if __name__ == "__main__":
    sys.exit(main())
