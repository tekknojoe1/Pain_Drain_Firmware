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
#include "ias.h"
#include "power.h"
#include "debug.h"
#include "temp.h"
#include "tens.h"
#include "vibe.h"
#include <stdlib.h>
#include "bitbang_spi.h"
#include "my_i2c.h"

static cy_stc_ble_timer_info_t     timerParam = { .timeout = ADV_TIMER_TIMEOUT };        
static volatile uint32_t           mainTimer  = 1u;

static int loopcount = 0;

uint8 newBatteryLevel = 0;
uint8 value;
uint8 *respondStringPtr;
uint8 data[20];
uint32_t pinReadValue;
uint8_t data2[] = {0x01, 0x02, 0x03, 0x04}; // Replace with your actual data
unsigned int MAX_LENGTH = 20;
uint8 fakeBatteryPercentage = 100;
int previousValue = -1;
DeviceStatus device_status;
int tensPhase;
int tensAmpValue;
double tensDurationValue;
int tensPeriodValue;
int tensChannel;

int getExternalValue() {
    // Simulate getting an external value (e.g., sensor reading)
    // For this example, we'll use a simple counter.
    static int fakeBatteryPercentage = 100;
    if(fakeBatteryPercentage <= 0){
        fakeBatteryPercentage = 0;   
    }
    else{
        fakeBatteryPercentage--;  
    }
    return fakeBatteryPercentage;
}
void checkForValueChange() {
    int newValue = getExternalValue();
    if (newValue != previousValue) {
        DBG_PRINTF("Value changed: %d\r\n", newValue);
        previousValue = newValue;
    }
}

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
    //DBG_PRINTF("App call back events\r\n");
    //DBG_PRINTF("e=%d\r\n", event);
    cy_en_ble_gatt_err_code_t gattErr = CY_BLE_GATT_ERR_NONE;
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
            //UpdateLedState();
            //DBG_PRINTF("Disconnected\r\n");
            IasSetAlertLevel(NO_ALERT);
            break;

        case CY_BLE_EVT_GAP_DEVICE_CONNECTED:
            /* BLE link is established */
            keyInfo.SecKeyParam.bdHandle = (*(cy_stc_ble_gap_connected_param_t *)eventParam).bdHandle;
            Cy_BLE_GAP_SetSecurityKeys(&keyInfo);
            //DBG_PRINTF("connected \r\n");
            //power_led_connected();
            DBG_PRINTF("CONNECTED\r\n");
            //device_status = CONNECTED;
            //UpdateLedState();
            break;

        case CY_BLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
            if(Cy_BLE_GetAdvertisementState() == CY_BLE_ADV_STATE_STOPPED){   
                /* Fast and slow advertising period complete, go to low power  
                 * mode (Hibernate) and wait for an external
                 * user event to wake up the device again */
                //UpdateLedState();   FIXME
                //UpdateLedState();
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
                //UpdateLedState();        //FIXME
                
                /* Indicate that timer is raised to the main loop */
                mainTimer++;
            }
            break;
            
        case CY_BLE_EVT_HARDWARE_ERROR:    /* This event indicates that some internal HW error has occurred */
            break;

        case CY_BLE_EVT_STACK_SHUTDOWN_COMPLETE:
            /* Hibernate */
            //UpdateLedState();
            Cy_SysPm_Hibernate(); // This is needed to use wakeup button
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

        case CY_BLE_EVT_GATTS_READ_CHAR_VAL_ACCESS_REQ: //Executes when a read is requested from mobile app
        {
            //call a function to update the characteristic that was read if we need to send more data to the host
            cy_stc_ble_gatts_char_val_read_req_t *readReq = (cy_stc_ble_gatts_char_val_read_req_t *)eventParam;
            cy_stc_ble_gatt_handle_value_pair_t handleValuePair;
            
            if (readReq->attrHandle == cy_ble_basConfigPtr->bass->batteryLevelHandle)
            {
                // Update your local battery level value (example: increment by 1)
                if(fakeBatteryPercentage <= 0){
                 fakeBatteryPercentage = 100;   
                }
                else{
                    fakeBatteryPercentage--;
                }
                
                DBG_PRINTF("entered read case battery service\r\n");
                // Provide the updated value as the response to the read request
                handleValuePair.value.val = &fakeBatteryPercentage;
                handleValuePair.value.len = sizeof(fakeBatteryPercentage);
                handleValuePair.attrHandle = cy_ble_basConfigPtr->bass->batteryLevelHandle;
                gattErr = Cy_BLE_GATTS_WriteAttributeValueLocal(&handleValuePair);

                if (gattErr != CY_BLE_GATT_ERR_NONE)
                {
                    // Handle error
                    DBG_PRINTF("Read error\r\n");
                }
            }
            
            // Checks to see if its requesting the custom service characteristic
            if(CY_BLE_CUSTOM_SERVICE_CUSTOM_CHARACTERISTIC_CHAR_HANDLE == readReq->attrHandle)
            {
                // Create a handle-value pair structure
                cy_stc_ble_gatt_handle_value_pair_t handleValuePair;

                // Populates the HandValuePair attributes to send over to phone app
                handleValuePair.value.val = respondStringPtr;
                handleValuePair.value.len = MAX_LENGTH;
                handleValuePair.attrHandle = CY_BLE_CUSTOM_SERVICE_CUSTOM_CHARACTERISTIC_CHAR_HANDLE;
                
                // Writes the value to central device and returns an error code if there is one
                gattErr = Cy_BLE_GATTS_WriteAttributeValueLocal(&handleValuePair);
                if (gattErr != CY_BLE_GATT_ERR_NONE)
                {
                    // Handle error
                    DBG_PRINTF("Read error\r\n");
                }
              
                
                // End of working code

                // Test to see if charger was working properly
                pinReadValue = Cy_GPIO_Read(CHG_STAT_0_PORT, CHG_STAT_0_NUM);
                //DBG_PRINTF("charge status: %d\r\n", pinReadValue);
                
                //DBG_PRINTF("entered read case custom service\r\n");
                
            }
            break;
        }

        case CY_BLE_EVT_GATTS_WRITE_CMD_REQ:
            
            
        case CY_BLE_EVT_GATTS_WRITE_REQ: // Executes when mobile app writes something to device
            {    

                cy_stc_ble_gatts_write_cmd_req_param_t *writeReq = (cy_stc_ble_gatts_write_cmd_req_param_t *)eventParam;
                int length = writeReq->handleValPair.value.len;
                cy_en_ble_api_result_t writeError;
                //DBG_PRINTF("Length: %d\r\n", length);
                char receivedCommand[length + 1];
                int i = 0;
                char *tokens[10]; // An array to store the tokens, assuming a maximum of 10 tokens
                int token_count = 0; // To keep track of the number of tokens
                char *token;
                char *delimiter = " ";
                
                
                // Checks to see if its requesting the custom service characteristic
                if(CY_BLE_CUSTOM_SERVICE_CUSTOM_CHARACTERISTIC_CHAR_HANDLE == writeReq->handleValPair.attrHandle)
                {

                    for(int i = 0; i < length; i++ )
                    {
                        // Grabs the data that the phone app sent
                        receivedCommand[i] = (char)writeReq->handleValPair.value.val[i];
                    }
                    // Add a null terminator to mark end of string
                    receivedCommand[length] = '\0';
                    //DBG_PRINTF("Received string: %s\r\n", receivedCommand);
                    
                    // This splits the received command into sections by single space
                    token = strtok(receivedCommand, delimiter); // Gets the first token
                    
                    // Stores each token or each value into an array called tokens
                    while (token != NULL) {
                        tokens[token_count] = token; // Store the token in the array
                        token_count++;
                        token = strtok(NULL, delimiter); // Get the next token
                    }
                    switch(tokens[0][0]){
                        case 't':
                        {
                            /*
                            Packet information contains
                            1: char t - Temperature
                            2: int Temperature
                            */
                            int temperatureValue = atoi(tokens[1]); // Convert the numeric part after 't'
                            DBG_PRINTF("temperature: %d\r\n", temperatureValue);
                            set_temp(temperatureValue);  
                            break;
                        }
                        case 'T':
                        {
                            /*
                            int tensPhase;
                            int tensAmpValue;
                            double tensDurationValue;
                            int tensPeriodValue;
                            int tensChannel;
                            */
                            /*
                            Packet information contains
                            1: char T - TENS
                            2: int Amplitude
                            3: double Duration
                            4: double Period
                            5: int Channel
                            */
                            if(tokens[1][0] == 'p'){
                                /*
                                Packet information contains
                                1: char T - TENS
                                2: char p - Phase
                                3: int Phase Degree
                                */
                                tensPhase = atoi(tokens[2]);
                                DBG_PRINTF("Tens phase: %d\r\n", tensPhase);
                            }
                            else{
                                tensAmpValue = atoi(tokens[1]);
                                tensDurationValue = atof(tokens[2]);
                                tensPeriodValue = atoi(tokens[3]);
                                tensChannel = atoi(tokens[4]);
                                //int phaseDegree = atoi(tokens[5]);
                                DBG_PRINTF("Tens amplitude: %d\r\n", tensAmpValue);
                                DBG_PRINTF("Tens duration: %s\r\n", tokens[2]);
                                DBG_PRINTF("Tens period: %d\r\n", tensPeriodValue);
                                DBG_PRINTF("Tens Channel: %d\r\n", tensChannel);
                            }
                            set_tens_signal(tensAmpValue, tensDurationValue, tensPeriodValue, tensChannel,  tensPhase);
                            break;
                        }
                        case 'v':
                        {
                            /*
                            Packet information contains
                            1: char v - Vibration
                            2: int Amplitude
                            3: int Frequency
                            4: int Wavefrom
                            */
                            //char *waveType = tokens[1];
                            int vibeIntensity = atoi(tokens[1]);
                            int vibeFreq = atoi(tokens[2]);
                            //int vibeWaveform = atoi(tokens[3]);
                            //DBG_PRINTF("v waveType: %s\r\n", waveType);
                            DBG_PRINTF("vibration Intensity: %d\r\n", vibeIntensity);
                            DBG_PRINTF("vibration frequency: %d\r\n", vibeFreq);
                            //DBG_PRINTF("vibration waveform: %d\r\n", vibeWaveform);
                            set_vibe(vibeIntensity, vibeFreq);
                           
                            break;
                        }
                        
                        case 'B':
                        {
                            DBG_PRINTF("HERE\r\n");
                            if(atoi(tokens[1]) == 0){
                                device_status = NOT_CHARGING;
                                DBG_PRINTF("Assign B 0\r\n");
                                //DBG_PRINTF("Assign B 1\r\n", receivedCommand);
                            }else if(atoi(tokens[1]) == 1){
                                device_status = CHARGING;
                                DBG_PRINTF("Assign B 1\r\n");
                                //DBG_PRINTF("Assign B 1\r\n", receivedCommand);
                            } else if(atoi(tokens[1]) == 2){
                                DBG_PRINTF("Assign B 2\r\n");
                                //DBG_PRINTF("Assign B 2\r\n", receivedCommand);
                                device_status = FULLY_CHARGED;
                            } else if(atoi(tokens[1]) == 3){
                                device_status = LOW_BATTERY;
                                DBG_PRINTF("Assign B 3\r\n");
                                //DBG_PRINTF("Assign B 3\r\n", receivedCommand);
                            } else if(atoi(tokens[1]) == 4){
                                device_status = MEDIUM_BATTERY;
                                DBG_PRINTF("Assign B 4\r\n");
                                //DBG_PRINTF("Assign B 3\r\n", receivedCommand);
                            } else if(atoi(tokens[1]) == 5){
                                device_status = NORMAL;
                                DBG_PRINTF("Assign B 5\r\n");
                                //DBG_PRINTF("Assign B 3\r\n", receivedCommand);
                            } else if(atoi(tokens[1]) == 6){
                                device_status = WARNING;
                                DBG_PRINTF("Assign B 6\r\n");
                                //DBG_PRINTF("Assign B 3\r\n", receivedCommand);
                            } 
                            //else if(atoi(tokens[1]) == 7){
                            //    device_status = ADVERTISING;
                            //    DBG_PRINTF("Assign B 7\r\n");
                            //    //DBG_PRINTF("Assign B 3\r\n", receivedCommand);
                            //} else if(atoi(tokens[1]) == 8){
                            //    device_status = CONNECTED;
                            //    DBG_PRINTF("Assign B 8\r\n");
                            //    //DBG_PRINTF("Assign B 3\r\n", receivedCommand);
                            //}
                        }
                        
                        default:
                        {
                            DBG_PRINTF("Error\r\n");
                            break;
                        }
                        
                        
                    }
                    
                    // Allocate memory for the string plus one extra byte for the null terminator
                    respondStringPtr = (uint8_t *)malloc((length + 1) * sizeof(uint8_t));

                    // Check if memory allocation was successful
                    if (respondStringPtr != NULL) {
                        // Copy the string from writeReq->handleValPair.value.val to respondStringPtr
                        memcpy(respondStringPtr, writeReq->handleValPair.value.val, length);
                        
                        // Null-terminate the string
                        respondStringPtr[length] = '\0';
                    } else {
                        // Memory allocation failure
                        DBG_PRINTF("Memory Allocation Failed\r\n");
                    }


                    // Sends a write with response command
                    writeError = Cy_BLE_GATTS_WriteRsp(writeReq->connHandle);
                    if( writeError != CY_BLE_SUCCESS ){
                        DBG_PRINTF("Write rsp failed\r\n");
                    }
                    bool result = send_data_to_phone(writeReq->handleValPair.value.val, writeReq->handleValPair.value.len, CY_BLE_CUSTOM_SERVICE_CUSTOM_CHARACTERISTIC_CHAR_HANDLE);
                    if(!result){
                        DBG_PRINTF("Failed to send to phone\r\n");
                    }
                }
                
                // This is the custom characteristic descriptor case. This is only called to enable notifications. It is triggured by the mobile app.
                else if(CY_BLE_CUSTOM_SERVICE_CUSTOM_CHARACTERISTIC_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE == writeReq->handleValPair.attrHandle)
                {
                    /* Read the write request parameter */
                    Cy_BLE_GATTS_WriteAttributeValuePeer(cy_ble_connHandle, &writeReq->handleValPair);
                    
                    /* Send the response to the write request received */
                    cy_en_ble_api_result_t rsp;
                    Cy_BLE_GATTS_WriteRsp(writeReq->connHandle);
                    if( rsp != CY_BLE_SUCCESS ){
                        DBG_PRINTF("Write rsp failed for notifications\r\n");
                    }
                    bool isNotificationsEnabled = Cy_BLE_GATTS_IsNotificationEnabled(cy_ble_connHandle, CY_BLE_CUSTOM_SERVICE_CUSTOM_CHARACTERISTIC_CHAR_HANDLE);
                    if ( isNotificationsEnabled){
                        DBG_PRINTF("Notifications enabled\r\n");
                    } else{
                        DBG_PRINTF("Notifications disabled\r\n");
                    }
                }
            //call a function to process the data received in the eventParam
                 break;

            }
            
            
            
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
    
    power_init();

    Cy_SysPm_SetHibernateWakeupSource(CY_SYSPM_HIBERNATE_PIN1_LOW); // This enables the wakeup button
    
    
    /* Start BLE component and register generic event handler */
    Cy_BLE_Start(AppCallBack);
    
    
    /* Initialize BLE Services */
    IasInit();
    
    //PWM_VIBE_Start();
    
    PWM_TENS_Start();
    PWM_TENS2_Start();
    PWM_VIBE_Start();
    //vibe_turn_on_motor();
    //PWM_TENS_Enable();
    //PWM_TENS2_Enable();
    
    temp_init();
    
    
    //AMP_PWM_Start();
