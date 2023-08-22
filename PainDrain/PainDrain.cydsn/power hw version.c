/** 
 * Manages power and charging state machine

 */

#include <project.h>

#include "power.h"
#include "bq24298.h"

#define POWER_TIMER_PERIOD_MS 100
#define POWER_DISPLAY_TIMEOUT_SEC 20
#define POWER_DISPLAY_TIMEOUT_INTERVAL (POWER_DISPLAY_TIMEOUT_SEC*1000 / POWER_TIMER_PERIOD_MS)

#define POWER_IDLE 0
#define POWER_DOWN 10
static int power_state = 0;
static uint32 power_timeout = 0;
static uint32 power_flags = 0;   //Each bit indicates an active system. If all bits are zero we power down the entire device

#define I2C_BUFFER_SIZE 11
uint8_t i2cBuffer[I2C_BUFFER_SIZE];




void power_timer( void ) {
    //Called every 100ms when the WDT is enabled
    
    if (power_timeout > 0) {
        power_timeout--;
    }
}

void I2C_isr( void ) {
    Cy_SCB_I2C_Interrupt(I2C_HW, &I2C_context);   
}

void power_init( void ) {
    
    //I2C_Start();
    (void) Cy_SCB_I2C_Init(I2C_HW, &I2C_config, &I2C_context);
    /* Configure desired data rate.
    * Note that internally configured clock is utilized.
    */
    (void) Cy_SCB_I2C_SetDataRate(I2C_HW, I2C_DATA_RATE_HZ, I2C_CLK_FREQ_HZ);
    
    /* Hook interrupt service routine and enable interrupt */
    Cy_SysInt_Init(&I2C_SCB_IRQ_cfg, &I2C_isr);
    NVIC_EnableIRQ(I2C_SCB_IRQ_cfg.intrSrc);
    
    /* Enable I2C */
    Cy_SCB_I2C_Enable(I2C_HW);
    
    
    
    power_timeout = POWER_DISPLAY_TIMEOUT_INTERVAL;
    
    LCD_PWM_Start();
    LCD_PWM_SetCompare0(255);  //Set to full brightness
    
    bq24298_init();
}

void power_wakeup( void ) {
    
    power_timeout = POWER_DISPLAY_TIMEOUT_INTERVAL;
    LCD_PWM_SetCompare0(255);  //Set to full brightness 
    
}

void power_task( void ) {
    
    //Power button detection to wakeup
    if (Cy_GPIO_Read(PWR_BTN_PORT, PWR_BTN_NUM) == 0) {
        power_wakeup();
    }
    
    
    
    switch (power_state) {
     
        default: //POWER_IDLE
            
            //We just powered up from dead batteries or we reset
            //WDT should be enabled
            
            //WDT timer runs every 100 ms
        
            if (power_timeout == 0) {
                //We timed out so we should power down the display or ourselves
                //if (power_flags == 0) {
                    //We don't have any active sub devices running to lets power all the way off.
                    
                //    power_state = POWER_DOWN;
                //} else {
                    //Lets just turn off the display
                    LCD_PWM_SetCompare0(0);  
                    
                //}
            }
            
            Cy_SysPm_Sleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
        
        break;
        
        case POWER_DOWN:
            
            //FIXME: Disable WDT
            //FIXME: Configure power button as wakeup source
            
            Cy_SysPm_DeepSleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
            
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

void power_i2c_read_reg(uint8_t reg, uint8_t* d, int num_regs)
{
   cy_stc_scb_i2c_master_xfer_config_t transfer;
   uint8_t readByte = 0;
   uint8_t writeBuffer[1UL] = {reg};

   /* Configure write transaction */
   transfer.slaveAddress = BQ24298_I2C_ADDR;
   transfer.buffer = writeBuffer;
   transfer.bufferSize = 1; // Read only a single byte
   transfer.xferPending = true; /* Do not generate Stop condition at the end of transaction */

   /* Initiate write transaction.
   * The Start condition is generated to begin this transaction.
   */
   Cy_SCB_I2C_MasterWrite(I2C_HW, &transfer, &I2C_context);

   /* Wait for transaction completion */
   while (0UL != (CY_SCB_I2C_MASTER_BUSY & Cy_SCB_I2C_MasterGetStatus(SCB3, &I2C_context)))
   {
   }
   
   /* Configure read transaction */
   transfer.buffer = d;
   transfer.bufferSize = num_regs;
   transfer.xferPending = false; /* Generate Stop condition the end of transaction */

   /* Initiate read transaction.
   * The ReStart condition is generated to begin this transaction because
   * previous transaction was completed without Stop.
   */
   Cy_SCB_I2C_MasterRead(I2C_HW, &transfer, &I2C_context);
   
   /* Wait for transaction completion */
   while (0UL != (CY_SCB_I2C_MASTER_BUSY & Cy_SCB_I2C_MasterGetStatus(SCB3, &I2C_context)))
   {
   }
}


void power_i2c_read_reg2(int subAddr, uint8 d[], int num_regs) {
    
    cy_en_scb_i2c_status_t status;
        
    status = Cy_SCB_I2C_MasterSendStart(I2C_HW, BQ24298_I2C_ADDR, CY_SCB_I2C_WRITE_XFER, 50, &I2C_context);
    //if (status != CY_SCB_I2C_SUCCESS) {
    //    
    //    (void) Cy_SCB_I2C_MasterSendStop(I2C_HW, 0, &I2C_context);
    //    
    //    return status;
    //}
    
    status = Cy_SCB_I2C_MasterWriteByte(I2C_HW, subAddr, 50, &I2C_context);
    //if (status != CY_SCB_I2C_SUCCESS) {
    //    
    //    (void) Cy_SCB_I2C_MasterSendStop(I2C_HW, 0, &I2C_context);
    //    
    //    return status;
    //}
        
    status = Cy_SCB_I2C_MasterSendReStart(I2C_HW, BQ24298_I2C_ADDR, CY_SCB_I2C_READ_XFER, 50, &I2C_context);
    //if (status != CY_SCB_I2C_SUCCESS) {
    //    
    //    (void) Cy_SCB_I2C_MasterSendStop(I2C_HW, 0, &I2C_context);
    //    
    //    return status;
    //}
    
    /* Configure transaction */
    cy_stc_scb_i2c_master_xfer_config_t transaction;
    transaction.slaveAddress = BQ24298_I2C_ADDR;
    transaction.buffer = d;
    transaction.bufferSize = num_regs;
    transaction.xferPending = false;
    
    status = Cy_SCB_I2C_MasterRead(I2C_HW, &transaction, &I2C_context);
    //if (status != CY_SCB_I2C_SUCCESS) {
    //    
    //    (void) Cy_SCB_I2C_MasterSendStop(I2C_HW, 0, &I2C_context);
    //    
    //    return status;
    //}
    
    status = Cy_SCB_I2C_MasterSendStop(I2C_HW, 50, &I2C_context);
    //if (status != CY_SCB_I2C_SUCCESS) {
    //    return status;
    //}
        
}

void power_i2c_write_reg(int subAddr, int d) {
    
   
	I2C_MasterGetStatus();

	//Set subaddr
	I2C_MasterSendStart(BQ24298_I2C_ADDR, CY_SCB_I2C_WRITE_XFER, 0);
	I2C_MasterWriteByte(subAddr, 0);
	I2C_MasterWriteByte(d, 0);
	I2C_MasterSendStop(0);
	
    
}


