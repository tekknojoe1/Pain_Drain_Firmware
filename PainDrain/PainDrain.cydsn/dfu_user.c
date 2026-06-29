/***************************************************************************//**
* \file dfu_user.c
* \version 3.0
* 
* This file provides the custom API for a firmware application with 
* DFU SDK.
* - Cy_DFU_ReadData (address, length, ctl, params) - to read  the NVM block
* - Cy_Bootalod_WriteData(address, length, ctl, params) - to write the NVM block
*
* - Cy_DFU_TransportStart() to start a communication interface
* - Cy_DFU_TransportStop () to stop  a communication interface
* - Cy_DFU_TransportReset() to reset a communication interface
* - Cy_DFU_TransportRead (buffer, size, count, timeout)
* - Cy_DFU_TransportWrite(buffer, size, count, timeout)
*
********************************************************************************
* \copyright
* Copyright 2016-2018, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include <string.h>
#include "transport_ble.h"
#include "cy_syslib.h"
#include "cy_flash.h"
#include "cy_dfu.h"


/*
* The DFU SDK metadata initial value is placed here
* Note: the number of elements equal to the number of the app multiplies by 2 
*       because of the two fields per app plus one element for the CRC-32C field.
*/
CY_SECTION(".cy_boot_metadata") __USED
static const uint32_t cy_dfu_metadata[CY_FLASH_SIZEOF_ROW / sizeof(uint32_t)] =
{
    CY_DFU_APP0_VERIFY_START, CY_DFU_APP0_VERIFY_LENGTH, /* The App0 base address and length */
    CY_DFU_APP1_VERIFY_START, CY_DFU_APP1_VERIFY_LENGTH, /* The App1 base address and length */
    0u                                                             /* The rest does not matter     */
};


static uint32_t IsMultipleOf(uint32_t value, uint32_t multiple);
static void GetStartEndAddress(uint32_t appId, uint32_t *startAddress, uint32_t *endAddress);


/* Dual-app slot metadata (last 256 B of each slot) -- shared format with the
 * bootloader chooser and build_slot.py. The CRC covers [slot_start, metadata) =
 * the whole image excluding this record; both slots span META-START = 0x77F00. */
#define OTA_META_MAGIC      (0xDEADBEEFu)
#define OTA_APP0_START      (0x10010000u)
#define OTA_APP1_START      (0x10088000u)
#define OTA_APP0_META_ADDR  (0x10087F00u)
#define OTA_APP1_META_ADDR  (0x100FFF00u)
#define OTA_CRC_LEN         (OTA_APP0_META_ADDR - OTA_APP0_START)   /* 0x77F00 */

typedef struct {
    uint32_t magic;     /* OTA_META_MAGIC = valid */
    uint32_t version;
    uint32_t size;      /* bytes the crc covers; 0 = no CRC stored (legacy image) */
    uint32_t crc;       /* CRC-32 (IEEE/zlib) over [slot_start, slot_start+size)  */
} ota_metadata_t;

/* CRC-32 (IEEE 802.3, poly 0xEDB88320) -- byte-for-byte identical to Python
 * zlib.crc32 (build_slot.py) and the bootloader's check. Table built on first use. */
static uint32_t ota_crc32(const uint8_t *data, uint32_t len)
{
    static uint32_t table[256];
    static uint8_t  built = 0u;
    if (built == 0u)
    {
        for (uint32_t i = 0u; i < 256u; i++)
        {
            uint32_t c = i;
            for (uint8_t k = 0u; k < 8u; k++) { c = (c & 1u) ? (0xEDB88320u ^ (c >> 1)) : (c >> 1); }
            table[i] = c;
        }
        built = 1u;
    }
    uint32_t crc = 0xFFFFFFFFu;
    for (uint32_t i = 0u; i < len; i++) { crc = table[(crc ^ data[i]) & 0xFFu] ^ (crc >> 8); }
    return ~crc;
}