//    int temp = 16384;
//    int set_amp = (100 * 16384) / 100;
//    AMP_PWM_SetCompare0(set_amp);
    // Enable is high for amp_enable
    //Cy_GPIO_Write(TEMP_USER_EN_PORT, TEMP_USER_EN_NUM, 1);
    
    //power_init();
    

    
    //AMP_PWM_Start();
    // Enable is high for amp_enable, I2C Testing
    //Cy_GPIO_Write(TEMP_USER_EN_PORT, TEMP_USER_EN_NUM, 1);
      
    /***************************************************************************
    * Main polling loop
    ***************************************************************************/
    while(1)
    {
        //DBG_PRINTF("Loops from main %d\r\n", loopcount++);
        /* Cy_BLE_ProcessEvents() allows BLE stack to process pending events */
        //uint32_t startTime = Cy_SysTick_GetValue();
        //DBG_PRINTF("TICK value: %d\r\n", startTime);
        Cy_BLE_ProcessEvents();
       
        /* To achieve low power */
        //LowPowerImplementation();
        //DBG_PRINTF("power task\r\n");
        power_task();
        /*
        if (device_status == NOT_CHARGING) {
            DBG_PRINTF("NOT_CHARGING\r\n");
        } else if (device_status == CHARGING) {
            DBG_PRINTF("CHARGING IN MAIN\r\n");
        } else if (device_status == FULLY_CHARGED) {
            DBG_PRINTF("FULLY_CHARGED\r\n");
        } else if (device_status == LOW_BATTERY) {
            DBG_PRINTF("LOW_BATTERY\r\n");
        } else if (device_status == MEDIUM_BATTERY) {
            DBG_PRINTF("MEDIUM_BATTERY\r\n");
        } else if (device_status == NORMAL_OPERATION) {
            DBG_PRINTF("NORMAL_OPERATION\r\n");
        } else if (device_status == WARNING) {
            DBG_PRINTF("WARNING\r\n");
        } else {
            DBG_PRINTF("UNKNOWN_STATUS\r\n");
        }
        */
        //ui_task();    
        
        // Test code for TENS
        //DBG_PRINTF("tens task\r\n");
        tens_timer();
        set_tens_task();
        
        
        /* Start the I2S interface */

        //I2S_Start();
        //vibe_init();
        //I2S_Start();
        //vibe_init();
        //DBG_PRINTF("vibe task\r\n");
        vibe_task();
        UpdateLedState();
        
        
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