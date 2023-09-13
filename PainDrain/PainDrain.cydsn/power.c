/** 
 * Manages power and charging state machine

 */

#include <project.h>

#include "power.h"
#include "bq24298.h"
#include "my_i2c.h"


#define POWER_TIMER_PERIOD_MS 10
#define POWER_DISPLAY_TIMEOUT_SEC 20
#define POWER_DISPLAY_TIMEOUT_INTERVAL (POWER_DISPLAY_TIMEOUT_SEC*1000 / POWER_TIMER_PERIOD_MS)

#define POWER_IDLE 0
#define POWER_DOWN 10
#define MAX_LCD_PWM 191 // limit to 80 percent of 255
static int power_state = 0;
static uint32 power_timeout = 0;
static uint32 power_flags = 0;   //Each bit indicates an active system. If all bits are zero we power down the entire device





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
    
    
    myI2C_Start();
    
    //Cy_SysInt_Init(&gpio_irq_cfg, gpio_interrupt_handler);
	//NVIC_EnableIRQ(gpio_irq_cfg.intrSrc);
	//Unmask interrupt?
    
    power_timeout = POWER_DISPLAY_TIMEOUT_INTERVAL;
    
    LCD_PWM_Start();
    LCD_PWM_SetCompare0(MAX_LCD_PWM);  //Set to full brightness
    
    bq24298_init();
    
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
    LCD_PWM_SetCompare0(MAX_LCD_PWM);  //Set to full brightness 
    
}

void power_task( void ) {
    
    //Power button detection to wakeup
    if (Cy_GPIO_Read(PWR_BTN_PORT, PWR_BTN_NUM) == 0) {
        power_wakeup();
    }
    
    power_flags_update(POWER_FLAG_CHG, !Cy_GPIO_Read(CHG_PG_PORT, CHG_PG_NUM) );    

    switch (power_state) {
     
        default: //POWER_IDLE
            
            //We just powered up from dead batteries or we reset
            //WDT should be enabled
            
            //WDT timer runs every 100 ms
        
            if (power_timeout == 0) {
                //We timed out so we should power down the display or ourselves
                
                if (Cy_GPIO_Read(CHG_PG_PORT, CHG_PG_NUM) != 0) {  //Don't turn off the LCD screen if we are charging
                
                    //Lets just turn off the display
                    LCD_PWM_SetCompare0(0); 
                    
                    if (power_flags == 0) {
                        //We don't have any active sub devices running so lets power all the way off.
                        
                        power_state = POWER_DOWN;
                    } 
                } else {
                    power_wakeup(); 
                }   
            }
            
            Cy_SysPm_Sleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
        
        break;
        
        case POWER_DOWN:
            
            //FIXME: Disable WDT
            //FIXME: Configure power button as wakeup source
            
            //Cy_BLE_Stop();   
            //Cy_SysPm_DeepSleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
            //Cy_SysPm_Hibernate();
            
            //We wake up here
            power_wakeup();
            
            power_state = POWER_IDLE;            
        break;
        
        
    }
    
    
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
    }
    d[num_regs-1] = myI2C_I2CMasterReadByte(0);
    
    myI2C_I2CMasterSendStop();
    
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



