/***************************************************************************//**
* \file CY_BLE.c
* \version 2.0
* 
* \brief
*  This file contains the source code of initialization of the config structure
*  for BLE.
* 
********************************************************************************
* \copyright
* Copyright 2017-2018, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "BLE_config.h"

#if CY_BLE_MODE_PROFILE
#include "flash/cy_flash.h"
#include "ble/cy_ble_event_handler.h"

    
/***************************************
* Global Variables
***************************************/
/* Initializes the cy_stc_ble_gapp_disc_mode_info_t cy_ble_discoveryModeInfo  structure */
#if(CY_BLE_GAP_ROLE_PERIPHERAL || CY_BLE_GAP_ROLE_BROADCASTER)
static cy_stc_ble_gapp_adv_params_t cy_ble_gappAdvConfig[0x01u] = {

    /* Peripheral configuration 0 */
    {
        .fastAdvIntervalMin                 = 0x0020u, 
        .fastAdvIntervalMax                 = 0x0020u, 
        .fastAdvTimeOut                     = 0x001Eu, 
        .slowAdvEnable                      = 0x01u, 
        .slowAdvIntervalMin                 = 0x0640u, 
        .slowAdvIntervalMax                 = 0x0640u, 
        .slowAdvTimeOut                     = 0x0096u, 
    },
};


cy_stc_ble_gapp_disc_param_t cy_ble_discoveryParam[0x01u] = {

    /* Peripheral configuration 0 */
    {
        0x0020u, /* uint16_t advertising_interval_min */ 
        0x0020u, /* uint16_t advertising_interval_max */ 
        CY_BLE_GAPP_CONNECTABLE_UNDIRECTED_ADV, /* uint8_t advertising_type */ 
        0x00u, /* uint8_t own_addr_type */ 
        0x00u, /* uint8_t direct_addr_type */ 
        {0x00u, 0x00u, 0x00u, 0x50u, 0xA0u, 0x00u}, /* uint8_t* direct_addr */ 
        0x07u, /* uint8_t advertising_channel_map */ 
        0x00u, /* uint8_t advertising_filter_policy */ 
    },
};


cy_stc_ble_gapp_disc_data_t cy_ble_discoveryData[0x01u] = {

    /* Peripheral configuration 0 */
    {
        { 0x02u, 0x01u, 0x05u, 0x0Du, 0x09u, 0x42u, 0x4Cu,
        0x45u, 0x20u, 0x4Bu, 0x65u, 0x79u, 0x62u, 0x6Fu,
        0x61u, 0x72u, 0x64u, 0x09u, 0x02u, 0x12u, 0x18u,
        0x0Au, 0x18u, 0x0Fu, 0x18u, 0x13u, 0x18u, 0x03u,
        0x19u, 0xC1u, 0x03u }, /* uint8_t advertising_data[CY_BLE_MAX_ADV_DATA_LEN] */ 
        0x1Fu, /* uint8_t adv_data_length */ 
    },
};


cy_stc_ble_gapp_scan_rsp_data_t cy_ble_scanRspData[0x01u] = {

    /* Peripheral configuration 0 */
    {
        { 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u }, /* uint8_t scan_rsp_data[CY_BLE_MAX_SCAN_RSP_DATA_LEN] */ 
        0x00u, /* uint8_t scan_rsp_data_length */ 
    },
};


/* This array of type cy_stc_ble_gapp_disc_mode_info_t is present only when the 
   BLE component is configured for Peripheral or Broadcaster GAP roles. 
   It contains the Advertisement settings and also the Advertisement and 
   Scan response data parameters entered in the customizer. This variable
   can be used by advanced users to change Advertisement settings,
   Advertisement or Scan response data in runtime. 
*/
cy_stc_ble_gapp_disc_mode_info_t cy_ble_discoveryModeInfo[0x01u] = {

    /* Peripheral configuration 0 */
    {
        0x01u, /* uint8_t discMode */ 
        &cy_ble_discoveryParam[0], 
        &cy_ble_discoveryData[0], 
        &cy_ble_scanRspData[0], 
        0x001Eu, /* uint16_t advTo */ 
    },
};


#endif /* CY_BLE_GAP_ROLE_PERIPHERAL || CY_BLE_GAP_ROLE_BROADCASTER */

