/** 
 * Manages power and charging state machine

 */

#include <project.h>

#include "power.h"
#include "bq24298.h"
#include "my_i2c.h"
#include "debug.h"
#include "bq25883.h"
#include "bq28Z610.h"
#include "tens.h"
#include "temp.h"
#include "vibe.h"
#include "flash_storage.h"


#define POWER_TIMER_PERIOD_MS 10
#define POWER_DISPLAY_TIMEOUT_SEC 20
#define POWER_DISPLAY_TIMEOUT_INTERVAL (POWER_DISPLAY_TIMEOUT_SEC*1000 / POWER_TIMER_PERIOD_MS)
#define POWER_LED_SLOW_BLINK_TIMER 200
#define POWER_LED_FAST_BLINK_TIMER 100
#define POWER_LED_CONNECTED_TIMER 600

#define POWER_IDLE 0
#define POWER_DOWN 10
#define POWER_ACTIVE 2
#define MAX_LCD_PWM 191 // limit to 80 percent of 255
static int power_state = POWER_IDLE;
static uint32 power_timeout = 0;
static uint32 power_flags = 0;   //Each bit indicates an active system. If all bits are zero we power down the entire device
DeviceStatus last_device_status;
DeviceMode current_mode;
bool triggerBattery = false;
bool isConnected = false;
bool chargingValueSent = false;
bool notChargingValueSent = false;
int cycles = 0;
int power_off_cycles = 0;
bool shutdown_ready = false;  // Initialize to false
bool isAdvertisingInit = false;




void power_led_off(void) {
    Cy_GPIO_Write(LED_RED_PORT, LED_RED_NUM, 0);
    Cy_GPIO_Write(LED_GREEN_PORT, LED_GREEN_NUM, 0);
    Cy_GPIO_Write(LED_BLUE_PORT, LED_BLUE_NUM, 0);
}

void power_led_red(void) {
    Cy_GPIO_Write(LED_RED_PORT, LED_RED_NUM, 1);
    Cy_GPIO_Write(LED_GREEN_PORT, LED_GREEN_NUM, 0);
    Cy_GPIO_Write(LED_BLUE_PORT, LED_BLUE_NUM, 0);
}

void power_led_yellow(void) {
    Cy_GPIO_Write(LED_RED_PORT, LED_RED_NUM, 1);
    Cy_GPIO_Write(LED_GREEN_PORT, LED_GREEN_NUM, 1);
    Cy_GPIO_Write(LED_BLUE_PORT, LED_BLUE_NUM, 0);
}

void power_led_green(void) {
    Cy_GPIO_Write(LED_RED_PORT, LED_RED_NUM, 0);
    Cy_GPIO_Write(LED_GREEN_PORT, LED_GREEN_NUM, 1);
    Cy_GPIO_Write(LED_BLUE_PORT, LED_BLUE_NUM, 0);
}

void power_led_blue(void) {
    Cy_GPIO_Write(LED_RED_PORT, LED_RED_NUM, 0);
    Cy_GPIO_Write(LED_GREEN_PORT, LED_GREEN_NUM, 0);
    Cy_GPIO_Write(LED_BLUE_PORT, LED_BLUE_NUM, 1);
}

void power_led_advertising(void){
    if(cycles >= POWER_LED_SLOW_BLINK_TIMER){
        cycles = 0;
    }
    //DBG_PRINTF("advertising timer: %d \r\n", cycles);
    if(cycles < POWER_LED_SLOW_BLINK_TIMER / 2){
        power_led_blue();
    } else{
        power_led_off();
    }
    cycles++;
}
void power_led_connected(void){
    if(cycles > POWER_LED_CONNECTED_TIMER){
        cycles = 0;
    }
    if(!isConnected){
        if(cycles >= POWER_LED_CONNECTED_TIMER){
            power_led_off();
            isConnected = true;
        } else{
            power_led_blue();
            cycles++; 
        } 
    }
}

void power_led_charging(void){
    if(cycles >= POWER_LED_SLOW_BLINK_TIMER){
        cycles = 0;
    }

    if(cycles < POWER_LED_SLOW_BLINK_TIMER / 2){
        power_led_green(); 
    } else{
        power_led_off();
    } 
    cycles++;   
}