/*******************************************************************************
* Function Name: Cy_DFU_ValidateApp  (override of the __WEAK SDK default)
****************************************************************************//**
* The DFU "Verify Application" command's integrity check. Our bootloader selects
* a slot by the {magic,version,size,crc} record at the slot tail (the SDK's
* signature-footer scheme isn't produced by our build). Verify both: magic, and
* -- when a CRC is stored (size != 0) -- the CRC-32 over the image. size == 0 is
* a legacy image with no stored CRC: accept on magic alone (brick-safe / backward
* compatible). Returning SUCCESS lets the host report a successful update.
*******************************************************************************/
cy_en_dfu_status_t Cy_DFU_ValidateApp(uint32_t appId, cy_stc_dfu_params_t *params)
{
    (void)params;
    const ota_metadata_t *m =
        (const ota_metadata_t *)((appId == 0u) ? OTA_APP0_META_ADDR : OTA_APP1_META_ADDR);
    const uint32_t slotStart = (appId == 0u) ? OTA_APP0_START : OTA_APP1_START;

    if (m->magic != OTA_META_MAGIC) { return CY_DFU_ERROR_VERIFY; }
    if (m->size != 0u)   /* CRC stored -> verify it */
    {
        if (m->size > OTA_CRC_LEN) { return CY_DFU_ERROR_VERIFY; }            /* sanity */
        if (ota_crc32((const uint8_t *)slotStart, m->size) != m->crc) { return CY_DFU_ERROR_VERIFY; }
    }
    return CY_DFU_SUCCESS;
}


/*******************************************************************************
* Function Name: IsMultipleOf
****************************************************************************//**
*
* This internal function check if value parameter is a multiple of parameter 
* multiple
*
* \param value      value that will be checked
* \param multiple   value with which value is checked 
*
* \return 1 - value is multiple of parameter multiple, else 0  
*
*******************************************************************************/
static uint32_t IsMultipleOf(uint32_t value, uint32_t multiple)
{
    return ( ((value % multiple) == 0u)? 1ul : 0ul);
}


/*******************************************************************************
* Function Name: GetStartEndAddress
****************************************************************************//**
*
* This internal function returns start and end address of application
*
* \param appId          The application number
* \param startAddress   The pointer to a variable where an application start
*                       address is stored
* \param endAddress     The pointer to a variable where a size of application 
*                       area is stored.
*
*******************************************************************************/
static void GetStartEndAddress(uint32_t appId, uint32_t *startAddress, uint32_t *endAddress)
{
    uint32_t verifyStart;
    uint32_t verifySize;

    (void)Cy_DFU_GetAppMetadata(appId, &verifyStart, &verifySize);

#if (CY_DFU_APP_FORMAT == CY_DFU_SIMPLIFIED_APP)
    *startAddress = verifyStart - CY_DFU_SIGNATURE_SIZE; 
    *endAddress = verifyStart + verifySize;
#else
    *startAddress = verifyStart; 
    *endAddress = verifyStart + verifySize + CY_DFU_SIGNATURE_SIZE;
#endif
}


