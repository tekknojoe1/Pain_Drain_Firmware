/*******************************************************************************
* File Name: host_main.c
*
* Version 1.10
*
* Description:
*  This example demonstrates the basic use of the BLE PDL Middleware and
*  BLE Configuration Tool. This design implements a Bluetooth Low Energy (BLE)
*  Find Me Profile (FMP) that consists of an Immediate Alert Service (IAS).
*
* Hardware Dependency:
*  CY8CKIT-062 PSoC6 BLE Pioneer Kit
*  CY8CPROTO-063-BLE PSoC 6 BLE Prototyping Kit 
*
******************************************************************************
* Copyright (2018), Cypress Semiconductor Corporation.
******************************************************************************
* This software is owned by Cypress Semiconductor Corporation (Cypress) and is
* protected by and subject to worldwide patent protection (United States and
* foreign), United States copyright laws and international treaty provisions.
* Cypress hereby grants to licensee a personal, non-exclusive, non-transferable
* license to copy, use, modify, create derivative works of, and compile the
* Cypress Source Code and derivative works for the sole purpose of creating
* custom software in support of licensee product to be used only in conjunction
* with a Cypress integrated circuit as specified in the applicable agreement.
* Any reproduction, modification, translation, compilation, or representation of
* this software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH
* REGARD TO THIS MATERIAL, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* Cypress reserves the right to make changes without further notice to the
* materials described herein. Cypress does not assume any liability arising out
* of the application or use of any product or circuit described herein. Cypress
* does not authorize its products for use as critical components in life-support
* systems where a malfunction or failure may reasonably be expected to result in
* significant injury to the user. The inclusion of Cypress' product in a life-
* support systems application implies that the manufacturer assumes all risk of
* such use and in doing so indemnifies Cypress against all charges. Use may be
* limited by and subject to the applicable Cypress software license agreement.
*****************************************************************************/


#include "common.h"
#include "user_interface.h"
#include "ias.h"
#include "power.h"
#include "debug.h"
//#include 

static cy_stc_ble_timer_info_t     timerParam = { .timeout = ADV_TIMER_TIMEOUT };        
static volatile uint32_t           mainTimer  = 1u;

static int loopcount = 0;