void preset_1_mode_led(void){
    if(cycles >= POWER_LED_SLOW_BLINK_TIMER){
        cycles = 0;
    }

    if(cycles < POWER_LED_SLOW_BLINK_TIMER / 2){
        power_led_yellow(); 
    } else{
        power_led_off();
    } 
    cycles++;   
}

void preset_2_mode_led(void) {
    // Total cycles for one slow blink (on + off)
    if (cycles >= 300) {
        cycles = 0;
    }

    // Define the threshold for fast blinks
    int fast_blink_timer = 300 / 4;

    if ((cycles % 300) < 300 / 2) {
        // Within the slow blink "on" phase
        if ((cycles % fast_blink_timer) < fast_blink_timer / 2) {
            // Fast blink "on"
            power_led_yellow();
        } else {
            // Fast blink "off"
            power_led_off();
        }
    } else {
        // Slow blink "off" phase
        power_led_off();
    }

    cycles++;
}

void preset_3_mode_led(void) {
    // Total cycles for one slow blink (on + off)
    if (cycles >= 402) {
        cycles = 0;
    }

    // Define the threshold for fast blinks
    int fast_blink_timer = 402 / 6; // Divide slow "on" phase into 3 parts

    if ((cycles % 402) < 402 / 2) {
        // Within the slow blink "on" phase
        if ((cycles % fast_blink_timer) < fast_blink_timer / 2) {
            // Fast blink "on"
            power_led_yellow();
        } else {
            // Fast blink "off"
            power_led_off();
        }
    } else {
        // Slow blink "off" phase
        power_led_off();
    }

    cycles++;
}



/*
This is used to do a slow led blink.
*/
void power_led_slow_blink(DeviceStatus status){
    if(cycles >= POWER_LED_SLOW_BLINK_TIMER){
        cycles = 0;
    }
    if(cycles < POWER_LED_SLOW_BLINK_TIMER / 2){
        
        switch (status){
            case MEDIUM_BATTERY:
                power_led_yellow();
                break;
            case LOW_BATTERY:
                power_led_red();
                break;
            case NORMAL:
                power_led_green();
                break;
            default:
                break;
        } 
    } else{
        power_led_off();
    } 
    cycles++;   
}

void reset_timer_cycles(){
    cycles = 0;
}

void power_timer( void ) {    //Called every 100ms when the WDT is enabled
    
    if (power_timeout > 0) {
        power_timeout--;
    }
}

void power_flags_update(int flag, int state) {
    
    if (state == 0) {
        power_flags &= ~(1<<flag);
    } else {
        power_flags |= (1<<flag); 
    }
}

void gpio_interrupt_handler ( void ) {
    
    //Cy_GPIO_ClearInterrupt(PWR_BTN_PORT, PWR_BTN_NUM );
    //Cy_GPIO_ClearInterrupt(BTN0_PORT, BTN0_NUM );
    NVIC_ClearPendingIRQ( gpio_irq_cfg.intrSrc );
    
}

bool isDeviceCharging(){
    //DBG_PRINTF("Charge: %d\r\n", Cy_GPIO_Read(CHG_STAT_0_PORT, CHG_STAT_0_NUM));
    if(Cy_GPIO_Read(CHG_STAT_0_PORT, CHG_STAT_0_NUM) == 0){
        return true;
    } else{
        return false;  
    }
}


void power_init( void ) {
    
    
    //myI2C_Start();
    
    //Cy_SysInt_Init(&gpio_irq_cfg, gpio_interrupt_handler);
	//NVIC_EnableIRQ(gpio_irq_cfg.intrSrc);
	//Unmask interrupt?
    
    power_timeout = POWER_DISPLAY_TIMEOUT_INTERVAL;
    
    //LCD_PWM_Start();
    //LCD_PWM_SetCompare0(MAX_LCD_PWM);  //Set to full brightness
    DBG_PRINTF("Backlight on\r\n");

    
    //bq28Z610_init();
    //bq25883_init();
    
    
    power_flags_update(POWER_FLAG_BLE, 1);
}