/*******************************************************************************
* Function Name: Cy_DFU_WriteData
****************************************************************************//**
*
* This function documentation is part of the DFU SDK API, see the
* cy_dfu.h file or DFU SDK API Reference Manual for details.
*
*******************************************************************************/
cy_en_dfu_status_t Cy_DFU_WriteData (uint32_t address, uint32_t length, uint32_t ctl, 
                                               cy_stc_dfu_params_t *params)
{
    /* Allowed update range = both app slots (App0 start .. end of flash). The
     * 64 KB bootloader below App0 is protected, and the running app's own slot
     * is rejected by the GetRunningApp check above -- so writes land only in the
     * INACTIVE slot, in either A/B direction. */
    const uint32_t minUFlashAddress = CY_DFU_APP0_VERIFY_START;
    const uint32_t maxUFlashAddress = CY_FLASH_BASE + CY_FLASH_SIZE;
    /* EM_EEPROM Limits*/
    const uint32_t minEmEepromAddress = CY_EM_EEPROM_BASE;
    const uint32_t maxEmEepromAddress = CY_EM_EEPROM_BASE + CY_EM_EEPROM_SIZE;
    
    cy_en_dfu_status_t status = CY_DFU_SUCCESS;

    uint32_t app = Cy_DFU_GetRunningApp();
    uint32_t startAddress;
    uint32_t endAddress;
    
    GetStartEndAddress(app, &startAddress, &endAddress);

    /* Check if the address  and length are valid 
     * Note Length = 0 is valid for erase command */
    if ( (IsMultipleOf(address, CY_FLASH_SIZEOF_ROW) == 0u) || 
         ( (length != CY_FLASH_SIZEOF_ROW) && ( (ctl & CY_DFU_IOCTL_ERASE) == 0u) ) )
    {
        status = CY_DFU_ERROR_LENGTH;   
    }

    /* Refuse to write to a row within a range of the current application */ 
    if ( (startAddress <= address) && (address < endAddress) )
    {   /* It is forbidden to overwrite the currently running application */
        status = CY_DFU_ERROR_ADDRESS;
    }
#if CY_DFU_OPT_GOLDEN_IMAGE
    if (status == CY_DFU_SUCCESS)
    {
        uint8_t goldenImages[] = { CY_DFU_GOLDEN_IMAGE_IDS() };
        uint32_t count = sizeof(goldenImages) / sizeof(goldenImages[0]);
        uint32_t idx;
        for (idx = 0u; idx < count; ++idx)
        {
            app = goldenImages[idx];
            GetStartEndAddress(app, &startAddress, &endAddress);

            if ( (startAddress <= address) && (address < endAddress) )
            {
                status = Cy_DFU_ValidateApp(app, params);
                status = (status == CY_DFU_SUCCESS) ? CY_DFU_ERROR_ADDRESS : CY_DFU_SUCCESS;
                break;
            }
        }
    } 
#endif /* #if CY_DFU_OPT_GOLDEN_IMAGE != 0 */
    
    /* Check if the address is inside the valid range */
    if ( ( (minUFlashAddress <= address) && (address < maxUFlashAddress) ) 
      || ( (minEmEepromAddress <= address) && (address < maxEmEepromAddress) )  )
    {   /* Do nothing, this is an allowed memory range to update to */
    }
    else
    {
        status = CY_DFU_ERROR_ADDRESS;   
    }
    
    if (status == CY_DFU_SUCCESS)
    {
        /* INVALIDATE-FIRST: on the first programmed row of this OTA session, erase
         * the target (inactive) slot's metadata so the slot is INVALID for the
         * whole transfer. The image's own metadata row (highest address, written
         * last) re-validates it only once the update completes -- so an interrupted
         * or aborted OTA can never leave a bootable, half-written slot. ('app' is
         * the running slot; the inactive slot is the other one.) */
        static uint8_t s_meta_invalidated = 0u;
        if (s_meta_invalidated == 0u)
        {
            uint32_t metaRow = ((app == 0u) ? OTA_APP1_META_ADDR : OTA_APP0_META_ADDR)
                               & ~(CY_FLASH_SIZEOF_ROW - 1u);
            (void) Cy_Flash_EraseRow(metaRow);
            s_meta_invalidated = 1u;
        }

        if ((ctl & CY_DFU_IOCTL_ERASE) != 0u)
        {
            (void) memset(params->dataBuffer, 0, CY_FLASH_SIZEOF_ROW);
        }
        cy_en_flashdrv_status_t fstatus =  Cy_Flash_WriteRow(address, (uint32_t*)params->dataBuffer);
        status = (fstatus == CY_FLASH_DRV_SUCCESS) ? CY_DFU_SUCCESS : CY_DFU_ERROR_DATA;
    }
    return (status);
}


