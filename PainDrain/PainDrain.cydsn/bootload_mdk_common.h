/*******************************************************************************
* \file bootload_mdk_common.h
* \version 2.0
*
* Memory layout macros shared between the App1 scatter/linker file and C source.
* App0 (bootloader) lives at 0x10000000; App1 (PainDrain) lives at 0x10040000.
*
* These values must match the Bootloader SDK configuration in the PSoC Creator
* bootloader project (Bootloader_BLE_App101).
*
********************************************************************************
* Copyright 2017, Cypress Semiconductor Corporation. All rights reserved.
*******************************************************************************/

#ifndef BOOTLOAD_MDK_COMMON_H_
#define BOOTLOAD_MDK_COMMON_H_

/* Bootloader SDK parameters */
#define __CY_BOOT_METADATA_ADDR         0x100FFA00
#define __CY_BOOT_METADATA_LENGTH       0x200
#define __CY_PRODUCT_ID                 0x01020304
#define __CY_CHECKSUM_TYPE              0x00

/* Signature size: 4 = CRC-32 */
#define __CY_BOOT_SIGNATURE_SIZE        4

/* TOC (last Flash row on PSoC6 BLE 512K) */
#define CY_TOC_START                    0x16007C00
#define CY_TOC_SIZE                     0x400

/* ---- App0: Bootloader ---- */
#define CY_APP0_CORE0_FLASH_ADDR        0x10000000
#define CY_APP0_CORE0_FLASH_LENGTH      0x10000

#define CY_APP0_CORE1_FLASH_ADDR        0x10010000
#define CY_APP0_CORE1_FLASH_LENGTH      0x30000

/* ---- App1: PainDrain ---- */
#define CY_APP1_CORE0_FLASH_ADDR        0x10040000
#define CY_APP1_CORE0_FLASH_LENGTH      0x32000

#define CY_APP1_CORE1_FLASH_ADDR        0x10072000
#define CY_APP1_CORE1_FLASH_LENGTH      0x02000

/* Bootloader SDK metadata */
#define CY_BOOT_META_FLASH_ADDR         0x100FFA00
#define CY_BOOT_META_FLASH_LENGTH       0x200

/* Emulated EEPROM */
#define CY_APP0_CORE0_EM_EEPROM_ADDR    0x14000000
#define CY_APP0_CORE0_EM_EEPROM_LENGTH  0x8000
#define CY_APP0_CORE1_EM_EEPROM_ADDR    CY_APP0_CORE0_EM_EEPROM_ADDR
#define CY_APP0_CORE1_EM_EEPROM_LENGTH  0x8000

#define CY_APP1_CORE0_EM_EEPROM_ADDR    0x14000000
#define CY_APP1_CORE0_EM_EEPROM_LENGTH  0x8000
#define CY_APP1_CORE1_EM_EEPROM_ADDR    CY_APP1_CORE0_EM_EEPROM_ADDR
#define CY_APP1_CORE1_EM_EEPROM_LENGTH  0x8000

/* SMIF XIP (unused) */
#define CY_APP0_CORE0_SMIF_ADDR         0x18000000
#define CY_APP0_CORE0_SMIF_LENGTH       0x00000000
#define CY_APP0_CORE1_SMIF_ADDR         (CY_APP0_CORE0_SMIF_ADDR + CY_APP0_CORE0_SMIF_LENGTH)
#define CY_APP0_CORE1_SMIF_LENGTH       0x00000000

#define CY_APP1_CORE0_SMIF_ADDR         0x14000200
#define CY_APP1_CORE0_SMIF_LENGTH       0x00000000
#define CY_APP1_CORE1_SMIF_ADDR         (CY_APP1_CORE0_SMIF_ADDR + CY_APP1_CORE0_SMIF_LENGTH)
#define CY_APP1_CORE1_SMIF_LENGTH       0x00000000

/* RAM layout */
#define CY_APP_RAM_COMMON_ADDR          0x08000000
#define CY_APP_RAM_COMMON_LENGTH        0x00000100

#define CY_APP0_CORE0_RAM_ADDR          0x08000100
#define CY_APP0_CORE0_RAM_LENGTH        0x00001F00
#define CY_APP0_CORE1_RAM_ADDR          (CY_APP0_CORE0_RAM_ADDR + CY_APP0_CORE0_RAM_LENGTH)
#define CY_APP0_CORE1_RAM_LENGTH        0x00008000

#define CY_APP1_CORE0_RAM_ADDR          CY_APP0_CORE0_RAM_ADDR
#define CY_APP1_CORE0_RAM_LENGTH        0x0001FF00
#define CY_APP1_CORE1_RAM_ADDR          (CY_APP1_CORE0_RAM_ADDR + CY_APP1_CORE0_RAM_LENGTH)
#define CY_APP1_CORE1_RAM_LENGTH        0x00020000

#endif /* BOOTLOAD_MDK_COMMON_H_ */

/* [] END OF FILE */
