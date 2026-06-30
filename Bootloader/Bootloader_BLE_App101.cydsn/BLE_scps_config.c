/***************************************************************************//**
* \file CY_BLE_scps_config.c
* \version 2.0
* 
* \brief
*  This file contains the source code of initialization of the config structure
*  for the Scan Parameter Service.
* 
********************************************************************************
* \copyright
* Copyright 2017-2018, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "ble/cy_ble_scps.h"

#if(CY_BLE_MODE_PROFILE && defined(CY_BLE_SCPS))

#ifdef CY_BLE_SCPS_SERVER
static const cy_stc_ble_scpss_t cy_ble_scpss =
{
    0x002Du,    /* Handle of the SCPS service */
    0x002Fu,    /* Handle of the Scan Interval Window  characteristic */
    0x0031u,    /* Handle of the Scan Refresh characteristic */
    0x0032u,    /* Handle of the Client Characteristic Configuration descriptor */
};
#endif /* CY_BLE_SCPS_SERVER */

/**
* \addtogroup group_globals
* @{
*/

/** The configuration structure for the Scan Parameter Service. */
cy_stc_ble_scps_config_t cy_ble_scpsConfig =
{
    /* Service GATT DB handles structure */
    #ifdef CY_BLE_SCPS_SERVER
    .scpss = &cy_ble_scpss,
    #else
    .scpss = NULL,
    #endif /* CY_BLE_SCPS_SERVER */

    /* An application layer event callback function to receive service events from the BLE Component. */
    .callbackFunc = NULL,
};

/** @} group_globals */

#endif /* (CY_BLE_MODE_PROFILE && defined(CY_BLE_SCPS)) */

/* [] END OF FILE */
