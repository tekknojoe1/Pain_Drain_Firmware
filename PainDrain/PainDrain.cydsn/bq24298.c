/** 
 * Driver for BQ24298 Charger device

 */

#include <project.h>

#include "power.h"
#include "bq24298.h"

void bq24298_init( void ) {
    
    
    bq24298_write_reg(CHG_IN_SRC_CTRL_REG, 0x37);
    
    bq24298_write_reg(CHG_PWR_ON_CFG_REG, 0x0B);
    bq24298_write_reg(CHG_CHG_TERM_TMR_CTRL_REG, 0xC8);
    bq24298_write_reg(CHG_MISC_OP_CTRL_REG, 0x4B);
    
    bq24298_write_reg(CHG_PWR_ON_CFG_REG, 0x1B);  //Enable
}

void bq24298_write_reg(int reg, int d) {
    power_i2c_write_reg(reg, d);
}


void bq24298_read_reg(uint8_t reg, uint8_t* d, int num_regs) {
    power_i2c_read_reg(reg, d, num_regs);
}