/* Initializes the cy_stc_ble_gapc_disc_info_t  cy_ble_discoveryInfo  structure */
#if(CY_BLE_GAP_ROLE_CENTRAL || CY_BLE_GAP_ROLE_OBSERVER)

#endif /* CY_BLE_GAP_ROLE_CENTRAL || CY_BLE_GAP_ROLE_OBSERVER */

#if ((CY_BLE_MODE_PROFILE) && (CY_BLE_BONDING_REQUIREMENT == CY_BLE_BONDING_YES))

#if(CY_BLE_MODE_PROFILE)
    CY_SECTION(".cy_em_eeprom") CY_ALIGN(CY_FLASH_SIZEOF_ROW) static const cy_stc_ble_flash_storage_t cy_ble_flashStorage =
    {
        { 0x00u }, 
        {{
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u /* CRC */
        },
        {
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u /* CRC */
        },
        {
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u /* CRC */
        },
        {
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u /* CRC */
        },
        {
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u /* CRC */
        },
        {
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u /* CRC */
        },
        {
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u /* CRC */
        },
        {
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u /* CRC */
        },
        {
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u /* CRC */
        },
        {
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u /* CRC */
        },
        {
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u /* CRC */
        },
        {
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u /* CRC */
        },
        {
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u /* CRC */
        },
        {
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u /* CRC */
        },
        {
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u /* CRC */
        },
        {
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u /* CRC */
        },
        {
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u /* CRC */
        }}, 
        0x0Au, /* CY_BLE_GATT_DB_CCCD_COUNT */ 
        0x11u, 
    };
#endif /* (CY_BLE_MODE_PROFILE) */

#endif  /* (CY_BLE_MODE_PROFILE) && (CY_BLE_BONDING_REQUIREMENT == CY_BLE_BONDING_YES) */