void power_5v_on( void ) {
    uint8 d;
    
    power_i2c_read_reg(BQ25883_I2C_ADDR, CHG_PWR_ON_CFG_REG, &d, 1);
    d |= CHG_PWR_ON_CFG_OTGCONFIG_MASK;
    power_i2c_write_reg(BQ25883_I2C_ADDR, CHG_PWR_ON_CFG_REG, d);    
    
}

void power_5v_off( void ) {
    uint8 d;
    
    power_i2c_read_reg(BQ25883_I2C_ADDR, CHG_PWR_ON_CFG_REG, &d, 1);
    d &= ~CHG_PWR_ON_CFG_OTGCONFIG_MASK;
    power_i2c_write_reg(BQ25883_I2C_ADDR, CHG_PWR_ON_CFG_REG, d);  
    
}

void power_wakeup( void ) {
    
    power_timeout = POWER_DISPLAY_TIMEOUT_INTERVAL;
    //LCD_PWM_SetCompare0(MAX_LCD_PWM);  //Set to full brightness 
    
}
void power_off_device(){
    DBG_PRINTF("POWERING OFF DEVICE\r\n");
    
    //Turn off all stimulus
    shut_off_all_stimuli();
    
    // Turns off Leds
    power_led_off();
    Cy_BLE_Stop(); // Turns off BLE STACK 
    Cy_SysPm_DeepSleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
    Cy_SysPm_Hibernate(); // Put system in hibernate
}



void check_power_button_press(){
    
    // Check if the power button is pressed
    if (Cy_GPIO_Read(PWR_PORT, PWR_NUM) == 0) {  // Button is pressed
        power_off_cycles++;  // Increment the cycle count while button is pressed

        //DBG_PRINTF("Power cycles: %d\r\n", power_off_cycles);

        // If the button is held for at least 600 cycles (3 seconds) and device is not charging, set flag to shut down
        if (power_off_cycles >= 600 && !isDeviceCharging()) {
            shutdown_ready = true;  // Mark the system as ready for shutdown
            //DBG_PRINTF("Button ready for shutdown.\r\n");
            power_led_blue();
        }
    } else {  // Button is released
        // If the button was held for 600 cycles and shutdown is ready, enter hibernate
        if (shutdown_ready) {
            DBG_PRINTF("Button released after 600 cycles, powering down.\r\n");
            CyDelayUs(100); // 100ms delay to help combat button "bounce"
            // Switch power state to power down
            power_state = POWER_DOWN;
            
        // Device will stay in hibernate until a wake-up event occurs
        } else if (power_off_cycles > 0 && power_off_cycles < 600) {
            DBG_PRINTF("power state: %d\r\n", power_state);
            if(power_state == POWER_ACTIVE){
                // Increment the mode
                current_mode = (current_mode + 1) % (PRESET_3_MODE + 1); // Cycle through modes

                // Handle actions for the new mode (optional)
                switch (current_mode) {
                    case BLUETOOTH_MODE:
                        shut_off_all_stimuli();
                        DBG_PRINTF("Switched to Bluetooth Mode.\r\n");
                        // Perform actions for Bluetooth Mode
                        break;
                    case PRESET_1_MODE:
                        DBG_PRINTF("Switched to Preset 1 Mode.\r\n");
                        loadAndApplyPreset(1);
                      
                        // Perform actions for Preset 1
                        break;
                    case PRESET_2_MODE:
                        DBG_PRINTF("Switched to Preset 2 Mode.\r\n");
                        loadAndApplyPreset(2);
                        break;
                    case PRESET_3_MODE:
                        DBG_PRINTF("Switched to Preset 3 Mode.\r\n");
                        loadAndApplyPreset(3);
                        break;
                    default:
                        DBG_PRINTF("Unknown Mode.\r\n");
                        break;
                }
            } else{
                // Short press detected, wake up the device
                DBG_PRINTF("Button released before 600 cycles, waking up.\r\n");
                power_state = POWER_ACTIVE;
            }
            

      
        }

        // Reset the power off cycle count and shutdown flag when the button is released
        power_off_cycles = 0;
        shutdown_ready = false;  // Reset shutdown flag
    }
}

