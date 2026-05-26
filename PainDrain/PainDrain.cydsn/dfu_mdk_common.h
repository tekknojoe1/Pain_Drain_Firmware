/*******************************************************************************
* \file dfu_mdk_common.h
* \version 3.0
*
* Memory layout macros for PainDrain as App1.
* Used by MDK scatter files and MDK-only symbol stubs.
* GCC projects use the linker script symbols instead.
*
* App0 (bootloader): 0x10000000
* App1 (PainDrain):  0x10040000
*
********************************************************************************
* Copyright 2016-2018, Cypress Semiconductor Corporation. All rights reserved.
*******************************************************************************/

#ifndef DFU_MDK_COMMON_H_
#define DFU_MDK_COMMON_H_

/* DFU SDK metadata */
#define CY_BOOT_METADATA_ADDR       0x100FFA00
#define CY_BOOT_METADATA_LENGTH     0x200
#define CY_PRODUCT_ID               0x01020304
#define CY_CHECKSUM_TYPE            0x00

/* CRC-32 signature = 4 bytes */
#define CY_BOOT_SIGNATURE_SIZE      4

/* TOC (last Flash row on PSoC6 BLE 512K) */
#define CY_TOC_START                0x16007C00
#define CY_TOC_SIZE                 0x400

/* ---- App0: Bootloader ---- */
#define CY_APP0_CORE0_FLASH_ADDR    0x10000000
#define CY_APP0_CORE0_FLASH_LENGTH  0x10000

#define CY_APP0_CORE1_FLASH_ADDR    0x10010000
#define CY_APP0_CORE1_FLASH_LENGTH  0x30000

/* ---- App1: PainDrain ---- */
#define CY_APP1_CORE0_FLASH_ADDR    0x10040000
#define CY_APP1_CORE0_FLASH_LENGTH  0x32000

#define CY_APP1_CORE1_FLASH_ADDR    0x10072000
#define CY_APP1_CORE1_FLASH_LENGTH  0x4E000

/* Metadata region */
#define CY_BOOT_META_FLASH_ADDR     0x100FFA00
#define CY_BOOT_META_FLASH_LENGTH   0x200

/* Emulated EEPROM (unused) */
#define CY_APP0_CORE0_EM_EEPROM_ADDR   0x14000000
#define CY_APP0_CORE0_EM_EEPROM_LENGTH 0x00000000
#define CY_APP0_CORE1_EM_EEPROM_ADDR   (CY_APP0_CORE0_EM_EEPROM_ADDR + CY_APP0_CORE0_EM_EEPROM_LENGTH)
#define CY_APP0_CORE1_EM_EEPROM_LENGTH 0x00000000
#define CY_APP1_CORE0_EM_EEPROM_ADDR   0x14000000
#define CY_APP1_CORE0_EM_EEPROM_LENGTH 0x00000000
#define CY_APP1_CORE1_EM_EEPROM_ADDR   (CY_APP1_CORE0_EM_EEPROM_ADDR + CY_APP1_CORE0_EM_EEPROM_LENGTH)
#define CY_APP1_CORE1_EM_EEPROM_LENGTH 0x00000000

/* SMIF XIP (unused) */
#define CY_APP0_CORE0_SMIF_ADDR    0x18000000
#define CY_APP0_CORE0_SMIF_LENGTH  0x00000000
#define CY_APP0_CORE1_SMIF_ADDR    (CY_APP0_CORE0_SMIF_ADDR + CY_APP0_CORE0_SMIF_LENGTH)
#define CY_APP0_CORE1_SMIF_LENGTH  0x00000000
#define CY_APP1_CORE0_SMIF_ADDR    0x18000000
#define CY_APP1_CORE0_SMIF_LENGTH  0x00000000
#define CY_APP1_CORE1_SMIF_ADDR    (CY_APP1_CORE0_SMIF_ADDR + CY_APP1_CORE0_SMIF_LENGTH)
#define CY_APP1_CORE1_SMIF_LENGTH  0x00000000

/* RAM */
#define CY_APP_RAM_COMMON_ADDR      0x08000000
#define CY_APP_RAM_COMMON_LENGTH    0x00000100

#define CY_APP0_CORE0_RAM_ADDR      0x08000100
#define CY_APP0_CORE0_RAM_LENGTH    0x00001F00
#define CY_APP0_CORE1_RAM_ADDR      (CY_APP0_CORE0_RAM_ADDR + CY_APP0_CORE0_RAM_LENGTH)
#define CY_APP0_CORE1_RAM_LENGTH    0x00008000

#define CY_APP1_CORE0_RAM_ADDR      CY_APP0_CORE0_RAM_ADDR
#define CY_APP1_CORE0_RAM_LENGTH    0x0001FF00
#define CY_APP1_CORE1_RAM_ADDR      0x08020000
#define CY_APP1_CORE1_RAM_LENGTH    0x00020000

__asm void cy_DFU_mdkAsmDummy(void);

#endif /* DFU_MDK_COMMON_H_ */

/* [] END OF FILE */
