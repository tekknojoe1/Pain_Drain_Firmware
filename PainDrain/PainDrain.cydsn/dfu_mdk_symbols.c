/*******************************************************************************
* \file dfu_mdk_symbols.c
* \version 3.0
*
* Exports ELF symbols required by CyMCUElfTool to generate correct HEX and
* CYACD2 files for PainDrain as App1 (the DFU application).
*
* Only compiled when using the ARM MDK toolchain. GCC projects get these
* symbols from the linker script instead.
*
********************************************************************************
* Copyright 2016-2018, Cypress Semiconductor Corporation. All rights reserved.
*******************************************************************************/

#include "dfu_mdk_common.h"

__asm void cy_DFU_mdkAsmDummy(void)
{
    EXPORT __cy_boot_metadata_addr
    EXPORT __cy_boot_metadata_length

    EXPORT __cy_app_core1_start_addr

    EXPORT __cy_product_id
    EXPORT __cy_checksum_type
    EXPORT __cy_app_id

    EXPORT __cy_app_verify_start
    EXPORT __cy_app_verify_length

/* Metadata block shared by both apps */
__cy_boot_metadata_addr   EQU __cpp(CY_BOOT_METADATA_ADDR)
__cy_boot_metadata_length EQU __cpp(CY_BOOT_METADATA_LENGTH)

/* CM4 entry point for App1 (used by CM0+ to start CM4 after launch) */
__cy_app_core1_start_addr EQU __cpp(CY_APP1_CORE1_FLASH_ADDR)

__cy_product_id           EQU __cpp(CY_PRODUCT_ID)
__cy_checksum_type        EQU __cpp(CY_CHECKSUM_TYPE)

/* This is App1 */
__cy_app_id               EQU 1

/* CRC-32 signature covers all of App1 flash minus the 4-byte signature itself */
__cy_app_verify_start     EQU __cpp(CY_APP1_CORE0_FLASH_ADDR)
__cy_app_verify_length    EQU __cpp(CY_APP1_CORE0_FLASH_LENGTH + CY_APP1_CORE1_FLASH_LENGTH \
                                    - CY_BOOT_SIGNATURE_SIZE)
}

/* [] END OF FILE */