void power_task( void ) {
    //bq25883_read_all_reg();
    /*
    uint8_t lsb;
    uint8_t msb;

    power_i2c_read_reg(BQ28Z610_I2C_ADDR, 0x0A, &lsb, 2);
    power_i2c_read_reg(BQ28Z610_I2C_ADDR, 0x0B, &msb, 2);
    uint16_t value = (msb << 8) | lsb;
    DBG_PRINTF("Combined Value: %d\r\n", value);
    */ 
    /*
    uint8_t arr[2];
    int status;
    power_i2c_read_reg(BQ28Z610_I2C_ADDR, 0x0A, arr, 2);  //Start at address 0 and read both 0 and 1
    int16_t value = (arr[0] << 8) | arr[1];
    if(reg_array[0x0A] != value){
        DBG_PRINTF("Different reg 0x%x: old: 0x%x new: 0x%x\r\n", 0x0A, reg_array[0x0A], value); 
        reg_array[0x0A] = value;
    }
    */
    //DBG_PRINTF("Combined Value: %d\r\n", value);
    check_charger();
    check_power_button_press();
   
    cy_en_gpio_status_t initStatus;
    
    //initStatus = Cy_GPIO_Pin_Init(PWR_BTN_PORT, PWR_BTN_NUM, &PWR_BTN_);
    //DBG_PRINTF("Power flag %d\r\n", power_flags);
    power_flags_update(POWER_FLAG_CHG, !Cy_GPIO_Read(CHG_PG_PORT, CHG_PG_NUM) );  
    //DBG_PRINTF("Power state: %d\r\n", power_state);

    switch (power_state) {
        
     
        default: //POWER_IDLE
            
            //We just powered up from dead batteries or we reset
            //WDT should be enabled
            
            //WDT timer runs every 100 ms
            Cy_WDT_Enable();
        
            if (power_timeout == 0) {
                //We timed out so we should power down the display or ourselves
                
                if (Cy_GPIO_Read(CHG_PG_PORT, CHG_PG_NUM) != 0) {  //Don't turn off the LCD screen if we are charging
                
                    //Lets just turn off the display
                    //LCD_PWM_SetCompare0(0); 
                    
                    if (power_flags == 0 && device_status != CHARGING) {
                        //We don't have any active sub devices running so lets power all the way off.
                        
                        power_state = POWER_DOWN;
                        
                    } 
                } else {
                    power_wakeup(); 
                }   
            }
            
            //Read the gas gauge chip and post the result
            
            
            
            
            
            Cy_SysPm_Sleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
        
        break;
        
        case POWER_DOWN:
            
            //FIXME: Disable WDT
            //FIXME: Configure power button as wakeup source
            //DBG_PRINTF("Powering down\r\n");
            power_off_device();
            
            //We wake up here
            //power_wakeup();
            DBG_PRINTF("Powering down\r\n");
            
            power_state = POWER_IDLE;            
        break;
    }  
}

