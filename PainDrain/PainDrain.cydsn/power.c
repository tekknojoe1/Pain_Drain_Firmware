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

#define POWER_TIMER_PERIOD_MS 1
#define POWER_DISPLAY_TIMEOUT_SEC 20
#define POWER_DISPLAY_TIMEOUT_INTERVAL (POWER_DISPLAY_TIMEOUT_SEC*1000 / POWER_TIMER_PERIOD_MS)
#define POWER_LED_SLOW_BLINK_TIMER 100
#define POWER_LED_FAST_BLINK_TIMER 50
#define POWER_LED_CONNECTED_TIMER 600

#define POWER_ADV 0    //No stimulus, advertising, we can go to sleep from this mode
#define POWER_CONNECTED 1
#define POWER_MODE_1 2
#define POWER_MODE_2 3
#define POWER_MODE_3 4
#define START_CHARGER 8
#define POWER_CHARGING 9
#define POWER_CHARGED 10
#define STOP_CHARGER 11
#define POWER_DOWN 12   //


static DeviceStatus last_charger_status = NOT_CHARGING;


#define MAX_LCD_PWM 191 // limit to 80 percent of 255
static int power_state = POWER_ADV;
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
bool just_woke_from_hibernate = true;
bool isAdvertisingInit = false;
static int button_state = 1;

bool power_stimulus_active(void) {
    return (power_flags & ((1<<POWER_FLAG_TENS) | 
                           (1<<POWER_FLAG_TEMP) | 
                           (1<<POWER_FLAG_VIBE))) != 0;
}

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
    if(cycles >= 120) cycles = 0;

    // 1 blink: ON 0-5, OFF 6-11, PAUSE 12-119 (1 second pause)
    // Same pattern as Preset 1 but blue instead of yellow
    if(cycles < 6){
        power_led_blue();
    } else {
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
    if(cycles >= 120) cycles = 0;

    // 1 blink: ON 0-5, OFF 6-11, PAUSE 12-119 (1 second pause)
    // Same pattern as Preset 1 but blue instead of yellow
    if(cycles < 6){
        power_led_green();
    } else {
        power_led_off();
    }
    cycles++;
    
}

void power_led_charged(void){
    power_led_green();   //solid green
}

void preset_1_mode_led(void){
    if(cycles >= 120) cycles = 0;

    // 1 blink: ON 0-5, OFF 6-11, PAUSE 12-119 (1 second pause)
    if(cycles < 6){
        power_led_yellow();
    } else {
        power_led_off();
    }
    cycles++;
}

void preset_2_mode_led(void){
    if(cycles >= 120) cycles = 0;

    // 2 blinks: ON 0-5, OFF 6-11, ON 12-17, OFF 18-23, PAUSE 24-119 (1 second pause)
    if(cycles < 6 || (cycles >= 12 && cycles < 18)){
        power_led_yellow();
    } else {
        power_led_off();
    }
    cycles++;
}

