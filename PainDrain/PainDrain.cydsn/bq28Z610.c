/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
 * Driver for BQ28Z610-R1 Battery gauge
*/

#include <project.h>

#include "power.h"
#include "bq28Z610.h"
#include "debug.h"
#include "my_i2c.h"

void bq28Z610_write_reg(int reg, int d) {
    power_i2c_write_reg(BQ28Z610_I2C_ADDR, reg, d);
}

void bq28Z610_read_reg(uint8_t reg, uint8_t* d, int num_regs) {
    power_i2c_read_reg(BQ28Z610_I2C_ADDR, reg, d, num_regs);
}

void bq28Z610_init( void ){
    uint8_t d;
    //power_i2c_write_reg(BQ28Z610_I2C_ADDR, reg, d);
}

/* [] END OF FILE */