/*
check_charger() - Reads the charging pin and detects if device is charging, fully charged, normal operation,
low battery, warning, etc. Updates LEDs and sends notification to phone when charging status is changed.
*/
void check_charger() {
    // Read charging status
    uint8_t charge_status_register[1];
    uint8_t lower_bits;
    bool isCharging = isDeviceCharging();
    power_i2c_read_reg(BQ25883_I2C_ADDR, 0xB, charge_status_register, 1);
    lower_bits = charge_status_register[0] & 0x07; // This grabs the 3 lower bits to determine charge status
    //DBG_PRINTF("Charge reg: %d\r\n", lower_bits);
    // Determine the current device status
    if (lower_bits == 0x06) {
        device_status = FULLY_CHARGED;
    } else if (isCharging) {
        device_status = CHARGING;
    } 
    /*
    These 3 if statements below need to be removed when we get battery guage working
    Right now its only triggered by app for testing purposes
    */
    else if (device_status == MEDIUM_BATTERY) {
        return;
    } 
    else if (device_status == LOW_BATTERY) {
        return;
    } 
    else if (device_status == FULLY_CHARGED) {
        return;
    } 
    // End of test code
    else if (Cy_GPIO_Read(CHG_STAT_0_PORT, CHG_STAT_0_NUM) == 1 && isConnected) {
        device_status = NORMAL;
    } else if (!isCharging) {
        device_status = NOT_CHARGING;
    } else {
        device_status = WARNING;
    }
    

    // Check if the charging status has changed
    if (last_device_status != device_status) {
        // Update the global charging status variable
        last_device_status = device_status;
        DBG_PRINTF("charging_status: %d \r\n", device_status);

        // Update the battery status based on the new charging status
        uint8_t* data = NULL;
        switch (device_status) {
            case CHARGING:
                shut_off_all_stimuli();
                // Set this to bluetooth mode since when we unplug we'll start from there and not in a preset
                //current_mode = BLUETOOTH_MODE;
                //power_off_device();
                cy_stc_ble_gap_disconnect_info_t disconnectInfo;
                disconnectInfo.bdHandle = cy_ble_connHandle[0].bdHandle;
                disconnectInfo.reason = CY_BLE_HCI_ERROR_OTHER_END_TERMINATED_USER;

                Cy_BLE_GAP_Disconnect(&disconnectInfo); 
                power_led_off();
                power_led_green();
                Cy_BLE_GAPP_StopAdvertisement();
                current_mode = BLUETOOTH_MODE;
                //Cy_BLE_Stop(); // Turns off BLE STACK 
                //Cy_SysPm_DeepSleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
                //Cy_SysPm_Hibernate(); // Put system in hibernate
                //power_led_green();
                data = (uint8_t*)"charge";
                break;
            case NOT_CHARGING:
        
                if(Cy_BLE_GetAdvertisementState() == CY_BLE_ADV_STATE_STOPPED){
                   Cy_BLE_GAPP_StartAdvertisement(CY_BLE_ADVERTISING_FAST, CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX); 
                }
                //power_led_off();
                data = (uint8_t*)"no charge";
                break;
            case FULLY_CHARGED:
                data = (uint8_t*)"full batt";
                break;
            case LOW_BATTERY:
                data = (uint8_t*)"low batt";
                break;
            case NORMAL:
                data = (uint8_t*)"normal";
                // Handle normal operation status if needed
                break;
            default:
                DBG_PRINTF("Charge Status is not 1 or 0\r\n");
                return;
        }

        // Send data to phone if applicable
        if (data != NULL) {
            uint8_t length = strlen((char*)data);
            bool result = send_data_to_phone(data, length, CY_BLE_CUSTOM_SERVICE_CUSTOM_CHARACTERISTIC_CHAR_HANDLE);
            if (result) {
                DBG_PRINTF("Status sent: %s\r\n", data);
            }
        }
    }
}



/*
send_data_to_phone() - Takes the data to be sent to phone, the length in bytes of the data, and the characteristic handle 
of which ble characterstic you want to send to. Some characteristics can be found in the BLE_config.c file.
This sends a notification to the phone so that the phone can execute functions when a new value is recieved from device
*/
bool send_data_to_phone(uint8_t* data, uint16_t length, uint8_t characteristic){
    cy_en_ble_api_result_t gattErr;
    cy_stc_ble_gatt_handle_value_pair_t handleValuePair;
    handleValuePair.value.val = data;
    handleValuePair.value.len = length;
    handleValuePair.attrHandle = characteristic;

    // Sends notification when device is connected
    if(Cy_BLE_GetConnectionState(cy_ble_connHandle[0]) == CY_BLE_CONN_STATE_CONNECTED){
        gattErr = Cy_BLE_GATTS_SendNotification(cy_ble_connHandle, &handleValuePair);
        if(gattErr == CY_BLE_SUCCESS){ 
            return true;
        } else if(gattErr == CY_BLE_ERROR_NO_DEVICE_ENTITY){
            DBG_PRINTF("There is no connection for the corresponding bdHandle\r\n");      
        } else if(gattErr == CY_BLE_ERROR_INVALID_PARAMETER){
            DBG_PRINTF("Validation of the input parameter failed\r\n");      
        } else if(gattErr == CY_BLE_ERROR_INVALID_OPERATION){
            DBG_PRINTF("This operation is not permitted. Or an error was returned during the write attribute value in the GATT database\r\n");      
        } else if(gattErr == CY_BLE_ERROR_NTF_DISABLED){
            DBG_PRINTF("Characteristic notifications disabled\r\n");      
        } else{
            DBG_PRINTF("Unknown Error\r\n");       
        }
    }
    return false;
    
}