int newValue = 0;
/*******************************************************************************
* Function Name: AppCallBack
********************************************************************************
*
* Summary:
*   This is an event callback function to receive events from the BLE Component.
*
* Parameters:
*   event      - The event code
*   eventParam - The event parameters
*
*******************************************************************************/
void AppCallBack(uint32 event, void *eventParam)
{
    static int CHAR_MAX_LEN = 20;
    static cy_stc_ble_gap_sec_key_info_t keyInfo =
    {
        .localKeysFlag    = CY_BLE_GAP_SMP_INIT_ENC_KEY_DIST | 
                            CY_BLE_GAP_SMP_INIT_IRK_KEY_DIST | 
                            CY_BLE_GAP_SMP_INIT_CSRK_KEY_DIST,
        
        .exchangeKeysFlag = CY_BLE_GAP_SMP_INIT_ENC_KEY_DIST | 
                            CY_BLE_GAP_SMP_INIT_IRK_KEY_DIST | 
                            CY_BLE_GAP_SMP_INIT_CSRK_KEY_DIST |
                            CY_BLE_GAP_SMP_RESP_ENC_KEY_DIST |
                            CY_BLE_GAP_SMP_RESP_IRK_KEY_DIST |
                            CY_BLE_GAP_SMP_RESP_CSRK_KEY_DIST,
    };
    
    switch(event)
    {
        
        DBG_PRINTF("e=%d\r\n", event);
        
        /* Mandatory events to be handled by Find Me Target design */
        case CY_BLE_EVT_STACK_ON:
            Cy_BLE_GAP_GenerateKeys(&keyInfo);
            
        case CY_BLE_EVT_GAP_DEVICE_DISCONNECTED:                             
            /* Start BLE advertisement for 180 seconds and update link status on LEDs */
            Cy_BLE_GAPP_StartAdvertisement(CY_BLE_ADVERTISING_FAST, CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX);
            IasSetAlertLevel(NO_ALERT);
            break;

        case CY_BLE_EVT_GAP_DEVICE_CONNECTED:
            /* BLE link is established */
            keyInfo.SecKeyParam.bdHandle = (*(cy_stc_ble_gap_connected_param_t *)eventParam).bdHandle;
            Cy_BLE_GAP_SetSecurityKeys(&keyInfo);
            UpdateLedState();   
            break;

        case CY_BLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
            if(Cy_BLE_GetAdvertisementState() == CY_BLE_ADV_STATE_STOPPED)
            {   
                /* Fast and slow advertising period complete, go to low power  
                 * mode (Hibernate) and wait for an external
                 * user event to wake up the device again */
                UpdateLedState();   
                Cy_BLE_Stop();             
            }
            break;

        /* Other events that are generated by the BLE Component that
         * are not required for functioning of this design */
        /**********************************************************
        *                       General Events
        ***********************************************************/
        case CY_BLE_EVT_TIMEOUT: 
            if((((cy_stc_ble_timeout_param_t *)eventParam)->reasonCode == CY_BLE_GENERIC_APP_TO) && 
               (((cy_stc_ble_timeout_param_t *)eventParam)->timerHandle == timerParam.timerHandle))
            {
                /* Update Led State */
                UpdateLedState();
                
                /* Indicate that timer is raised to the main loop */
                mainTimer++;
            }
            break;
            
        case CY_BLE_EVT_HARDWARE_ERROR:    /* This event indicates that some internal HW error has occurred */
            break;

        case CY_BLE_EVT_STACK_SHUTDOWN_COMPLETE:
            /* Hibernate */
            //UpdateLedState();
            //Cy_SysPm_Hibernate(); //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
            power_flags_update(POWER_FLAG_BLE, 0);  //Turn off ble power flag
            break;
            
        /**********************************************************
        *                       GAP Events
        ***********************************************************/
        case CY_BLE_EVT_GAP_AUTH_REQ:
            if(cy_ble_configPtr->authInfo[CY_BLE_SECURITY_CONFIGURATION_0_INDEX].security == 
                (CY_BLE_GAP_SEC_MODE_1 | CY_BLE_GAP_SEC_LEVEL_1))
            {
                cy_ble_configPtr->authInfo[CY_BLE_SECURITY_CONFIGURATION_0_INDEX].authErr =
                    CY_BLE_GAP_AUTH_ERROR_PAIRING_NOT_SUPPORTED;
            }    
            
            cy_ble_configPtr->authInfo[CY_BLE_SECURITY_CONFIGURATION_0_INDEX].bdHandle = 
                ((cy_stc_ble_gap_auth_info_t *)eventParam)->bdHandle;

            if(Cy_BLE_GAPP_AuthReqReply(&cy_ble_configPtr->authInfo[CY_BLE_SECURITY_CONFIGURATION_0_INDEX]) != 
                    CY_BLE_SUCCESS)
            {
                Cy_BLE_GAP_RemoveOldestDeviceFromBondedList();
                Cy_BLE_GAPP_AuthReqReply(&cy_ble_configPtr->authInfo[CY_BLE_SECURITY_CONFIGURATION_0_INDEX]);
            }  
            break;
            
        case CY_BLE_EVT_GAP_PASSKEY_ENTRY_REQUEST:
            break;

        case CY_BLE_EVT_GAP_PASSKEY_DISPLAY_REQUEST:
            break;

        case CY_BLE_EVT_GAP_AUTH_COMPLETE:

            break;
        case CY_BLE_EVT_GAP_AUTH_FAILED:

            break;

        case CY_BLE_EVT_GAP_ENCRYPT_CHANGE:
            break;
            
        case CY_BLE_EVT_GAP_KEYS_GEN_COMPLETE:
            keyInfo.SecKeyParam = (*(cy_stc_ble_gap_sec_key_param_t *)eventParam);
            Cy_BLE_GAP_SetIdAddress(&cy_ble_deviceAddress);
            break;

        /**********************************************************
        *                       GATT Events
        ***********************************************************/
        case CY_BLE_EVT_GATT_CONNECT_IND:
            break;

        case CY_BLE_EVT_GATT_DISCONNECT_IND:
            IasSetAlertLevel(NO_ALERT);
            break;

        case CY_BLE_EVT_GATTS_XCNHG_MTU_REQ:
            break;

        case CY_BLE_EVT_GATTS_READ_CHAR_VAL_ACCESS_REQ:
        {
            //call a function to update the characteristic that was read if we need to send more data to the host
            /*
            cy_stc_ble_gatts_char_val_read_req_t *readReqParam = (cy_stc_ble_gatts_char_val_read_req_t *)eventParam;

            if (readReqParam->attrHandle == CY_BLE_CUSTOM_SERVICE_CUSTOM_CHARACTERISTIC_CHAR_HANDLE)
            {
                // Prepare the characteristic value that should be sent in response to the read request
                uint8_t preparedValue = ...; // Your prepared value
                
                // Set the prepared value using Cy_BLE_GATTS_WriteAttributeValue
                cy_stc_ble_gatts_db_attr_val_info_t attrValueInfo = {
                    .handleValuePair = {.attrHandle = readReqParam->attrHandle, .value = {.val = &preparedValue, .len = sizeof(preparedValue)}},
                    .connHandle = readReqParam->connHandle,
                    .flags = CY_BLE_GATT_DB_LOCALLY_INITIATED, // Assuming this is a locally initiated operation
                    .offset = 0u
                };
                
                Cy_BLE_GATTS_WriteAttributeValue(&attrValueInfo);
                
                // Optionally, update the characteristic value for future reads
                // ...
            }
            */
            /*
            cy_stc_ble_gatts_char_val_read_req_t *readReqParam = (cy_stc_ble_gatts_char_val_read_req_t *) eventParam;
            if( readReqParam->attrHandle == CY_BLE_CUSTOM_SERVICE_CUSTOM_CHARACTERISTIC_CHAR_HANDLE ){
               
                
                Cy_BLE_GATTS_ReadAttributeValue(eventParam);
            }
            */
            
            cy_stc_ble_gatt_handle_value_pair_t handleValuePair;
            uint8_t value;
            handleValuePair.attrHandle = CY_BLE_CUSTOM_SERVICE_CUSTOM_CHARACTERISTIC_CHAR_HANDLE;
            handleValuePair.value.len = CHAR_MAX_LEN;
            handleValuePair.value.val = &value;
            
            if( CY_BLE_GATT_ERR_NONE == Cy_BLE_GATTS_ReadAttributeValue(eventParam) ){
                value++;
                
                Cy_BLE_GATTS_WriteAttributeValue(eventParam);
                
            }
            
            break;
        }

        case CY_BLE_EVT_GATTS_WRITE_CMD_REQ:
            
            
        case CY_BLE_EVT_GATTS_WRITE_REQ:  
            {
                /*
                cy_stc_ble_conn_handle_t *connHandleParam = (cy_stc_ble_conn_handle_t *)eventParam;
                // Store the connection handle in your application's context
                memcpy(&cy_ble_connHandle, connHandleParam, sizeof(cy_stc_ble_conn_handle_t));
                uint8_t readValue;
                cy_stc_ble_gatts_db_attr_val_info_t attrValueInfo;

                attrValueInfo.handleValuePair.attrHandle = CY_BLE_CUSTOM_SERVICE_CUSTOM_CHARACTERISTIC_CHAR_HANDLE;
                attrValueInfo.handleValuePair.value.len = sizeof(readValue);
                attrValueInfo.handleValuePair.value.val = &readValue;
                attrValueInfo.connHandle = *connHandleParam;
                attrValueInfo.flags = CY_BLE_GATT_DB_LOCALLY_INITIATED;
                attrValueInfo.offset = 0u;

                cy_en_ble_gatt_err_code_t writeResult = Cy_BLE_GATTS_WriteAttributeValue(&attrValueInfo);

                if (writeResult == CY_BLE_GATT_ERR_NONE) {
                    // Attribute value write successful
                } else {
                    // Attribute value write failed
                }

                */
               /* 
            cy_stc_ble_gatts_write_cmd_req_param_t *writeReqParam = (cy_stc_ble_gatts_write_cmd_req_param_t *) eventParam;
            if ( writeReqParam->handleValPair.attrHandle == CY_BLE_CUSTOM_SERVICE_CUSTOM_CHARACTERISTIC_CHAR_HANDLE ){
                newValue = writeReqParam->handleValPair.value.val[0];
                newValue++;
            }
                */
            //call a function to process the data received in the eventParam
                
            }
            break;
            
            
            
        /**********************************************************
        *                       Other Events
        ***********************************************************/
        case CY_BLE_EVT_PENDING_FLASH_WRITE:
            break;

        default:
            break;
    }
}

