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
#include "flash_storage.h"
#include "bq28Z610.h"
#include "version.h"
#include "cy_flash.h"
#include "cy_dfu.h"

/* ---- OTA dual-app slots (must match the bootloader common.h memory map) ---- */
#ifndef APP_SLOT
#define APP_SLOT            (0u)            /* App0 default; build_slot.py sets -D APP_SLOT=1 for App1 */
#endif
#define OTA_SLOT0_BASE      (0x10010000u)
#define OTA_SLOT1_BASE      (0x10088000u)
#define OTA_SLOT_SIZE       (0x78000u)      /* 480 KB */
#define OTA_INACTIVE_BASE   ((APP_SLOT == 0u) ? OTA_SLOT1_BASE : OTA_SLOT0_BASE)
#define OTA_FLASH_ROW       (CY_FLASH_SIZEOF_ROW)   /* 512 bytes */

/* Stage 3 flash-write primitive: erase+write+read-back one row of the INACTIVE
 * slot while the app and BLE are running. This validates the flash driver and
 * the slot addressing before the full OTA. Triggered by the 'F' BLE command.
 * Safe: the inactive slot's metadata stays invalid, so the bootloader still
 * boots the running app. */
static void ota_flash_self_test(void)
{
    uint32_t addr = OTA_INACTIVE_BASE;
    uint32_t pattern[OTA_FLASH_ROW / 4u];
    uint32_t i;
    cy_en_flashdrv_status_t st;
    bool ok;

    for (i = 0u; i < OTA_FLASH_ROW / 4u; i++) { pattern[i] = 0xA5A50000u | i; }

    DBG_PRINTF("OTA flash test: write inactive-slot row @ %x\r\n", addr);
    st = Cy_Flash_WriteRow(addr, pattern);   /* erases then programs the row */
    DBG_PRINTF("  WriteRow status = %x\r\n", (unsigned)st);

    const volatile uint32_t *rb = (const volatile uint32_t *)addr;
    ok = (st == CY_FLASH_DRV_SUCCESS);
    for (i = 0u; ok && (i < OTA_FLASH_ROW / 4u); i++)
    {
        if (rb[i] != pattern[i]) { ok = false; }
    }
    DBG_PRINTF("  readback: %s\r\n", ok ? "PASS" : "FAIL");
}

/* ---- DFU / OTA: Cypress cy_dfu over the BLE Bootloader service (CySmart) ---- */
static cy_stc_dfu_params_t  dfuParams;
static uint32_t             dfuState;
__attribute__((aligned(4))) static uint8_t dfuDataBuffer[CY_DFU_SIZEOF_DATA_BUFFER];
__attribute__((aligned(4))) static uint8_t dfuPacketBuffer[CY_DFU_SIZEOF_CMD_BUFFER];

static cy_stc_ble_timer_info_t     timerParam = { .timeout = ADV_TIMER_TIMEOUT };
static volatile uint32_t           mainTimer  = 1u;
static const uint32_t BUILD_NUMBER = 3;


uint8 newBatteryLevel = 0;
Preset presets[MAX_PRESETS];
uint8 value;
uint8 *respondStringPtr;
uint8 data[20];
uint32_t pinReadValue;
unsigned int MAX_LENGTH = 20;
uint8 fakeBatteryPercentage = 100;
int previousValue = -1;
DeviceStatus device_status;
int tensPhase;
int tensAmpValue;
int tensModeValue;
int tensPlayPauseValue;
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

#define TIMER_PERIOD_MSEC   10000U   /* Timer period in milliseconds */