#if(CY_BLE_GATT_ROLE_SERVER)
static const cy_stc_ble_gatts_t cy_ble_gatts =
{
    0x0008u,    /* Handle of the GATT service */
    0x000Au,    /* Handle of the Service Changed characteristic */
    0x000Bu,    /* Handle of the Client Characteristic Configuration descriptor */
};
static const cy_stc_ble_gaps_t cy_ble_gaps =
{
    0x0001u,    /* Handle of the GAP service */
    0x0003u,    /* Handle of the Device Name characteristic */
    0x0005u,    /* Handle of the Appearance characteristic */
    0x0007u,    /* Handle of the Peripheral Preferred Connection Parameters characteristic */
    CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Central Address Resolution characteristic */
    CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Resolvable Private Address Only characteristic */
};
static uint8_t cy_ble_attValues[0xA3u] = {
    /* Device Name */
    (uint8_t)'B', (uint8_t)'L', (uint8_t)'E', (uint8_t)' ', (uint8_t)'K', (uint8_t)'e', (uint8_t)'y', (uint8_t)'b',
(uint8_t)'o', (uint8_t)'a', (uint8_t)'r', (uint8_t)'d', 

    /* Appearance */
    0xC1u, 0x03u, 

    /* Peripheral Preferred Connection Parameters */
    0x50u, 0x00u, 0x80u, 0x00u, 0x0Au, 0x00u, 0xFFu, 0x01u, 

    /* Service Changed */
    0x00u, 0x00u, 0x00u, 0x00u, 

    /* Report Map */
    
0x05u, 0x01u, /* USAGE_PAGE         */
0x09u, 0x06u, /* USAGE              */
0xA1u, 0x01u, /* COLLECTION        
*/
0x05u, 0x07u, /* USAGE_PAGE         */
0x19u, 0xE0u, /* USAGE_MINIMUM      */
0x29u, 0xE7u, /* USAGE_MAXIMUM     
*/
0x15u, 0x00u, /* LOGICAL_MINIMUM    */
0x25u, 0x01u, /* LOGICAL_MAXIMUM    */
0x75u, 0x01u, /* REPORT_SIZE       
*/
0x95u, 0x08u, /* REPORT_COUNT       */
0x81u, 0x02u, /* INPUT              */
0x95u, 0x01u, /* REPORT_COUNT      
*/
0x75u, 0x08u, /* REPORT_SIZE        */
0x81u, 0x01u, /* INPUT              */
0x95u, 0x05u, /* REPORT_COUNT      
*/
0x75u, 0x01u, /* REPORT_SIZE        */
0x05u, 0x08u, /* USAGE_PAGE         */
0x19u, 0x01u, /* USAGE_MINIMUM     
*/
0x29u, 0x05u, /* USAGE_MAXIMUM      */
0x91u, 0x02u, /* OUTPUT             */
0x95u, 0x01u, /* REPORT_COUNT      
*/
0x75u, 0x03u, /* REPORT_SIZE        */
0x91u, 0x01u, /* OUTPUT             */
0x95u, 0x06u, /* REPORT_COUNT      
*/
0x75u, 0x08u, /* REPORT_SIZE        */
0x15u, 0x00u, /* LOGICAL_MINIMUM    */
0x25u, 0x65u, /* LOGICAL_MAXIMUM   
*/
0x05u, 0x07u, /* USAGE_PAGE         */
0x19u, 0x00u, /* USAGE_MINIMUM      */
0x29u, 0x65u, /* USAGE_MAXIMUM     
*/
0x81u, 0x00u, /* INPUT              */
0xC0u,      /* END_COLLECTION     */

    /* Report In */
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 

    /* Report Reference */
    0x00u, 0x01u, 

    /* HID Information */
    0x01u, 0x01u, 0x00u, 0x03u, 

    /* HID Control Point */
    0x00u, 

    /* Protocol Mode */
    0x01u, 

    /* Boot Keyboard Input Report */
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 

    /* Boot Keyboard Output Report */
    0x00u, 

    /* Report Out */
    0x00u, 

    /* Report Reference */
    0x00u, 0x02u, 

    /* Manufacturer Name String */
    (uint8_t)'C', (uint8_t)'y', (uint8_t)'p', (uint8_t)'r', (uint8_t)'e', (uint8_t)'s', (uint8_t)'s', (uint8_t)' ',
(uint8_t)'S', (uint8_t)'e', (uint8_t)'m', (uint8_t)'i', (uint8_t)'c', (uint8_t)'o', (uint8_t)'n', (uint8_t)'d',
(uint8_t)'u', (uint8_t)'c', (uint8_t)'t', (uint8_t)'o', (uint8_t)'r', 

    /* Firmware Revision String */
    (uint8_t)'1', (uint8_t)'.', (uint8_t)'0', (uint8_t)'0', 

    /* PnP ID */
    0x02u, 0xB4u, 0x04u, 0x01u, 0x00u, 0x01u, 0x00u, 

    /* Battery Level */
    0x64u, 

    /* Characteristic Presentation Format */
    0x04u, 0x01u, 0xADu, 0x27u, 0x01u, 0x06u, 0x01u, 

    /* Scan Interval Window */
    0x00u, 0x00u, 0x00u, 0x00u, 

    /* Scan Refresh */
    0x00u, 

    /* Alert Level */
    0x00u, 

};
#if(CY_BLE_GATT_DB_CCCD_COUNT != 0u)
static uint8_t cy_ble_attValuesCCCD[CY_BLE_GATT_DB_CCCD_COUNT];
#endif /* CY_BLE_GATT_DB_CCCD_COUNT != 0u */