void shut_off_all_stimuli(){
    set_vibe(0); // Turns off vibe motor
    set_tens_signal(0, 0.0, 0, 0, 0); // Turns off tens
    set_temp(0); // Turns off peltier
    
    DBG_PRINTF("Shut off all stimuli\r\n");
    
}

void power_get_diag_data(uint8 d[]) {
    
    memset(d, 0, CHG_VNDR_PART_REV_REG+1);
    
    power_i2c_read_reg(BQ25883_I2C_ADDR, 0, d, CHG_VNDR_PART_REV_REG+1);  
      
        
}

void power_i2c_read_reg(uint8_t deviceAddr, uint8_t reg, uint8_t* d, int num_regs) {
    int status;
    int i;
    
    myI2C_I2CMasterClearStatus();
    
    status = myI2C_I2CMasterSendStart(deviceAddr, 0);
    status = myI2C_I2CMasterWriteByte(reg);
    status = myI2C_I2CMasterSendRestart(deviceAddr, 1);
    
    for (i=0;i<num_regs-1;i++) {
        CyDelay(10);
        d[i] = myI2C_I2CMasterReadByte(1);
        CyDelay(10);
        //DBG_PRINTF("reg 0x%x: %d\r\n", reg, d[i]);
        /*
        if(reg_array[reg] != d[i]){
            DBG_PRINTF("Different reg 0x%x: old: 0x%x new: 0x%x\r\n", reg, reg_array[reg], d[i]); 
            reg_array[reg] = d[i];
        }
        */
    }
    d[num_regs-1] = myI2C_I2CMasterReadByte(0);
    
    myI2C_I2CMasterSendStop();
}

void power_i2c_write_reg(uint8_t deviceAddr, uint8_t reg, uint8_t d) {
    int status;
    
    myI2C_I2CMasterClearStatus();
    status = myI2C_I2CMasterSendStart(deviceAddr, 0);
	status = myI2C_I2CMasterWriteByte(reg); 
	status = myI2C_I2CMasterWriteByte(d); 
    myI2C_I2CMasterSendStop();
}


int I2C_SCL_Read( void ) {
    return Cy_GPIO_Read(I2C_SCL_PORT, I2C_SCL_NUM);
}    
    
int I2C_SDA_Read( void ) {
   return Cy_GPIO_Read(I2C_SDA_PORT, I2C_SDA_NUM); 
}

void I2C_SCL_Write(int state) {
    Cy_GPIO_Write(I2C_SCL_PORT, I2C_SCL_NUM, state);
}

void I2C_SDA_Write(int state) {
    Cy_GPIO_Write(I2C_SDA_PORT, I2C_SDA_NUM, state);    
}