/*******************************************************************************
* Function Name: LowPowerImplementation()
********************************************************************************
*
* Summary:
*   Implements low power in the project.
*
* Theory:
*   The function tries to enter deep sleep as much as possible - whenever the 
*   BLE is idle and the UART transmission/reception is not happening. 
*
*******************************************************************************/
void LowPowerImplementation(void)
{
    
    
    Cy_SysPm_DeepSleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
}

/*******************************************************************************
* Function Name: HostMain
********************************************************************************
*
* Summary:
*   Main function for the project.
*
* Theory:
*   The function starts BLE.
*   This function processes all BLE events and also implements the low power 
*   functionality.
*
*******************************************************************************/
int HostMain(void)
{   
    
    /* Initialization the user interface: LEDs, SW2, etc.  */
    InitUserInterface();
    
    /* Start BLE component and register generic event handler */
    Cy_BLE_Start(AppCallBack);
    
    /* Initialize BLE Services */
    IasInit();
    
    PWM_FAN_Start();
    //PWM_VIBE_Start();
    PWM_TENS_Start();
    
    power_init();
    
    
      
    /***************************************************************************
    * Main polling loop
    ***************************************************************************/
    while(1)
    {
        DBG_PRINTF("Loops from main %d\r\n", loopcount++);
        
        
        /* Cy_BLE_ProcessEvents() allows BLE stack to process pending events */
        Cy_BLE_ProcessEvents();
        
        /* To achieve low power */
        //LowPowerImplementation();
        power_task();
        
        ui_task();        
         
        /* Update Alert Level value on the blue LED */
        switch(IasGetAlertLevel())
        {
            case NO_ALERT:
                //Alert_LED_Write(LED_OFF);
                break;

            case MILD_ALERT:
                if(mainTimer)
                {
                    /* Toggle alert LED after timeout */
                    //Alert_LED_INV();
                }
                break;

            case HIGH_ALERT:
                //Alert_LED_Write(LED_ON);
                break;
        }
        
        
        
        /* Restart 1s timer */
        if(mainTimer != 0u)
        {
            mainTimer = 0u;
            Cy_BLE_StartTimer(&timerParam);
        }
    }
}

    
/* [] END OF FILE */