static cy_stc_ble_gatts_att_gen_val_len_t cy_ble_attValuesLen[0x1Bu] = {
    { 0x000Cu, (void *)&cy_ble_attValues[0] }, /* Device Name */
    { 0x0002u, (void *)&cy_ble_attValues[12] }, /* Appearance */
    { 0x0008u, (void *)&cy_ble_attValues[14] }, /* Peripheral Preferred Connection Parameters */
    { 0x0004u, (void *)&cy_ble_attValues[22] }, /* Service Changed */
    { 0x0002u, (void *)&cy_ble_attValuesCCCD[0] }, /* Client Characteristic Configuration */
    { 0x003Fu, (void *)&cy_ble_attValues[26] }, /* Report Map */
    { 0x0008u, (void *)&cy_ble_attValues[89] }, /* Report In */
    { 0x0002u, (void *)&cy_ble_attValuesCCCD[2] }, /* Client Characteristic Configuration */
    { 0x0002u, (void *)&cy_ble_attValues[97] }, /* Report Reference */
    { 0x0004u, (void *)&cy_ble_attValues[99] }, /* HID Information */
    { 0x0001u, (void *)&cy_ble_attValues[103] }, /* HID Control Point */
    { 0x0001u, (void *)&cy_ble_attValues[104] }, /* Protocol Mode */
    { 0x0008u, (void *)&cy_ble_attValues[105] }, /* Boot Keyboard Input Report */
    { 0x0002u, (void *)&cy_ble_attValuesCCCD[4] }, /* Client Characteristic Configuration */
    { 0x0001u, (void *)&cy_ble_attValues[113] }, /* Boot Keyboard Output Report */
    { 0x0001u, (void *)&cy_ble_attValues[114] }, /* Report Out */
    { 0x0002u, (void *)&cy_ble_attValues[115] }, /* Report Reference */
    { 0x0015u, (void *)&cy_ble_attValues[117] }, /* Manufacturer Name String */
    { 0x0004u, (void *)&cy_ble_attValues[138] }, /* Firmware Revision String */
    { 0x0007u, (void *)&cy_ble_attValues[142] }, /* PnP ID */
    { 0x0001u, (void *)&cy_ble_attValues[149] }, /* Battery Level */
    { 0x0007u, (void *)&cy_ble_attValues[150] }, /* Characteristic Presentation Format */
    { 0x0002u, (void *)&cy_ble_attValuesCCCD[6] }, /* Client Characteristic Configuration */
    { 0x0004u, (void *)&cy_ble_attValues[157] }, /* Scan Interval Window */
    { 0x0001u, (void *)&cy_ble_attValues[161] }, /* Scan Refresh */
    { 0x0002u, (void *)&cy_ble_attValuesCCCD[8] }, /* Client Characteristic Configuration */
    { 0x0001u, (void *)&cy_ble_attValues[162] }, /* Alert Level */
};

