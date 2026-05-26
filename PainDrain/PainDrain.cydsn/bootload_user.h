/***************************************************************************//**
* \file bootload_user.h
* \version 2.0
*
* User-configurable Bootloader SDK parameters for PainDrain (App1).
* Included by the Bootloader SDK; do not rename this file.
*
* Requires the PSoC Creator Bootloader SDK component to be added to the
* PainDrain project before this will compile (provides bootloader/cy_bootload.h).
*
********************************************************************************
* Copyright 2016-2017, Cypress Semiconductor Corporation. All rights reserved.
*******************************************************************************/

#if !defined(BOOTLOAD_USER_H)
#define BOOTLOAD_USER_H

#include <stdint.h>
#include "flash/cy_flash.h"

/** Buffer sizes — must accommodate a full flash row plus 16 bytes of overhead */
#define CY_BOOTLOAD_SIZEOF_CMD_BUFFER   (CY_FLASH_SIZEOF_ROW + 16u)
#define CY_BOOTLOAD_SIZEOF_DATA_BUFFER  (CY_FLASH_SIZEOF_ROW + 16u)

/** Do not protect App0 (bootloader) from being overwritten */
#define CY_BOOTLOAD_OPT_GOLDEN_IMAGE    (0)
#define CY_BOOTLOAD_GOLDEN_IMAGE_IDS()  0u

/** Two apps in metadata: App0 (bootloader) + App1 (PainDrain) */
#define CY_BOOTLOAD_MAX_APPS            (2u)

#define CY_BOOTLOAD_OPT_VERIFY_DATA     (1)
#define CY_BOOTLOAD_OPT_ERASE_DATA      (1)
#define CY_BOOTLOAD_OPT_VERIFY_APP      (1)
#define CY_BOOTLOAD_OPT_SEND_DATA       (1)
#define CY_BOOTLOAD_OPT_GET_METADATA    (1)
#define CY_BOOTLOAD_OPT_SET_EIVECTOR    (0)
#define CY_BOOTLOAD_METADATA_WRITABLE   (1)
#define CY_BOOTLOAD_OPT_CRYPTO_HW       (0)
#define CY_BOOTLOAD_OPT_PACKET_CRC      (0)
#define CY_BOOTLOAD_APP_FORMAT          (CY_BOOTLOAD_BASIC_APP)
#define CY_BOOTLOAD_SEC_APP_VERIFY_TYPE (CY_BOOTLOAD_VERIFY_FAST)

/* Verify-region addresses come from the linker script (GCC) or MDK common header */
#if !defined(CY_DOXYGEN)
    #if defined(__GNUC__) || defined(__ICCARM__)
        extern uint8_t __cy_app0_verify_start;
        extern uint8_t __cy_app0_verify_length;
        extern uint8_t __cy_app1_verify_start;
        extern uint8_t __cy_app1_verify_length;

        #define CY_BOOTLOAD_APP0_VERIFY_START  ((uint32_t)&__cy_app0_verify_start)
        #define CY_BOOTLOAD_APP0_VERIFY_LENGTH ((uint32_t)&__cy_app0_verify_length)
        #define CY_BOOTLOAD_APP1_VERIFY_START  ((uint32_t)&__cy_app1_verify_start)
        #define CY_BOOTLOAD_APP1_VERIFY_LENGTH ((uint32_t)&__cy_app1_verify_length)

    #elif defined(__ARMCC_VERSION)
        #include "bootload_mdk_common.h"

        #define CY_BOOTLOAD_APP0_VERIFY_START  (CY_APP0_CORE0_FLASH_ADDR)
        #define CY_BOOTLOAD_APP0_VERIFY_LENGTH (CY_APP0_CORE0_FLASH_LENGTH + CY_APP0_CORE1_FLASH_LENGTH \
                                                - __CY_BOOT_SIGNATURE_SIZE)
        #define CY_BOOTLOAD_APP1_VERIFY_START  (CY_APP1_CORE0_FLASH_ADDR)
        #define CY_BOOTLOAD_APP1_VERIFY_LENGTH (CY_APP1_CORE0_FLASH_LENGTH + CY_APP1_CORE1_FLASH_LENGTH \
                                                - __CY_BOOT_SIGNATURE_SIZE)
    #else
        #error "Compiler not supported"
    #endif
#endif /* !defined(CY_DOXYGEN) */

#endif /* !defined(BOOTLOAD_USER_H) */

/* [] END OF FILE */