void TimerInterruptHandler(void)
{
    /* Clear the terminal count interrupt */
    Cy_TCPWM_ClearInterrupt(Timer_HW, Timer_CNT_NUM, CY_TCPWM_INT_ON_TC);
    //DBG_PRINTF("Tens interrupt trigger\r\n");
    tens_timer();
    temp_timer();
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
            DBG_PRINTF("STACK ON\r\n");
            power_flags_update(POWER_FLAG_BLE, 1);
            
        case CY_BLE_EVT_GAP_DEVICE_DISCONNECTED:  
            /* Start BLE advertisement for 180 seconds and update link status on LEDs if not charging*/
            if(!isDeviceCharging() && current_mode == BLUETOOTH_MODE){
                Cy_BLE_GAPP_StartAdvertisement(CY_BLE_ADVERTISING_FAST, CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX);
                isAdvertisingInit = true;
            }
            //Cy_BLE_GAPP_StartAdvertisement(CY_BLE_ADVERTISING_FAST, CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX);
            //UpdateLedState();
            DBG_PRINTF("DISCONNECTED\r\n");
            IasSetAlertLevel(NO_ALERT);
            break;

        case CY_BLE_EVT_GAP_DEVICE_CONNECTED:
            /* BLE link is established */
            keyInfo.SecKeyParam.bdHandle = (*(cy_stc_ble_gap_connected_param_t *)eventParam).bdHandle;
            Cy_BLE_GAP_SetSecurityKeys(&keyInfo);
            //DBG_PRINTF("connected \r\n");
            //power_led_connected();
            DBG_PRINTF("CONNECTED\r\n");
            power_flags_update(POWER_FLAG_BLE, 1);
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
                DBG_PRINTF("ADVERTISEMENT STOPPED\r\n");
                // If not advertising and not charging then we power off the device
                //if(!isDeviceCharging() && current_mode == BLUETOOTH_MODE){
                //    power_off_device();
                //}
                power_flags_update(POWER_FLAG_BLE, 0);  //Turn off ble power flag
             
                //power_led_off();
                //Cy_BLE_Stop(); // Turns off BLE STACK 
                //Cy_SysPm_DeepSleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
                //Cy_SysPm_Hibernate(); // Put system in hibernate
            } else {
                extern cy_stc_ble_gap_bd_addr_t cy_ble_deviceAddress;
                DBG_PRINTF("ADVERTISEMENT STARTED - BLE Address: %x:%x:%x:%x:%x:%x\r\n",
                    cy_ble_deviceAddress.bdAddr[5], cy_ble_deviceAddress.bdAddr[4],
                    cy_ble_deviceAddress.bdAddr[3], cy_ble_deviceAddress.bdAddr[2],
                    cy_ble_deviceAddress.bdAddr[1], cy_ble_deviceAddress.bdAddr[0]);
                power_flags_update(POWER_FLAG_BLE, 1);
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
            /* eventParam points to the BLE hardware error code:
             *   0x01 = IPC sync lost (CM0+ controller not running)
             *   0x0C = HW init failure (regulator / ECO / BLESS)
             *   0x0D = PILO calibration failure (wrong LFCLK source) */
            DBG_PRINTF("HW ERR code=%x\r\n", (eventParam != NULL) ? *(uint8_t *)eventParam : 0xFF);
            break;

        case CY_BLE_EVT_STACK_SHUTDOWN_COMPLETE:
            /* Hibernate */
            //UpdateLedState();
            power_flags_update(POWER_FLAG_BLE, 0);  //Turn off ble power flag
            DBG_PRINTF("shutdown complete\r\n");
            power_led_off();
            Cy_SysPm_Hibernate(); // This is needed to use wakeup button
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
                //DBG_PRINTF("My response string: %s\r\n", (char *)respondStringPtr);
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
                        //writeFlashStorage[i] = writeReq->handleValPair.value.val[i];
                        //DBG_PRINTF("data: %d\r\n", writeFlashStorage[i]);
                    }
                    // Add a null terminator to mark end of string
                    receivedCommand[length] = '\0';
                   
                    DBG_PRINTF("Received string: %s\r\n", receivedCommand);
                    //writeToEeprom((uint8_t*)receivedCommand, sizeof(receivedCommand), 1);

                    // This splits the received command into sections by single space
                    token = strtok(receivedCommand, delimiter); // Gets the first token
                    
                    // Stores each token or each value into an array called tokens
                    while (token != NULL) {
                        tokens[token_count] = token; // Store the token in the array
                        token_count++;
                        token = strtok(NULL, delimiter); // Get the next token
                    }
                    bool handled = false;
                    switch(tokens[0][0]){
                        case 't':
                        {
                            /*
                            Packet information contains
                            1: char t - Temperature
                            2: int power level (-100 to 100, 0 = off)
                            */
                            int powerLevel = atoi(tokens[1]);
                            DBG_PRINTF("Temp power level from device: %d\r\n", powerLevel);
                            temp_set_power_level(powerLevel);
                            break;
                        }
                        case 'T':
                        {
                            
                           
                            /*
                            Packet information contains
                            1: char T - TENS
                            2: int Amplitude
                            3: int Mode
                            4: int Play/Pause
                            5: int Channel
                            6: int Phase
                            */
                            tensAmpValue = atoi(tokens[1]);
                            tensModeValue = atoi(tokens[2]);
                            tensPlayPauseValue = atoi(tokens[3]);
                            tensChannel = atoi(tokens[4]);
                            tensPhase = atoi(tokens[5]);
                            
                            TensSetting tensSetting = {
                                .intensity = atoi(tokens[1]),
                                .mode = atoi(tokens[2]),
                                .play = atoi(tokens[3]),
                                .channel = atoi(tokens[4]),
                                .phase = atoi(tokens[5]),
                            };
                            DBG_PRINTF("Tens Intensity: %d\r\n", tensSetting.intensity);
                            DBG_PRINTF("Tens Mode: %d\r\n", tensSetting.mode);
                            DBG_PRINTF("Tens Play Pause: %d\r\n", tensSetting.play);
                            DBG_PRINTF("Tens Channel: %d\r\n", tensSetting.channel);
                            DBG_PRINTF("Tens Phase: %d\r\n", tensSetting.phase);
                            
                            // I think there is a bug in this function. It seems to disconnect the device sometimes
                            set_tens_signal(tensAmpValue, tensModeValue, tensPlayPauseValue, tensChannel,  tensPhase);
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
                            //int vibeIntensity = atoi(tokens[1]);
                            int vibeFreq = atoi(tokens[1]);
                            //int vibeWaveform = atoi(tokens[3]);
                            //DBG_PRINTF("v waveType: %s\r\n", waveType);
                            //DBG_PRINTF("vibration Intensity: %d\r\n", vibeIntensity);
                            DBG_PRINTF("vibration frequency: %d\r\n", vibeFreq);
                            //DBG_PRINTF("vibration waveform: %d\r\n", vibeWaveform);
                            set_vibe(vibeFreq);
                           
                            break;
                        }
                        
                        case 'p':
                        {
                            
                            /*
                            Not implemented yet
                            Used to save a preset to device using flash or EEPROM
                            1: char p - preset
                            2: int - preset Number
                            3: char - Either T, t, or v for any of the stimuli
                            4-9: This could contain either 1-5 more values. We basically 
                            are just pulling the tens, temp, or vibe so based on which it is
                            there could be 1-5 parameters
                            */
                            
                                // Parse the preset number
                                int presetNumber = atoi(tokens[1]);

                                if (presetNumber < 1 || presetNumber > MAX_PRESETS) {
                                    DBG_PRINTF("Invalid preset number: %d\r\n", presetNumber);
                                    break;
                                }

                                Preset* currentPreset = &presets[presetNumber - 1]; // Get the preset to update

                                // Determine which stimulus is being updated
                                char stimulus = tokens[2][0]; // 'T', 't', or 'v'

                                // It’s a good idea to initialize your entire struct,
                                // e.g., by setting header and footer markers.
                                currentPreset->header = HEADER_MARKER;
                                currentPreset->preset_id = presetNumber;
                                currentPreset->footer = FOOTER_MARKER;

                                if (stimulus == 'T') {
                                    // Parse and fill TENS settings
                                    currentPreset->tens.intensity = (uint8_t)atoi(tokens[3]);
                                    currentPreset->tens.mode      = (uint8_t)atoi(tokens[4]);
                                    currentPreset->tens.play      = (uint8_t)atoi(tokens[5]);
                                    currentPreset->tens.channel   = (uint8_t)atoi(tokens[6]);
                                    currentPreset->tens.phase     = (uint8_t)atoi(tokens[7]);
                                    DBG_PRINTF("Updated TENS for preset %d\r\n", presetNumber);
                                } else if (stimulus == 'v') {
                                    // Parse and fill Vibration settings
                                    currentPreset->vibration.frequency = (uint8_t)atoi(tokens[3]);
                                    DBG_PRINTF("Updated Vibration for preset %d\r\n", presetNumber);
                                } else if (stimulus == 't') {
                                    // Parse and fill Temperature settings
                                    currentPreset->temperature.temp = (int16_t)atoi(tokens[3]);
                                    DBG_PRINTF("Updated Temperature for preset %d\r\n", presetNumber);
                                } else {
                                    DBG_PRINTF("Invalid stimulus type: %c\r\n", stimulus);
                                    break;
                                }
                                
                                //CyDelay(100);
                                // 🔎 **Print Current Preset Data Before Writing to EEPROM**
                                //DBG_PRINTF("Preset Data Before EEPROM Write:\r\n");
                                //DBG_PRINTF("  ID: %d\r\n", currentPreset->preset_id);
                                //DBG_PRINTF("  Header: 0x%X, Footer: 0x%X\r\n", currentPreset->header, currentPreset->footer);
                                //DBG_PRINTF("  TENS -> Intensity: %d, Mode: %d, Play: %d, Channel: %d, Phase: %d\r\n",
                                //           currentPreset->tens.intensity, 
                                //           currentPreset->tens.mode, 
                                //           currentPreset->tens.play, 
                                //           currentPreset->tens.channel, 
                                //           currentPreset->tens.phase);
                                //DBG_PRINTF("  Vibration -> Frequency: %d\r\n", currentPreset->vibration.frequency);
                                //DBG_PRINTF("  Temperature -> Temp: %d\r\n", currentPreset->temperature.temp);

                                // Now write the entire preset to EEPROM.
                                // IMPORTANT: Use sizeof(Preset) rather than sizeof(pointer)
                                
                                writeToEeprom((uint8_t*)currentPreset, sizeof(Preset), presetNumber);
                               
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
                            break;
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
                        
                        case 'V':
                        {
                            static uint8_t version_resp[4] = {0x56, FIRMWARE_VERSION_MAJOR, FIRMWARE_VERSION_MINOR, FIRMWARE_VERSION_PATCH};
                            DBG_PRINTF("Version requested: %d.%d.%d\r\n", FIRMWARE_VERSION_MAJOR, FIRMWARE_VERSION_MINOR, FIRMWARE_VERSION_PATCH);
                            writeError = Cy_BLE_GATTS_WriteRsp(writeReq->connHandle);
                            if (writeError != CY_BLE_SUCCESS) {
                                DBG_PRINTF("Write rsp failed\r\n");
                            }
                            bool result = send_data_to_phone(version_resp, sizeof(version_resp), CY_BLE_CUSTOM_SERVICE_CUSTOM_CHARACTERISTIC_CHAR_HANDLE);
                            if (!result) {
                                DBG_PRINTF("Failed to send version to phone\r\n");
                            }
                            handled = true;
                            break;
                        }

                        case 'b':
                        {
                            uint8_t soc = bq28Z610_get_soc();
                            uint16_t tte = bq28Z610_get_tte();
                            uint16_t mv = bq28Z610_get_voltage_mv();
                            static uint8_t batt_resp[7];
                            batt_resp[0] = 0x62;
                            batt_resp[1] = soc;
                            batt_resp[2] = (uint8_t)(tte & 0xFF);
                            batt_resp[3] = (uint8_t)(tte >> 8);
                            batt_resp[4] = (uint8_t)device_status;
                            batt_resp[5] = (uint8_t)(mv & 0xFF);
                            batt_resp[6] = (uint8_t)(mv >> 8);
                            DBG_PRINTF("Battery requested: SoC=%u%%, TTE=%u min, mV=%u, Status=%d\r\n", (unsigned)soc, (unsigned)tte, (unsigned)mv, (int)device_status);
                            writeError = Cy_BLE_GATTS_WriteRsp(writeReq->connHandle);
                            if (writeError != CY_BLE_SUCCESS) {
                                DBG_PRINTF("Write rsp failed\r\n");
                            }
                            bool result = send_data_to_phone(batt_resp, sizeof(batt_resp), CY_BLE_CUSTOM_SERVICE_CUSTOM_CHARACTERISTIC_CHAR_HANDLE);
                            if (!result) {
                                DBG_PRINTF("Failed to send battery to phone\r\n");
                            }
                            handled = true;
                            break;
                        }

                        case 'F':
                        {
                            /* Stage 3: OTA flash-write self-test on the inactive slot. */
                            DBG_PRINTF("OTA flash self-test requested\r\n");
                            writeError = Cy_BLE_GATTS_WriteRsp(writeReq->connHandle);
                            if (writeError != CY_BLE_SUCCESS) {
                                DBG_PRINTF("Write rsp failed\r\n");
                            }
                            ota_flash_self_test();
                            handled = true;
                            break;
                        }

                        default:
                        {
                            DBG_PRINTF("char: %c\r\n", tokens[0][0]);
                            DBG_PRINTF("Error\r\n");
                            break;
                        }
                        
                        
                    }
                    
                    if (!handled) {
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
                }
                
                // This is the custom characteristic descriptor case. This is only called to enable notifications. It is triggured by the mobile app.
                else if(CY_BLE_CUSTOM_SERVICE_CUSTOM_CHARACTERISTIC_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE == writeReq->handleValPair.attrHandle)
                {
                    /* Read the write request parameter */
                    Cy_BLE_GATTS_WriteAttributeValuePeer(cy_ble_connHandle, &writeReq->handleValPair);
                    
                    /* Send the response to the write request received */
                    cy_en_ble_api_result_t rsp;
                    rsp = Cy_BLE_GATTS_WriteRsp(writeReq->connHandle);
                    if( rsp != CY_BLE_SUCCESS ){
                        DBG_PRINTF("Write rsp failed for notifications\r\n");
                    }
                    bool isNotificationsEnabled = Cy_BLE_GATTS_IsNotificationEnabled(cy_ble_connHandle, CY_BLE_CUSTOM_SERVICE_CUSTOM_CHARACTERISTIC_CHAR_HANDLE);
                    if ( isNotificationsEnabled ){
                        DBG_PRINTF("Notifications enabled\r\n");
                    } else{
                        DBG_PRINTF("Notifications disabled\r\n");
                    }
                }
                else {
                 DBG_PRINTF("Characterisitic handle: %d\r\n", writeReq->handleValPair.attrHandle);   
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
* Function Name: void Switch_ISR(void)
********************************************************************************
*
* Summary:
*  This function is executed when GPIO interrupt is triggered.
*
* Parameters: None
*
* Return: None
*
*******************************************************************************/
void Isr_switch(void)
{
    DBG_PRINTF("ISR Charge port interrrupt\r\n");
    /* Clears the triggered pin interrupt */
	Cy_GPIO_ClearInterrupt(CHG_STAT_0_PORT, CHG_STAT_0_NUM);
	NVIC_ClearPendingIRQ(SysInt_Switch_cfg.intrSrc);
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
    DBG_PRINTF("APP BUILD %d\r\n", BUILD_NUMBER);
    DBG_PRINTF("START OF PROGRAM\r\n");
    DBG_PRINTF("Firmware version: " FIRMWARE_VERSION_STR "\r\n");
    
    /* Check the IO status. If current status is frozen, unfreeze the system. */
    if(Cy_SysPm_GetIoFreezeStatus())
    {   
        /* Unfreeze the system */
        Cy_SysPm_IoUnfreeze();
        DBG_PRINTF("IOs unfrozen after wake from Hibernate\r\n");
    }
    
    power_init();
    /* Start LPComp */
    LPComp_1_Start();

    /* Explicitly enable the internal ULP reference voltage */
    Cy_LPComp_UlpReferenceEnable(LPCOMP);

    /* Configure LPComp Channel 1 inputs:
       - Positive: CHG_STAT (GPIO)
       - Negative: Local VREF (internal ~0.4-0.8V)
    */
    Cy_LPComp_SetInputs(LPCOMP, CY_LPCOMP_CHANNEL_0, CY_LPCOMP_SW_GPIO, CY_LPCOMP_SW_LOCAL_VREF);

    /* Allow 50us settle time for ULP reference */
    Cy_SysLib_DelayUs(50u);
    
    Cy_SysPm_SetHibernateWakeupSource(CY_SYSPM_HIBERNATE_PIN1_LOW | CY_SYSPM_LPCOMP0_LOW); // This allows wakeup on button and lpcomp 1 pin
    //Cy_SysPm_SetHibernateWakeupSource(CY_SYSPM_HIBERNATE_PIN1_LOW); // This allows wakeup on button and lpcomp 1 pin
    
    /* Start BLE component and register generic event handler */
    Cy_BLE_Start(AppCallBack);

    /* Initialize the DFU (OTA) engine and start the BLE DFU transport (the
     * Bootloader service that CySmart and the mobile app talk to). cy_dfu writes
     * the received image into the INACTIVE slot; on completion we reset and the
     * bootloader boots the new slot via our own {magic,version} metadata. */
    dfuParams.timeout      = 1u;     /* ms. MUST be >0 (Cy_DFU_Continue asserts timeout!=0).
                                      * Kept at 1ms so the read is effectively a poll and the
                                      * app main loop stays responsive when no OTA is active;
                                      * DFU packets are buffered by the BTS callback between calls. */
    dfuParams.dataBuffer   = dfuDataBuffer;
    dfuParams.packetBuffer = dfuPacketBuffer;
    (void)Cy_DFU_Init(&dfuState, &dfuParams);
    Cy_DFU_TransportStart();
    DBG_PRINTF("DFU transport started\r\n");

    
    /* Initialize BLE Services */
    IasInit();
    
    //PWM_VIBE_Start();
    
    
    PWM_VIBE_Start();
    //vibe_turn_on_motor();
    //PWM_TENS_Enable();
    //PWM_TENS2_Enable();
    
    temp_init();
    bq28Z610_init();
    
    
    //AMP_PWM_Start();
//    int temp = 16384;
//    int set_amp = (100 * 16384) / 100;
//    AMP_PWM_SetCompare0(set_amp);
    // Enable is high for amp_enable
    //Cy_GPIO_Write(TEMP_USER_EN_PORT, TEMP_USER_EN_NUM, 1);
    
    //power_init();
    
    tens_init();
    
    //AMP_PWM_Start();
    // Enable is high for amp_enable, I2C Testing
    //Cy_GPIO_Write(TEMP_USER_EN_PORT, TEMP_USER_EN_NUM, 1);
    
    
    /* Initialize the interrupt vector table with the timer interrupt handler
     * address and assign priority. */
    Cy_SysInt_Init(&isrTimer_cfg, TimerInterruptHandler);
    NVIC_ClearPendingIRQ(isrTimer_cfg.intrSrc);/* Clears the interrupt */
    NVIC_EnableIRQ(isrTimer_cfg.intrSrc); /* Enable the core interrupt */
    __enable_irq(); /* Enable global interrupts. */
    
    /* Start the TCPWM component in timer/counter mode. The return value of the
     * function indicates whether the arguments are valid or not. It is not used
     * here for simplicity. */
    (void)Cy_TCPWM_Counter_Init(Timer_HW, Timer_CNT_NUM, &Timer_config);
    Cy_TCPWM_Enable_Multiple(Timer_HW, Timer_CNT_MASK); /* Enable the counter instance */
    
    /* Set the timer period in milliseconds. To count N cycles, period should be
     * set to N-1. */
    Cy_TCPWM_Counter_SetPeriod(Timer_HW, Timer_CNT_NUM, TIMER_PERIOD_MSEC - 1);
    
    /* Trigger a software reload on the counter instance. This is required when 
     * no other hardware input signal is connected to the component to act as
     * a trigger source. */
    Cy_TCPWM_TriggerReloadOrIndex(Timer_HW, Timer_CNT_MASK); 
    
    /* Interrupt for charge port */
    Cy_SysInt_Init(&SysInt_Switch_cfg, Isr_switch);
	NVIC_ClearPendingIRQ(SysInt_Switch_cfg.intrSrc);
	NVIC_EnableIRQ(SysInt_Switch_cfg.intrSrc);
    
    
    int lastValue = 0; // remove after debugging used to track last lpcomp value
    
    initEeprom();
      
    /***************************************************************************
    * Main polling loop
    ***************************************************************************/
    while(1)
    {
        Cy_BLE_ProcessEvents();

        /* Process the OTA/DFU protocol (non-blocking poll). When a transfer
         * finishes, the new image (incl. its higher-version metadata) is in the
         * inactive slot -- reset and let the bootloader select it. */
        Cy_DFU_Continue(&dfuState, &dfuParams);
        if (dfuState == CY_DFU_STATE_FINISHED)
        {
            DBG_PRINTF("DFU complete -> reset into new slot\r\n");
            Cy_SysLib_Delay(50u);
            NVIC_SystemReset();
        }
        else if (dfuState == CY_DFU_STATE_FAILED)
        {
            DBG_PRINTF("DFU failed -> restarting transport\r\n");
            (void)Cy_DFU_Init(&dfuState, &dfuParams);
            Cy_DFU_TransportReset();
        }

        int currentValue = Cy_LPComp_GetCompare(LPCOMP, CY_LPCOMP_CHANNEL_1);
        //DBG_PRINTF("lp comp changed value: %d\r\n", currentValue);
        if(lastValue != currentValue){
           DBG_PRINTF("lp comp changed value: %d\r\n", currentValue); 
           lastValue = currentValue;
        }
        power_task();
        vibe_task();
        tens_task();
        temp_task();   
        
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