static const cy_stc_ble_gatts_db_t cy_ble_gattDB[0x35u] = {
    { 0x0001u, 0x2800u /* Primary service                     */, 0x00000001u /*           */, 0x0007u, {{0x1800u, NULL}}                           },
    { 0x0002u, 0x2803u /* Characteristic                      */, 0x00020001u /* rd        */, 0x0003u, {{0x2A00u, NULL}}                           },
    { 0x0003u, 0x2A00u /* Device Name                         */, 0x01020001u /* rd        */, 0x0003u, {{0x000Cu, (void *)&cy_ble_attValuesLen[0]}} },
    { 0x0004u, 0x2803u /* Characteristic                      */, 0x00020001u /* rd        */, 0x0005u, {{0x2A01u, NULL}}                           },
    { 0x0005u, 0x2A01u /* Appearance                          */, 0x01020001u /* rd        */, 0x0005u, {{0x0002u, (void *)&cy_ble_attValuesLen[1]}} },
    { 0x0006u, 0x2803u /* Characteristic                      */, 0x00020001u /* rd        */, 0x0007u, {{0x2A04u, NULL}}                           },
    { 0x0007u, 0x2A04u /* Peripheral Preferred Connection Par */, 0x01020001u /* rd        */, 0x0007u, {{0x0008u, (void *)&cy_ble_attValuesLen[2]}} },
    { 0x0008u, 0x2800u /* Primary service                     */, 0x00000001u /*           */, 0x000Bu, {{0x1801u, NULL}}                           },
    { 0x0009u, 0x2803u /* Characteristic                      */, 0x00200001u /* ind       */, 0x000Bu, {{0x2A05u, NULL}}                           },
    { 0x000Au, 0x2A05u /* Service Changed                     */, 0x01200000u /* ind       */, 0x000Bu, {{0x0004u, (void *)&cy_ble_attValuesLen[3]}} },
    { 0x000Bu, 0x2902u /* Client Characteristic Configuration */, 0x030A0301u /* rd,wr     */, 0x000Bu, {{0x0002u, (void *)&cy_ble_attValuesLen[4]}} },
    { 0x000Cu, 0x2800u /* Primary service                     */, 0x00000001u /*           */, 0x0020u, {{0x1812u, NULL}}                           },
    { 0x000Du, 0x2803u /* Characteristic                      */, 0x00020001u /* rd        */, 0x000Eu, {{0x2A4Bu, NULL}}                           },
    { 0x000Eu, 0x2A4Bu /* Report Map                          */, 0x01020003u /* rd        */, 0x000Eu, {{0x003Fu, (void *)&cy_ble_attValuesLen[5]}} },
    { 0x000Fu, 0x2803u /* Characteristic                      */, 0x001A0001u /* rd,wr,ntf */, 0x0012u, {{0x2A4Du, NULL}}                           },
    { 0x0010u, 0x2A4Du /* Report In                           */, 0x011A0303u /* rd,wr,ntf */, 0x0012u, {{0x0008u, (void *)&cy_ble_attValuesLen[6]}} },
    { 0x0011u, 0x2902u /* Client Characteristic Configuration */, 0x030A0301u /* rd,wr     */, 0x0011u, {{0x0002u, (void *)&cy_ble_attValuesLen[7]}} },
    { 0x0012u, 0x2908u /* Report Reference                    */, 0x01020001u /* rd        */, 0x0012u, {{0x0002u, (void *)&cy_ble_attValuesLen[8]}} },
    { 0x0013u, 0x2803u /* Characteristic                      */, 0x00020001u /* rd        */, 0x0014u, {{0x2A4Au, NULL}}                           },
    { 0x0014u, 0x2A4Au /* HID Information                     */, 0x01020003u /* rd        */, 0x0014u, {{0x0004u, (void *)&cy_ble_attValuesLen[9]}} },
    { 0x0015u, 0x2803u /* Characteristic                      */, 0x00040001u /* wwr       */, 0x0016u, {{0x2A4Cu, NULL}}                           },
    { 0x0016u, 0x2A4Cu /* HID Control Point                   */, 0x01040300u /* wwr       */, 0x0016u, {{0x0001u, (void *)&cy_ble_attValuesLen[10]}} },
    { 0x0017u, 0x2803u /* Characteristic                      */, 0x00060001u /* rd,wwr    */, 0x0018u, {{0x2A4Eu, NULL}}                           },
    { 0x0018u, 0x2A4Eu /* Protocol Mode                       */, 0x01060303u /* rd,wwr    */, 0x0018u, {{0x0001u, (void *)&cy_ble_attValuesLen[11]}} },
    { 0x0019u, 0x2803u /* Characteristic                      */, 0x00120001u /* rd,ntf    */, 0x001Bu, {{0x2A22u, NULL}}                           },
    { 0x001Au, 0x2A22u /* Boot Keyboard Input Report          */, 0x01120003u /* rd,ntf    */, 0x001Bu, {{0x0008u, (void *)&cy_ble_attValuesLen[12]}} },
    { 0x001Bu, 0x2902u /* Client Characteristic Configuration */, 0x030A0301u /* rd,wr     */, 0x001Bu, {{0x0002u, (void *)&cy_ble_attValuesLen[13]}} },
    { 0x001Cu, 0x2803u /* Characteristic                      */, 0x000E0001u /* rd,wr,wwr */, 0x001Du, {{0x2A32u, NULL}}                           },
    { 0x001Du, 0x2A32u /* Boot Keyboard Output Report         */, 0x010E0303u /* rd,wr,wwr */, 0x001Du, {{0x0001u, (void *)&cy_ble_attValuesLen[14]}} },
    { 0x001Eu, 0x2803u /* Characteristic                      */, 0x000E0001u /* rd,wr,wwr */, 0x0020u, {{0x2A4Du, NULL}}                           },
    { 0x001Fu, 0x2A4Du /* Report Out                          */, 0x010E0303u /* rd,wr,wwr */, 0x0020u, {{0x0001u, (void *)&cy_ble_attValuesLen[15]}} },
    { 0x0020u, 0x2908u /* Report Reference                    */, 0x01020001u /* rd        */, 0x0020u, {{0x0002u, (void *)&cy_ble_attValuesLen[16]}} },
    { 0x0021u, 0x2800u /* Primary service                     */, 0x00000001u /*           */, 0x0027u, {{0x180Au, NULL}}                           },
    { 0x0022u, 0x2803u /* Characteristic                      */, 0x00020001u /* rd        */, 0x0023u, {{0x2A29u, NULL}}                           },
    { 0x0023u, 0x2A29u /* Manufacturer Name String            */, 0x01020003u /* rd        */, 0x0023u, {{0x0015u, (void *)&cy_ble_attValuesLen[17]}} },
    { 0x0024u, 0x2803u /* Characteristic                      */, 0x00020001u /* rd        */, 0x0025u, {{0x2A26u, NULL}}                           },
    { 0x0025u, 0x2A26u /* Firmware Revision String            */, 0x01020003u /* rd        */, 0x0025u, {{0x0004u, (void *)&cy_ble_attValuesLen[18]}} },
    { 0x0026u, 0x2803u /* Characteristic                      */, 0x00020001u /* rd        */, 0x0027u, {{0x2A50u, NULL}}                           },
    { 0x0027u, 0x2A50u /* PnP ID                              */, 0x01020003u /* rd        */, 0x0027u, {{0x0007u, (void *)&cy_ble_attValuesLen[19]}} },
    { 0x0028u, 0x2800u /* Primary service                     */, 0x00000001u /*           */, 0x002Cu, {{0x180Fu, NULL}}                           },
    { 0x0029u, 0x2803u /* Characteristic                      */, 0x00120001u /* rd,ntf    */, 0x002Cu, {{0x2A19u, NULL}}                           },
    { 0x002Au, 0x2A19u /* Battery Level                       */, 0x01120003u /* rd,ntf    */, 0x002Cu, {{0x0001u, (void *)&cy_ble_attValuesLen[20]}} },
    { 0x002Bu, 0x2904u /* Characteristic Presentation Format  */, 0x01020001u /* rd        */, 0x002Bu, {{0x0007u, (void *)&cy_ble_attValuesLen[21]}} },
    { 0x002Cu, 0x2902u /* Client Characteristic Configuration */, 0x030A0301u /* rd,wr     */, 0x002Cu, {{0x0002u, (void *)&cy_ble_attValuesLen[22]}} },
    { 0x002Du, 0x2800u /* Primary service                     */, 0x00000001u /*           */, 0x0032u, {{0x1813u, NULL}}                           },
    { 0x002Eu, 0x2803u /* Characteristic                      */, 0x00040001u /* wwr       */, 0x002Fu, {{0x2A4Fu, NULL}}                           },
    { 0x002Fu, 0x2A4Fu /* Scan Interval Window                */, 0x01040300u /* wwr       */, 0x002Fu, {{0x0004u, (void *)&cy_ble_attValuesLen[23]}} },
    { 0x0030u, 0x2803u /* Characteristic                      */, 0x00100001u /* ntf       */, 0x0032u, {{0x2A31u, NULL}}                           },
    { 0x0031u, 0x2A31u /* Scan Refresh                        */, 0x01100000u /* ntf       */, 0x0032u, {{0x0001u, (void *)&cy_ble_attValuesLen[24]}} },
    { 0x0032u, 0x2902u /* Client Characteristic Configuration */, 0x030A0301u /* rd,wr     */, 0x0032u, {{0x0002u, (void *)&cy_ble_attValuesLen[25]}} },
    { 0x0033u, 0x2800u /* Primary service                     */, 0x00000001u /*           */, 0x0035u, {{0x1802u, NULL}}                           },
    { 0x0034u, 0x2803u /* Characteristic                      */, 0x00040001u /* wwr       */, 0x0035u, {{0x2A06u, NULL}}                           },
    { 0x0035u, 0x2A06u /* Alert Level                         */, 0x01040300u /* wwr       */, 0x0035u, {{0x0001u, (void *)&cy_ble_attValuesLen[26]}} },
};