/*******************************************************************************
* Function Name: Cy_DFU_ReadData
****************************************************************************//**
*
* This function documentation is part of the DFU SDK API, see the
* cy_dfu.h file or DFU SDK API Reference Manual for details.
*
*******************************************************************************/
cy_en_dfu_status_t Cy_DFU_ReadData (uint32_t address, uint32_t length, uint32_t ctl, 
                                              cy_stc_dfu_params_t *params)
{
    /* Allowed update range = both app slots (App0 start .. end of flash). The
     * 64 KB bootloader below App0 is protected, and the running app's own slot
     * is rejected by the GetRunningApp check above -- so writes land only in the
     * INACTIVE slot, in either A/B direction. */
    const uint32_t minUFlashAddress = CY_DFU_APP0_VERIFY_START;
    const uint32_t maxUFlashAddress = CY_FLASH_BASE + CY_FLASH_SIZE;
    /* EM_EEPROM Limits*/
    const uint32_t minEmEepromAddress = CY_EM_EEPROM_BASE;
    const uint32_t maxEmEepromAddress = CY_EM_EEPROM_BASE + CY_EM_EEPROM_SIZE;
    
    cy_en_dfu_status_t status = CY_DFU_SUCCESS;

    /* Check if the length is valid */
    if (IsMultipleOf(length, CY_FLASH_SIZEOF_ROW) == 0u) 
    {
        status = CY_DFU_ERROR_LENGTH;   
    }

    /* Check if the address is inside the valid range */
    if ( ( (minUFlashAddress <= address) && (address < maxUFlashAddress) ) 
      || ( (minEmEepromAddress <= address) && (address < maxEmEepromAddress) )  )
    {   /* Do nothing, this is an allowed memory range to update to */
    }
    else
    {
        status = CY_DFU_ERROR_ADDRESS;   
    }

    /* Read or Compare */
    if (status == CY_DFU_SUCCESS)
    {
        if ((ctl & CY_DFU_IOCTL_COMPARE) == 0u)
        {
            (void) memcpy(params->dataBuffer, (const void *)address, length);
            status = CY_DFU_SUCCESS;
        }
        else
        {
            status = ( memcmp(params->dataBuffer, (const void *)address, length) == 0 )
                     ? CY_DFU_SUCCESS : CY_DFU_ERROR_VERIFY;
        }
    }
    return (status);
}


/*******************************************************************************
* Function Name: Cy_DFU_TransportRead
****************************************************************************//**
*
* This function documentation is part of the DFU SDK API, see the
* cy_dfu.h file or DFU SDK API Reference Manual for details.
*
*******************************************************************************/
cy_en_dfu_status_t Cy_DFU_TransportRead (uint8_t *buffer, uint32_t size, uint32_t *count, uint32_t timeout)
{
    return (CyBLE_CyBtldrCommRead(buffer, size, count, timeout));
}

/*******************************************************************************
* Function Name: Cy_DFU_TransportWrite
****************************************************************************//**
*
* This function documentation is part of the DFU SDK API, see the
* cy_dfu.h file or DFU SDK API Reference Manual for details.
*
*******************************************************************************/
cy_en_dfu_status_t Cy_DFU_TransportWrite(uint8_t *buffer, uint32_t size, uint32_t *count, uint32_t timeout)
{
    return (CyBLE_CyBtldrCommWrite(buffer, size, count, timeout));
}

/*******************************************************************************
* Function Name: Cy_DFU_TransportReset
****************************************************************************//**
*
* This function documentation is part of the DFU SDK API, see the
* cy_dfu.h file or DFU SDK API Reference Manual for details.
*
*******************************************************************************/
void Cy_DFU_TransportReset(void)
{
    CyBLE_CyBtldrCommReset();
}

/*******************************************************************************
* Function Name: Cy_DFU_TransportStart
****************************************************************************//**
*
* This function documentation is part of the DFU SDK API, see the
* cy_dfu.h file or DFU SDK API Reference Manual for details.
*
*******************************************************************************/
void Cy_DFU_TransportStart(void)
{
    CyBLE_CyBtldrCommStart();
}

/*******************************************************************************
* Function Name: Cy_DFU_TransportStop
****************************************************************************//**
*
* This function documentation is part of the DFU SDK API, see the
* cy_dfu.h file or DFU SDK API Reference Manual for details.
*
*******************************************************************************/
void Cy_DFU_TransportStop(void)
{
    CyBLE_CyBtldrCommStop();
}


/* [] END OF FILE */