/*******************************************************************************
* Function Name: UpdateLedState
********************************************************************************
*
* Summary:
*  This function updates LED status based on current BLE state.
*
*******************************************************************************/
void UpdateLedState(void)
{
#if(SYS_VOLTAGE >= RGB_LED_MIN_VOLTAGE_MV) 

    if(!isDeviceCharging()) {
        
        //power_led_off();
        if(!notChargingValueSent){
            uint8_t* data;
            data = (uint8_t*)"charging 1";
            bool result = send_data_to_phone(data, 10, CY_BLE_CUSTOM_SERVICE_CUSTOM_CHARACTERISTIC_CHAR_HANDLE);
            // was successful
            if(result){
                DBG_PRINTF("Not Charging Status sent\r\n");
                notChargingValueSent = true;
                chargingValueSent = false;
            }
        }
        // Checks to see what mode we are in
        if(current_mode == BLUETOOTH_MODE)
        {
            // Handles connected LED and executes once per connection
            if(Cy_BLE_GetNumOfActiveConn() > 0u && !isConnected)
            {
                if(isConnected){
                    reset_timer_cycles();   
                }
                //DBG_PRINTF("Connected\r\n");
                //isConnected = true;
                // Connected indication LED
                power_led_connected();
                
            }
            // Handles Disconnected LED and executes once per Disconnection
            else if(Cy_BLE_GetNumOfActiveConn() == 0u && isConnected)
            {
                /* If in disconnected state, turn on disconnect indication LED and turn
                * off Advertising LED.
                */
                //Disconnect_LED_Write(LED_ON);
                //Advertising_LED_Write(LED_OFF);
                
                /* Turn off Alert LED */
                //Alert_LED_Write(LED_OFF);
                //DBG_PRINTF("Disconnected\r\n");
                reset_timer_cycles();
                isConnected = false;
            }
            // If we are in ble mode then check if we are advertising if not and the device isn't charging then we will start advertising
            else if(Cy_BLE_GetAdvertisementState() == CY_BLE_ADV_STATE_ADVERTISING) {
                //Blink advertising indication LED
                //DBG_PRINTF("Already advertising\r\n");
                power_led_advertising();
            } 
            
            else if(Cy_BLE_GetAdvertisementState() == CY_BLE_ADV_STATE_STOPPED && isAdvertisingInit){
                Cy_BLE_GAPP_StartAdvertisement(CY_BLE_ADVERTISING_FAST, CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX);
            }  
            
        } else if(current_mode == PRESET_1_MODE){
            //Cy_BLE_GAPP_StopAdvertisement();
            preset_1_mode_led();
        } else if(current_mode == PRESET_2_MODE){
            preset_2_mode_led();
        } else if(current_mode == PRESET_3_MODE){
            preset_3_mode_led();
        }
        
        if(current_mode != BLUETOOTH_MODE){
            //DBG_PRINTF("else block\r\n");
            Cy_BLE_GAPP_StopAdvertisement();
            if(isConnected){
                cy_stc_ble_gap_disconnect_info_t disconnectInfo;
                disconnectInfo.bdHandle = cy_ble_connHandle[0].bdHandle;
                disconnectInfo.reason = CY_BLE_HCI_ERROR_OTHER_END_TERMINATED_USER;

                Cy_BLE_GAP_Disconnect(&disconnectInfo);
            }
        }
    } 
    
    else if(isDeviceCharging())
    {
        power_led_charging();
        if(!chargingValueSent){
            uint8_t* data;
            data = (uint8_t*)"charging 0";
            bool result = send_data_to_phone(data, 10, CY_BLE_CUSTOM_SERVICE_CUSTOM_CHARACTERISTIC_CHAR_HANDLE);
            // was successful
            if(result){
                DBG_PRINTF("Charging Status sent\r\n");
                notChargingValueSent = false;
                chargingValueSent = true;
            }
        }
    }
    
    else 
    {
        /* In connected state, turn off disconnect indication and advertising 
        * indication LEDs. 
        */
        //Disconnect_LED_Write(LED_OFF);
        //Advertising_LED_Write(LED_OFF);
    }
    
    //// Checks to see what mode we are in
    //if(current_mode == BLUETOOTH_MODE)
    //{
    //    //DBG_PRINTF("TESTING\r\n");
    //    // If we are in ble mode then check if we are advertising if not and the device isn't charging then we will start advertising
    //    if(Cy_BLE_GetAdvertisementState() == CY_BLE_ADV_STATE_ADVERTISING) {
    //        //Blink advertising indication LED
    //        //DBG_PRINTF("Already advertising\r\n");
    //        power_led_advertising();
    //    } else if(Cy_BLE_GetAdvertisementState() == CY_BLE_ADV_STATE_STOPPED && isAdvertisingInit){
    //        if(!isDeviceCharging()){
    //            
    //            DBG_PRINTF("Advertising again\r\n");
    //            Cy_BLE_GAPP_StartAdvertisement(CY_BLE_ADVERTISING_FAST, CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX);
    //        }
    //    } else {
    //        //DBG_PRINTF("state %d \r\n", Cy_BLE_GetAdvertisementState());
    //    }
    //    
    //}
    //else if(current_mode == PRESET_1_MODE){
    //    Cy_BLE_GAPP_StopAdvertisement();
    //    DBG_PRINTF("Looping preset 1\r\n");
    //}
    //// Handles connected LED and executes once per connection
    //else if(Cy_BLE_GetNumOfActiveConn() > 0u && !isConnected)
    //{
    //    if(isConnected){
    //        reset_timer_cycles();   
    //    }
    //    //DBG_PRINTF("Connected\r\n");
    //    //isConnected = true;
    //    // Connected indication LED
    //    power_led_connected();
    //    
    //}
    //// Handles Disconnected LED and executes once per Disconnection
    //else if(Cy_BLE_GetNumOfActiveConn() == 0u && isConnected)
    //{
    //    /* If in disconnected state, turn on disconnect indication LED and turn
    //    * off Advertising LED.
    //    */
    //    //Disconnect_LED_Write(LED_ON);
    //    //Advertising_LED_Write(LED_OFF);
    //    
    //    /* Turn off Alert LED */
    //    //Alert_LED_Write(LED_OFF);
    //    //DBG_PRINTF("Disconnected\r\n");
    //    reset_timer_cycles();
    //    isConnected = false;
    //}
    //// Handles Charging LED
    //else if(device_status == CHARGING)
    //{
    //    DBG_PRINTF("Charging\r\n");
    //    //power_led_off();
    //    power_led_charging();
    //    //power_led_green();
    //    //DBG_PRINTF("CHARGING\r\n");
    //    if(!chargingValueSent){
    //        uint8_t* data;
    //        data = (uint8_t*)"charging 0";
    //        bool result = send_data_to_phone(data, 10, CY_BLE_CUSTOM_SERVICE_CUSTOM_CHARACTERISTIC_CHAR_HANDLE);
    //        // was successful
    //        if(result){
    //            DBG_PRINTF("Charging Status sent\r\n");
    //            notChargingValueSent = false;
    //            chargingValueSent = true;
    //        }
    //    }
    //}
    //else if(device_status == FULLY_CHARGED)
    //{
    //    power_led_green();  
    //}
    //
    //else if(device_status == NOT_CHARGING)
    //{
    //    power_led_off();
    //    if(!notChargingValueSent){
    //        uint8_t* data;
    //        data = (uint8_t*)"charging 1";
    //        bool result = send_data_to_phone(data, 10, CY_BLE_CUSTOM_SERVICE_CUSTOM_CHARACTERISTIC_CHAR_HANDLE);
    //        // was successful
    //        if(result){
    //            DBG_PRINTF("Not Charging Status sent\r\n");
    //            notChargingValueSent = true;
    //            chargingValueSent = false;
    //        }
    //    }
    //    
    //}
    //else if(device_status == NORMAL)
    //{
    //    //power_led_slow_blink(NORMAL);
    //}
    //else if(device_status == MEDIUM_BATTERY)
    //{
    //    power_led_slow_blink(MEDIUM_BATTERY);
    //}
    //    else if(device_status == LOW_BATTERY)
    //{
    //    power_led_slow_blink(LOW_BATTERY);
    //}
    //else 
    //{
    //    /* In connected state, turn off disconnect indication and advertising 
    //    * indication LEDs. 
    //    */
    //    //Disconnect_LED_Write(LED_OFF);
    //    //Advertising_LED_Write(LED_OFF);
    //}
#else
    /* 
     *   If VDDD < 2.7 volts (DWR->System), only the red LED will be used:
     *     BLINK - Alert Level: Mild Alert
     *     OFF   - Alert Level: No Alert
     *     ON    - Alert Level: High Alert    
     *
     *     Updating Alert_LED executed in the Main polling loop
     */        
#endif /* #if(CYDEV_VDDD_MV >= RGB_LED_MIN_VOLTAGE_MV) */  
}
/* [] END OF FILE */


