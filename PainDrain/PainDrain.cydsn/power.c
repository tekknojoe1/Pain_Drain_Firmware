/** 
 * Manages power and charging state machine

 */

#include <project.h>

#include "power.h"
#include "bq24298.h"
#include "my_i2c.h"
#include "debug.h"
#include "bq25883.h"


#define POWER_TIMER_PERIOD_MS 10
#define POWER_DISPLAY_TIMEOUT_SEC 20
#define POWER_DISPLAY_TIMEOUT_INTERVAL (POWER_DISPLAY_TIMEOUT_SEC*1000 / POWER_TIMER_PERIOD_MS)

#define POWER_IDLE 0
#define POWER_DOWN 10
#define MAX_LCD_PWM 191 // limit to 80 percent of 255
static int power_state = 0;
static uint32 power_timeout = 0;
static uint32 power_flags = 0;   //Each bit indicates an active system. If all bits are zero we power down the entire device
uint8_t chargingStatus;
bool triggerBattery = false;
bool isConnected = false;
bool chargingValueSent = false;
bool notChargingValueSent = false;
//int reg_num = 0;

void power_led_off(void) {
    Cy_GPIO_Write(LED_RED_PORT, LED_RED_NUM, 0);
    Cy_GPIO_Write(LED_GREEN_PORT, LED_GREEN_NUM, 0);
    Cy_GPIO_Write(LED_BLUE_PORT, LED_BLUE_NUM, 0);
}

void power_led_lowbatt(void) {
    Cy_GPIO_Write(LED_RED_PORT, LED_RED_NUM, 1);
    Cy_GPIO_Write(LED_GREEN_PORT, LED_GREEN_NUM, 0);
    Cy_GPIO_Write(LED_BLUE_PORT, LED_BLUE_NUM, 0);
}

void power_led_charging(void) {
    Cy_GPIO_Write(LED_RED_PORT, LED_RED_NUM, 1);
    Cy_GPIO_Write(LED_GREEN_PORT, LED_GREEN_NUM, 1);
    Cy_GPIO_Write(LED_BLUE_PORT, LED_BLUE_NUM, 0);
}

void power_led_charged(void) {
    Cy_GPIO_Write(LED_RED_PORT, LED_RED_NUM, 0);
    Cy_GPIO_Write(LED_GREEN_PORT, LED_GREEN_NUM, 1);
    Cy_GPIO_Write(LED_BLUE_PORT, LED_BLUE_NUM, 0);
}

void power_led_ble(void) {
    Cy_GPIO_Write(LED_RED_PORT, LED_RED_NUM, 0);
    Cy_GPIO_Write(LED_GREEN_PORT, LED_GREEN_NUM, 0);
    Cy_GPIO_Write(LED_BLUE_PORT, LED_BLUE_NUM, 1);
}