void preset_3_mode_led(void){
    if(cycles >= 120) cycles = 0;

    // 3 blinks: ON 0-5, OFF 6-11, ON 12-17, OFF 18-23, ON 24-29, PAUSE 30-119 (1 second pause)
    if(cycles < 6 || (cycles >= 12 && cycles < 18) || (cycles >= 24 && cycles < 30)){
        power_led_yellow();
    } else {
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

    NVIC_ClearPendingIRQ( gpio_irq_cfg.intrSrc );

}

bool isDeviceCharging(){
    if(Cy_GPIO_Read(CHG_STAT_0_PORT, CHG_STAT_0_NUM) == 0){
        return true;
    } else{
        return false;
    }
}

bool isDeviceCharged(){
    if(device_status == FULLY_CHARGED ){
        return true;
    } else{
        return false;
    }
}


void power_init( void ) {

    //Undo TENS mid-voltage
	Cy_GPIO_Write(TENS_USER_EN1_PORT, TENS_USER_EN1_NUM, 0);
    Cy_GPIO_Write(TENS_POS1_PORT, TENS_POS1_NUM, 0);
    Cy_GPIO_Write(TENS_NEG1_PORT, TENS_NEG1_NUM, 0);


	/* Reset button state on startup to prevent wake-up button press
       from being interpreted as a mode change button press */
    button_state = 1;
    power_off_cycles = 0;
    shutdown_ready = false;
    just_woke_from_hibernate = true;  // Ignore first button event

    power_timeout = POWER_DISPLAY_TIMEOUT_INTERVAL;


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

}
void power_off_device(){
    DBG_PRINTF("POWERING OFF DEVICE\r\n");

    //Turn off all stimulus
    shut_off_all_stimuli();

    //Prepare mid-voltage on TENS_SENSE
    //Cy_GPIO_Write(TENS_USER_EN1_PORT, TENS_USER_EN1_NUM, 1);
    //Cy_GPIO_Write(TENS_POS1_PORT, TENS_POS1_NUM, 1);
    //Cy_GPIO_Write(TENS_NEG1_PORT, TENS_NEG1_NUM, 1);

    // Visual indication before hibernate (like the example does)
    //power_led_blue();  // or power_led_red(), your choice
    //CyDelay(2000);     // 2 second indication
    power_led_off();
    
    // Turns off BLE STACK
    Cy_BLE_Stop();

    CyDelay(1000);    //Make sure the LPComp has settled.

    /* Set the wake-up source (this should already be set in HostMain, but good to ensure) */
    Cy_SysPm_SetHibernateWakeupSource(CY_SYSPM_HIBERNATE_PIN1_LOW | CY_SYSPM_LPCOMP1_HIGH);
    
    /* Enter Hibernate */
    Cy_SysPm_Hibernate();
    
    /* Code never reaches here until after wake-up and reset */
}


#define BUTTON_HOLD_TIME 100
int check_power_button_press(){
    int ret = 0;
    
    /* Ignore button events immediately after wake-up */
    if(just_woke_from_hibernate)
    {
        /* Only clear the flag once button is fully released */
        if(Cy_GPIO_Read(PWR_PORT, PWR_NUM) == 1)
        {
            just_woke_from_hibernate = false;
            DBG_PRINTF("Wake-up button release detected, resuming normal button handling\r\n");
        }
        return 0;  // Ignore this button event
    }

    //Detect button press
    if (Cy_GPIO_Read(PWR_PORT, PWR_NUM) == 0) {
        //Button press
        button_state = 0;

        power_off_cycles++;  // Increment the cycle count while button is pressed

        // If the button is held for at least 600 cycles (3 seconds) and device is not charging, set flag to shut down
        if (power_off_cycles >= BUTTON_HOLD_TIME) {
            shutdown_ready = true;  // Mark the system as ready for shutdown
            DBG_PRINTF("Button ready for shutdown.\r\n");
            power_led_blue();
        } else {
            DBG_PRINTF("Power off cycles: %d\r\n", power_off_cycles);
        }

    } else if (button_state == 0 && Cy_GPIO_Read(PWR_PORT, PWR_NUM) == 1) {
        //Button upress

        if (shutdown_ready) {
            DBG_PRINTF("Button released after %d cycles, powering down.\r\n", BUTTON_HOLD_TIME);
            CyDelayUs(100); // 100ms delay to help combat button "bounce"
            // Switch power state to power down
            ret = 2;

        // Device will stay in hibernate until a wake-up event occurs
        } else if (power_off_cycles > 0 && power_off_cycles < BUTTON_HOLD_TIME) {
            DBG_PRINTF("Button unpress event\r\n");

            //Momentary button press event
            ret = 1;
        }

        // Reset the power off cycle count and shutdown flag when the button is released
        power_off_cycles = 0;
        shutdown_ready = false;  // Reset shutdown flag

    } else {
        button_state = 1;
        ret = 0; //no change
    }


    return ret;
}


void power_task( void ) {
    DeviceStatus charger_status;
    uint8_t* data;
    bool result;
    int button_status;

    charger_status = check_charger();
    button_status = check_power_button_press();
    
    power_timer();

    power_flags_update(POWER_FLAG_CHG, !Cy_GPIO_Read(CHG_PG_PORT, CHG_PG_NUM) );

    switch (power_state) {


        default: //POWER_ADV (ADVERTISING only)

            if(power_timeout == 0){
                DBG_PRINTF("Auto shutdown - advertising timeout reached\r\n");
                power_state = POWER_DOWN;
                break;
            }
                
            if (charger_status == CHARGING) {
                reset_timer_cycles();
                DBG_PRINTF("START_CHARGER\r\n");
                power_state = START_CHARGER;
                break;
            }

            if(Cy_BLE_GetNumOfActiveConn() > 0u)
            {
                reset_timer_cycles();
                // Connected indication LED

                DBG_PRINTF("POWER_CONNECTED\r\n");
                power_state = POWER_CONNECTED;
                break;
            }

            if (button_status == 1) {
                //Single button press. Go to Preset 1
                DBG_PRINTF("Switched to Preset 1 Mode.\r\n");
                DBG_PRINTF("POWER_MODE_1\r\n");
                reset_timer_cycles();
                power_state = POWER_MODE_1;
                loadAndApplyPreset(1);
            } else if (button_status == 2) {
                //Press and hold. Power down
                DBG_PRINTF("POWER_DOWN\r\n");
                power_state = POWER_DOWN;
            }

            
        break;

        case POWER_CONNECTED:  //Connected to phone
            power_timeout = POWER_DISPLAY_TIMEOUT_INTERVAL;
            
            if (charger_status == CHARGING) {
                reset_timer_cycles();
                DBG_PRINTF("START_CHARGER\r\n");
                power_state = START_CHARGER;
                break;
            }

            if(Cy_BLE_GetNumOfActiveConn() == 0u) {
                reset_timer_cycles();
                DBG_PRINTF("POWER_ADV\r\n");
                power_state = POWER_ADV;
            }

            if (button_status == 2) {
                //Press and hold. Force Power down
                DBG_PRINTF("POWER_DOWN\r\n");
                power_state = POWER_DOWN;
            }


        break;

        case POWER_MODE_1:
            power_timeout = POWER_DISPLAY_TIMEOUT_INTERVAL;

            if (charger_status == CHARGING) {
                reset_timer_cycles();
                DBG_PRINTF("START_CHARGER\r\n");
                power_state = START_CHARGER;
                break;
            }
            
            if (button_status == 1) {
                //Single button press. Go to Preset 2
                DBG_PRINTF("Switched to Preset 2 Mode.\r\n");
                DBG_PRINTF("POWER_MODE_2\r\n");
                loadAndApplyPreset(2);
                reset_timer_cycles();
                power_state = POWER_MODE_2;
            } else if (button_status == 2) {
                //Press and hold. Power down
                DBG_PRINTF("POWER_DOWN\r\n");
                power_state = POWER_DOWN;
            }

        break;


        case POWER_MODE_2:
            power_timeout = POWER_DISPLAY_TIMEOUT_INTERVAL;

            if (charger_status == CHARGING) {
                reset_timer_cycles();
                DBG_PRINTF("START_CHARGER\r\n");
                power_state = START_CHARGER;
                break;
            }
            
            if (button_status == 1) {
                //Single button press. Go to Preset 3
                DBG_PRINTF("Switched to Preset 3 Mode.\r\n");
                DBG_PRINTF("POWER_MODE_3\r\n");
                loadAndApplyPreset(3);
                reset_timer_cycles();
                power_state = POWER_MODE_3;
            } else if (button_status == 2) {
                //Press and hold. Power down
                DBG_PRINTF("POWER_DOWN\r\n");
                power_state = POWER_DOWN;
            }

        break;


        case POWER_MODE_3:
            power_timeout = POWER_DISPLAY_TIMEOUT_INTERVAL;

            if (charger_status == CHARGING) {
                reset_timer_cycles();
                DBG_PRINTF("START_CHARGER\r\n");
                power_state = START_CHARGER;
                break;
            }
            
            if (button_status == 1) {
                //Single button press. Go to Preset 3
                DBG_PRINTF("Switched back to Bluetooth Mode.\r\n");
                shut_off_all_stimuli();
                DBG_PRINTF("POWER_ADV\r\n");
                reset_timer_cycles();
                power_state = POWER_ADV;
            } else if (button_status == 2) {
                //Press and hold. Power down
                DBG_PRINTF("POWER_DOWN\r\n");
                power_state = POWER_DOWN;
            }

        break;


        case START_CHARGER:
            power_timeout = POWER_DISPLAY_TIMEOUT_INTERVAL;
            shut_off_all_stimuli();
            // Set this to bluetooth mode since when we unplug we'll start from there and not in a preset
            cy_stc_ble_gap_disconnect_info_t disconnectInfo;
            disconnectInfo.bdHandle = cy_ble_connHandle[0].bdHandle;
            disconnectInfo.reason = CY_BLE_HCI_ERROR_OTHER_END_TERMINATED_USER;

            Cy_BLE_GAP_Disconnect(&disconnectInfo);
            Cy_BLE_GAPP_StopAdvertisement();

            data = (uint8_t*)"charging 0";
            result = send_data_to_phone(data, 10, CY_BLE_CUSTOM_SERVICE_CUSTOM_CHARACTERISTIC_CHAR_HANDLE);
            // was successful
            DBG_PRINTF("POWER_CHARGING\r\n");
            power_state = POWER_CHARGING;
        break;

        case POWER_CHARGING:
            power_timeout = POWER_DISPLAY_TIMEOUT_INTERVAL;
            if (charger_status == NOT_CHARGING || charger_status == NORMAL) {
                DBG_PRINTF("STOP_CHARGER\r\n");
                power_state = STOP_CHARGER;
                break;
            } else if (charger_status == FULLY_CHARGED) {
                DBG_PRINTF("POWER_CHARGED\r\n");
                power_state = POWER_CHARGED;
                break;
            }

        break;

        case POWER_CHARGED:
            power_timeout = POWER_DISPLAY_TIMEOUT_INTERVAL;
            if (charger_status == NOT_CHARGING || charger_status == NORMAL) {
                DBG_PRINTF("STOP_CHARGER\r\n");
                power_state = STOP_CHARGER;
                break;
            }

        break;

        case STOP_CHARGER:
            power_timeout = POWER_DISPLAY_TIMEOUT_INTERVAL;
            //Restart advertising
            if(Cy_BLE_GetAdvertisementState() == CY_BLE_ADV_STATE_STOPPED){
               Cy_BLE_GAPP_StartAdvertisement(CY_BLE_ADVERTISING_FAST, CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX);
            }

            data = (uint8_t*)"charging 1";
            result = send_data_to_phone(data, 10, CY_BLE_CUSTOM_SERVICE_CUSTOM_CHARACTERISTIC_CHAR_HANDLE);
            // was successful
            DBG_PRINTF("POWER_ADV\r\n");
            power_state = POWER_ADV;
        break;

        case POWER_DOWN:

            //FIXME: Disable WDT
            //FIXME: Configure power button as wakeup source
            DBG_PRINTF("Powering down\r\n");
            CyDelay(1000);
            power_off_device();

            //We wake up here
            DBG_PRINTF("Powering Up\r\n");

            power_state = POWER_ADV;
        break;
    }

    last_charger_status = charger_status;

    UpdateLedState();

    Cy_SysPm_Sleep(CY_SYSPM_WAIT_FOR_INTERRUPT);

}

/*
check_charger() - Reads the charging pin and detects if device is charging, fully charged, normal operation,
low battery, warning, etc. Updates LEDs and sends notification to phone when charging status is changed.
*/
DeviceStatus check_charger() {
    // Read charging status
    uint8_t charge_status_register[1];
    uint8_t lower_bits;
    bool isCharging = isDeviceCharging();
    power_i2c_read_reg(BQ25883_I2C_ADDR, 0xB, charge_status_register, 1);
    lower_bits = charge_status_register[0] & 0x07; // This grabs the 3 lower bits to determine charge status
    // Determine the current device status
    if (lower_bits == 0x06) {
        device_status = FULLY_CHARGED;
    } else if (isCharging) {
        device_status = CHARGING;
    } else {
        device_status = NOT_CHARGING;
    }

    return device_status;
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
    set_pel_pwm(0); // Turns off peltier

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

    switch (power_state) {
        default: //POWER_ADV (ADVERTISING only)
            power_led_advertising();
        break;

        case POWER_CONNECTED:
           power_led_connected();
        break;

        case POWER_MODE_1:
            preset_1_mode_led();
        break;


        case POWER_MODE_2:
            preset_2_mode_led();
        break;


        case POWER_MODE_3:
            preset_3_mode_led();
        break;

        case POWER_CHARGING:
            power_led_charging();
        break;

        case POWER_CHARGED:
            power_led_green();
        break;

        case POWER_DOWN:
            power_led_off();
        break;


    }

}
/* [] END OF FILE */


