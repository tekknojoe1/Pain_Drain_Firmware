/***************************************************************************//**
* \file dfu_user.h
* \version 3.0
*
* DFU SDK user configuration for PainDrain (App1).
* Included by cy_dfu.h; do not rename this file.
*
********************************************************************************
* Copyright 2016-2018, Cypress Semiconductor Corporation. All rights reserved.
*******************************************************************************/

#if !defined(DFU_USER_H)
#define DFU_USER_H

#include <stdint.h>
#include "cy_flash.h"

/** The size of a buffer to hold DFU commands */
#define CY_DFU_SIZEOF_CMD_BUFFER  (CY_FLASH_SIZEOF_ROW + 16u)

/** The size of a buffer to hold an NVM row of data to write or verify */
#define CY_DFU_SIZEOF_DATA_BUFFER (CY_FLASH_SIZEOF_ROW + 16u)

#define CY_DFU_OPT_GOLDEN_IMAGE    (0)
#define CY_DFU_GOLDEN_IMAGE_IDS()  0u

/** Two apps: App0 (bootloader) + App1 (PainDrain) */
#define CY_DFU_MAX_APPS            (2u)

#define CY_DFU_OPT_VERIFY_DATA     (1)
#define CY_DFU_OPT_ERASE_DATA      (1)
#define CY_DFU_OPT_VERIFY_APP      (1)
#define CY_DFU_OPT_SEND_DATA       (1)
#define CY_DFU_OPT_GET_METADATA    (1)
#define CY_DFU_OPT_SET_EIVECTOR    (0)
#define CY_DFU_METADATA_WRITABLE   (1)
#define CY_DFU_OPT_CRYPTO_HW       (0)
#define CY_DFU_OPT_PACKET_CRC      (0)
#define CY_DFU_APP_FORMAT          (CY_DFU_BASIC_APP)
#define CY_DFU_SEC_APP_VERIFY_TYPE (CY_DFU_VERIFY_FAST)

#if !defined(CY_DOXYGEN)
    #if defined(__GNUC__) || defined(__ICCARM__)
        /* Linker-script-defined symbols for CRC verification regions */
        extern uint8_t __cy_app0_verify_start;
        extern uint8_t __cy_app0_verify_length;
        extern uint8_t __cy_app1_verify_start;
        extern uint8_t __cy_app1_verify_length;
        extern uint8_t __cy_boot_signature_size;

        #define CY_DFU_APP0_VERIFY_START       ( (uint32_t)&__cy_app0_verify_start )
        #define CY_DFU_APP0_VERIFY_LENGTH      ( (uint32_t)&__cy_app0_verify_length )
        #define CY_DFU_APP1_VERIFY_START       ( (uint32_t)&__cy_app1_verify_start )
        #define CY_DFU_APP1_VERIFY_LENGTH      ( (uint32_t)&__cy_app1_verify_length )
        #define CY_DFU_SIGNATURE_SIZE          ( (uint32_t)&__cy_boot_signature_size )

    #elif defined(__ARMCC_VERSION)
        #include "dfu_mdk_common.h"

        #define CY_DFU_APP0_VERIFY_START       ( CY_APP0_CORE0_FLASH_ADDR )
        #define CY_DFU_APP0_VERIFY_LENGTH      ( CY_APP0_CORE0_FLASH_LENGTH + CY_APP0_CORE1_FLASH_LENGTH \
                                                    - CY_BOOT_SIGNATURE_SIZE)
        #define CY_DFU_APP1_VERIFY_START       ( CY_APP1_CORE0_FLASH_ADDR )
        #define CY_DFU_APP1_VERIFY_LENGTH      ( CY_APP1_CORE0_FLASH_LENGTH + CY_APP1_CORE1_FLASH_LENGTH \
                                                    - CY_BOOT_SIGNATURE_SIZE)
        #define CY_DFU_SIGNATURE_SIZE          CY_BOOT_SIGNATURE_SIZE

    #else
        #error "Compiler not supported"
    #endif
#endif /* !defined(CY_DOXYGEN) */

#endif /* !defined(DFU_USER_H) */

/* [] END OF FILE */