void power_led_advertising(void){
    power_led_ble();
    CyDelay(500); // Wait half for blink effect
    power_led_off();
}
void power_led_connected(void){
    power_led_ble();
    CyDelay(3000); // Wait 3 seconds
    power_led_off();
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


void power_init( void ) {
    
    
    //myI2C_Start();
    
    //Cy_SysInt_Init(&gpio_irq_cfg, gpio_interrupt_handler);
	//NVIC_EnableIRQ(gpio_irq_cfg.intrSrc);
	//Unmask interrupt?
    
    power_timeout = POWER_DISPLAY_TIMEOUT_INTERVAL;
    
    //LCD_PWM_Start();
    //LCD_PWM_SetCompare0(MAX_LCD_PWM);  //Set to full brightness
    DBG_PRINTF("Backlight on\r\n");

    
    //bq24298_init();
    
    power_flags_update(POWER_FLAG_BLE, 1);
}

void power_5v_on( void ) {
    uint8 d;
    
    power_i2c_read_reg(CHG_PWR_ON_CFG_REG, &d, 1);
    d |= CHG_PWR_ON_CFG_OTGCONFIG_MASK;
    power_i2c_write_reg(CHG_PWR_ON_CFG_REG, d);    
    
}

void power_5v_off( void ) {
    uint8 d;
    
    power_i2c_read_reg(CHG_PWR_ON_CFG_REG, &d, 1);
    d &= ~CHG_PWR_ON_CFG_OTGCONFIG_MASK;
    power_i2c_write_reg(CHG_PWR_ON_CFG_REG, d);  
    
}

void power_wakeup( void ) {
    
    power_timeout = POWER_DISPLAY_TIMEOUT_INTERVAL;
    //LCD_PWM_SetCompare0(MAX_LCD_PWM);  //Set to full brightness 
    
}

void power_task( void ) {
    
    //reg_num = 0;
    bq25883_read_all_reg();
    
    
    if (Cy_GPIO_Read(CHG_STAT_PORT, CHG_STAT_NUM) == 0) {
        //power_led_charging();
    } else {
        //power_led_off();
    }
    
    //Power button detection to wakeup

    //DBG_PRINTF("Charging PIN Status %d\r\n", Cy_GPIO_Read(CHG_STAT_0_PORT, CHG_STAT_0_NUM));
    // Calls get battery status to read charge pin
    uint8_t newChargingStatus = get_charging_status();
    if(chargingStatus != newChargingStatus){
        // Assigns value change to global variable
        chargingStatus = newChargingStatus;
        if(chargingStatus == 1){
            // Its charging
            //DBG_PRINTF("Charging\r\n");
            
            uint8_t* data;
            data = (uint8_t*)"charging 1";
            
            // Sends to phone and returns a bool
            bool result = send_data_to_phone(data, 10, CY_BLE_CUSTOM_SERVICE_CUSTOM_CHARACTERISTIC_CHAR_HANDLE);
            // Was successful
            if(result){
                DBG_PRINTF("Charging Status sent\r\n");
            }
            
        } else if(chargingStatus == 0){
            // Its not charging
            DBG_PRINTF("Not Charging\r\n");
            
            uint8_t* data;
            data = (uint8_t*)"charging 0";
            
            // Sends to phone and returns a bool
            bool result = send_data_to_phone(data, 10, CY_BLE_CUSTOM_SERVICE_CUSTOM_CHARACTERISTIC_CHAR_HANDLE);
            // was successful
            if(result){
                DBG_PRINTF("Not Charging Status sent\r\n");
            }
        } else {
            // Charging Status wasn't 0 or 1
            DBG_PRINTF("Charge Status is not 1 or 0\r\n");
        }
        
    }
    
    if (Cy_GPIO_Read(PWR_PORT, PWR_NUM) == 0) {
        power_wakeup();
        DBG_PRINTF("Waking up\r\n");
        
        // test code to test charging notification just so it doesn't execute for however long the user holds the button down
        //power_led_charging();
        CyDelay(2000);
        uint8_t* data;
        if(triggerBattery){
            data = (uint8_t*)"charging 1";
            send_data_to_phone(data, 10, CY_BLE_CUSTOM_SERVICE_CUSTOM_CHARACTERISTIC_CHAR_HANDLE);
        } else{
            data = (uint8_t*)"charging 0";
            send_data_to_phone(data, 10, CY_BLE_CUSTOM_SERVICE_CUSTOM_CHARACTERISTIC_CHAR_HANDLE);
        }
        triggerBattery = !triggerBattery;
        //power_led_off();
        // end of test code
        
    }
    cy_en_gpio_status_t initStatus;
    
    //initStatus = Cy_GPIO_Pin_Init(PWR_BTN_PORT, PWR_BTN_NUM, &PWR_BTN_);
    //DBG_PRINTF("Power flag %d\r\n", power_flags);
    power_flags_update(POWER_FLAG_CHG, !Cy_GPIO_Read(CHG_PG_PORT, CHG_PG_NUM) );    

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
                    
                    if (power_flags == 0) {
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
            
            power_led_off();

            Cy_BLE_Stop();   
            Cy_SysPm_DeepSleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
            Cy_SysPm_Hibernate();
            
            //We wake up here
            power_wakeup();
            DBG_PRINTF("Powering down\r\n");
            
            power_state = POWER_IDLE;            
        break;
    }  
}

/*
get_charging_status() - reads the charging pin value. 1 - Not Charging, 0 - Charging
*/
uint32_t get_charging_status(){
    return Cy_GPIO_Read(CHG_STAT_0_PORT, CHG_STAT_0_NUM);
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
   
    // Sends notification
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
    return false;
    
}

void power_get_diag_data(uint8 d[]) {
    
    memset(d, 0, CHG_VNDR_PART_REV_REG+1);
    
    power_i2c_read_reg(0, d, CHG_VNDR_PART_REV_REG+1);  
      
        
}

void power_i2c_read_reg(uint8_t reg, uint8_t* d, int num_regs) {
    int status;
    int i;
    
    myI2C_I2CMasterClearStatus();
    
    status = myI2C_I2CMasterSendStart(BQ24298_I2C_ADDR, 0);
    status = myI2C_I2CMasterWriteByte(reg);
    status = myI2C_I2CMasterSendRestart(BQ24298_I2C_ADDR, 1);
    
    for (i=0;i<num_regs-1;i++) {
        d[i] = myI2C_I2CMasterReadByte(1);
        if(reg_array[reg] != d[i]){
            DBG_PRINTF("Different reg 0x%x: old: %d new: %d\r\n", reg, reg_array[reg], d[i]); 
            reg_array[reg] = d[i];
        }
    }
    d[num_regs-1] = myI2C_I2CMasterReadByte(0);
    
    myI2C_I2CMasterSendStop();
    //reg_num++;
    
}

void power_i2c_write_reg(uint8_t reg, uint8_t d) {
    int status;
    
    myI2C_I2CMasterClearStatus();
    status = myI2C_I2CMasterSendStart(BQ24298_I2C_ADDR, 0);
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
    if(Cy_BLE_GetAdvertisementState() == CY_BLE_ADV_STATE_ADVERTISING)
    {
        //Blink advertising indication LED
        power_led_advertising();
    }
    // Handles connected LED and executes once per connection
    else if(Cy_BLE_GetNumOfActiveConn() > 0u && !isConnected)
    {
        
        DBG_PRINTF("Connected\r\n");
        // Connected indication LED
        power_led_connected();
        isConnected = true;
        
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
        DBG_PRINTF("Disconnected\r\n");
        isConnected = false;
    }
    // Handles Charging LED
    else if(batteryStatus == CHARGING)
    {
        power_led_off();
        power_led_charging();

        if(!chargingValueSent){
            uint8_t* data;
            data = (uint8_t*)"charging 0";
            //DBG_PRINTF("Charging HERE\r\n");
            bool result = send_data_to_phone(data, 10, CY_BLE_CUSTOM_SERVICE_CUSTOM_CHARACTERISTIC_CHAR_HANDLE);
            // was successful
            if(result){
                DBG_PRINTF("Charging Status sent\r\n");
                notChargingValueSent = false;
                chargingValueSent = true;
            }
        }
    }
    else if(batteryStatus == FULLY_CHARGED)
    {
        power_led_off();
        power_led_charged();
        //isCharging = false;
        //DBG_PRINTF("Fully Charged\r\n");
        
    }
    else if(batteryStatus == LOW_BATTERY)
    {
        power_led_off();
        power_led_lowbatt();
        //isCharging = false;
        //DBG_PRINTF("Low Battery\r\n");
        
    }
    else if(batteryStatus == NOT_CHARGING)
    {
        power_led_off();
        if(!notChargingValueSent){
            uint8_t* data;
            data = (uint8_t*)"charging 1";
            //DBG_PRINTF("Charging HERE\r\n");
            bool result = send_data_to_phone(data, 10, CY_BLE_CUSTOM_SERVICE_CUSTOM_CHARACTERISTIC_CHAR_HANDLE);
            // was successful
            if(result){
                DBG_PRINTF("Not Charging Status sent\r\n");
                notChargingValueSent = true;
                chargingValueSent = false;
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