#endif /* (CY_BLE_GATT_ROLE_SERVER) */

/* Default device security */
#if (CY_BLE_MODE_PROFILE) 
    cy_stc_ble_gap_auth_info_t cy_ble_authInfo[0x01u] = {

    /* Security configuration 0 */
    {
        .security = (CY_BLE_GAP_SEC_MODE_1 | CY_BLE_GAP_SEC_LEVEL_2 ), 
        .bonding = CY_BLE_GAP_BONDING, 
        .ekeySize = 0x10u, 
        .authErr = CY_BLE_GAP_AUTH_ERROR_NONE, 
        .pairingProperties = 0x00u, 
    },
};

#endif /* CY_BLE_MODE_PROFILE */

#if(CY_BLE_GAP_ROLE_CENTRAL || CY_BLE_GAP_ROLE_PERIPHERAL)

/** Initialize BLE configuration parameters structure */
static const cy_stc_ble_params_t cy_ble_params =
{
        .txPowerLevelAdv                    = CY_BLE_LL_PWR_LVL_0_DBM,
        .txPowerLevelConn                   = CY_BLE_LL_PWR_LVL_0_DBM,

        .securityIoCapability               = CY_BLE_GAP_IOCAP_NOINPUT_NOOUTPUT,
        .securityPairingMethod              = 0x00u,
    
        .siliconDeviceAddressEnabled        = 0x01u,
    
        .gattDbIndexCount                   = 0x0035u,
};
#endif  /* (CY_BLE_GAP_ROLE_CENTRAL || CY_BLE_GAP_ROLE_PERIPHERAL) */

