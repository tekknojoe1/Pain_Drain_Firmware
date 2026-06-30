/***************************************************************************//**
* \file CY_BLE_hids_config.c
* \version 2.0
* 
* \brief
*  This file contains the source code of initialization of the config structure 
*  for the HID service.
* 
********************************************************************************
* \copyright
* Copyright 2017-2018, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "ble/cy_ble_hids.h"

#if(CY_BLE_MODE_PROFILE && defined(CY_BLE_HIDS))

#ifdef CY_BLE_HIDS_SERVER

static const cy_stc_ble_hidss_report_t cy_ble_hids1ReportArray[0x02u] = {
    {
        0x0010u, /* Handle of the Report characteristic value */ 
        0x0011u, /* Handle of the Client Characteristic Configuration descriptor */ 
        0x0012u, /* Handle of the Report Reference descriptor */ 
    },
    {
        0x001Fu, /* Handle of the Report characteristic value */ 
        CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Client Characteristic Configuration descriptor */ 
        0x0020u, /* Handle of the Report Reference descriptor */ 
    },
};

static const cy_stc_ble_hidss_t cy_ble_hidss[0x01u] = {
    {
        0x000Cu, /* Handle of the HID service */ 
        0x0018u, /* Handle of the Protocol Mode characteristic */ 
        0x02u, /* Number of report characteristics */ 
        cy_ble_hids1ReportArray, /* Information about report characteristics */ 
        {
            {
                0x001Au, /* Handle of the Report characteristic value */ 
                0x001Bu, /* Handle of the Client Characteristic Configuration descriptor */ 
                CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Report Reference descriptor */ 
            },
            {
                0x001Du, /* Handle of the Report characteristic value */ 
                CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Client Characteristic Configuration descriptor */ 
                CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Report Reference descriptor */ 
            },
            {
                CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Report characteristic value */ 
                CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Client Characteristic Configuration descriptor */ 
                CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Report Reference descriptor */ 
            },
        }, /* Information about boot report characteristics */ 
        0x000Eu, /* Handle of the Report Map characteristic */ 
        CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Report Map External Report Reference descriptor */ 
        0x0014u, /* Handle of the HID Information characteristic */ 
        0x0016u, /* Handle of the HID Control Point characteristic */ 
    },
};

#endif /* CY_BLE_HIDS_SERVER */

/**
* \addtogroup group_globals
* @{
*/

/** The configuration structure for the HID service. */
cy_stc_ble_hids_config_t cy_ble_hidsConfig =
{
    /* Service GATT DB handles structure */
    #ifdef CY_BLE_HIDS_SERVER
    .hidss = cy_ble_hidss,
    #else
    .hidss = NULL,
    #endif /* CY_BLE_HIDS_SERVER */

    /* An application layer event callback function to receive service events from the BLE Component. */
    .callbackFunc = NULL,
};

/** @} group_globals */

#endif /* (CY_BLE_MODE_PROFILE && defined(CY_BLE_HIDS)) */

/* [] END OF FILE */