cy_stc_ble_gap_bd_addr_t cy_ble_deviceAddress = {{0x00u, 0x00u, 0x00u, 0x50u, 0xA0u, 0x00u}, 0x00u };

/**
* \addtogroup group_globals
* @{
*/

/** The configuration structure for BLE */
cy_stc_ble_config_t cy_ble_config =
{
    /* Initialize the GAPP structures */
    /* Initialize the cy_stc_ble_gapp_disc_mode_info_t cy_ble_discoveryModeInfo structure */
    #if(CY_BLE_GAP_ROLE_PERIPHERAL || CY_BLE_GAP_ROLE_BROADCASTER)
        .discoveryModeInfo = cy_ble_discoveryModeInfo,
        .gappAdvParams = cy_ble_gappAdvConfig,
    #else
        .discoveryModeInfo = NULL,
    #endif /* CY_BLE_GAP_ROLE_PERIPHERAL || CY_BLE_GAP_ROLE_BROADCASTER */

    /* Initialize the cy_stc_ble_gapc_disc_info_t  cy_ble_discoveryInfo  structure */
    #if(CY_BLE_GAP_ROLE_CENTRAL || CY_BLE_GAP_ROLE_OBSERVER)
        .discoveryInfo = cy_ble_discoveryInfo,
        .gapcScanParams = cy_ble_gapcScanConfig,
    #else
        .discoveryInfo = NULL,
    #endif /* CY_BLE_GAP_ROLE_CENTRAL || CY_BLE_GAP_ROLE_OBSERVER */

    /* Initialize the GATT structures */
    #if((CY_BLE_GAP_ROLE_CENTRAL || CY_BLE_GAP_ROLE_PERIPHERAL) && (CY_BLE_BONDING_REQUIREMENT == CY_BLE_BONDING_YES))
        .flashStorage = &cy_ble_flashStorage,
    #else
        .flashStorage = NULL,
    #endif /* CY_BLE_GAP_ROLE_CENTRAL || CY_BLE_GAP_ROLE_PERIPHERAL) && (CY_BLE_BONDING_REQUIREMENT == CY_BLE_BONDING_YES */

    #if(CY_BLE_GATT_ROLE_SERVER)
        .gatts            = &cy_ble_gatts,
        .gaps             = &cy_ble_gaps,
        .attValuesCCCD    = cy_ble_attValuesCCCD,
        .gattDB           = cy_ble_gattDB,
    #else
        .gatts            = NULL,
        .attValuesCCCD    = NULL,
        .gattDB           = NULL,
    #endif /* CY_BLE_GATT_ROLE_SERVER */

    #if(CY_BLE_GAP_ROLE_CENTRAL || CY_BLE_GAP_ROLE_PERIPHERAL)
        /* Initialize the device security structure */
        .authInfo = cy_ble_authInfo,
    #else
        .authInfo = NULL,
    #endif /* (CY_BLE_GAP_ROLE_CENTRAL || CY_BLE_GAP_ROLE_PERIPHERAL */

    #if(CY_BLE_GAP_ROLE_CENTRAL || CY_BLE_GAP_ROLE_PERIPHERAL)
        /* Initialize the BLE configuration parameters structure */
        .params   = &cy_ble_params,
    #else
        .params = NULL,
    #endif /* (CY_BLE_GAP_ROLE_CENTRAL || CY_BLE_GAP_ROLE_PERIPHERAL */

    /* An application layer event callback function to receive service events from the BLE Component. */
    .callbackFunc   = NULL,

    .deviceAddress  = &cy_ble_deviceAddress,
};

/** @} group_globals */

#endif /* CY_BLE_MODE_PROFILE */

/* [] END OF FILE */
