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
 * Driver for BQ25883 Charger device
*/

#include <project.h>

#include "power.h"
#include "bq25883.h"
#include "debug.h"
#include "my_i2c.h"

void bq25883_init( void ) {
    
    //0x30
    /*
    bq25883_write_reg(CHG_IN_SRC_CTRL_REG, 0x37);
   
    
    bq25883_write_reg(CHG_PWR_ON_CFG_REG, 0x0B);
    bq25883_write_reg(CHG_CHG_TERM_TMR_CTRL_REG, 0xC8);
    bq25883_write_reg(CHG_MISC_OP_CTRL_REG, 0x4B);
    
    bq25883_write_reg(CHG_PWR_ON_CFG_REG, 0x1B);  //Enable
    */
}

void bq25883_write_reg(int reg, int d) {
    power_i2c_write_reg(BQ25883_I2C_ADDR, reg, d);
}

void bq25883_read_reg(uint8_t reg, uint8_t* d, int num_regs) {
    power_i2c_read_reg(BQ25883_I2C_ADDR, reg, d, num_regs);
}

void bq25883_read_all_reg() {
    uint8_t reg_value;
    //DBG_PRINTF("READ ALL REG\r\n");
    //power_i2c_read_reg(BQ2588X_REG_CHARGE_VOLT, &reg_value, 2);
    //power_i2c_read_reg(BQ2588X_REG_CHARGE_CURRENT, &reg_value, 2);
    //power_i2c_read_reg(BQ2588X_REG_VINDPM, &reg_value, 2);
    //power_i2c_read_reg(BQ2588X_REG_IINDPM, &reg_value, 2);
    //power_i2c_read_reg(BQ2588X_REG_PRECHG_TERM, &reg_value, 2);
    power_i2c_read_reg(BQ25883_I2C_ADDR, BQ2588X_REG_CHG_CTRL1, &reg_value, 2);
    //power_i2c_read_reg(BQ2588X_REG_CHG_CTRL2, &reg_value, 2);
    //power_i2c_read_reg(BQ2588X_REG_CHG_CTRL3, &reg_value, 2);
    //power_i2c_read_reg(BQ2588X_REG_CHG_CTRL4, &reg_value, 2);
    //power_i2c_read_reg(BQ2588X_REG_OTG_CTRL, &reg_value, 2);
    //power_i2c_read_reg(BQ2588X_REG_ICO_LIMIT, &reg_value, 2);
    power_i2c_read_reg(BQ25883_I2C_ADDR, BQ2588X_REG_CHG_STATUS1, &reg_value, 2);
    power_i2c_read_reg(BQ25883_I2C_ADDR, BQ2588X_REG_CHG_STATUS2, &reg_value, 2);
    power_i2c_read_reg(BQ25883_I2C_ADDR, BQ2588X_REG_NTC_STATUS, &reg_value, 2);
    power_i2c_read_reg(BQ25883_I2C_ADDR, BQ2588X_REG_FAULT_STATUS, &reg_value, 2);
    //power_i2c_read_reg(BQ2588X_REG_CHG_FLAG1, &reg_value, 2);
    //power_i2c_read_reg(BQ2588X_REG_CHG_FLAG2, &reg_value, 2);
    //power_i2c_read_reg(BQ2588X_REG_FAULT_FLAG, &reg_value, 2);
    //power_i2c_read_reg(BQ2588X_REG_CHG_INT_MASK1, &reg_value, 2);
    //power_i2c_read_reg(BQ2588X_REG_CHG_INT_MASK2, &reg_value, 2);
    //power_i2c_read_reg(BQ2588X_REG_FAULT_INT_MASK, &reg_value, 2);
    //power_i2c_read_reg(BQ2588X_REG_ADC_CTRL, &reg_value, 2);
    //power_i2c_read_reg(BQ2588X_REG_ADC_FUNC_DIS, &reg_value, 2);
    //power_i2c_read_reg(BQ2588X_REG_IBUS_ADC_1, &reg_value, 2);
    //power_i2c_read_reg(BQ2588X_REG_IBUS_ADC_0, &reg_value, 2);
    //power_i2c_read_reg(BQ2588X_REG_ICHG_ADC_1, &reg_value, 2);
    //power_i2c_read_reg(BQ2588X_REG_ICHG_ADC_0, &reg_value, 2);
    //power_i2c_read_reg(BQ2588X_REG_VBUS_ADC_1, &reg_value, 2);
    //power_i2c_read_reg(BQ2588X_REG_VBUS_ADC_0, &reg_value, 2);
    //power_i2c_read_reg(BQ2588X_REG_VBAT_ADC_1, &reg_value, 2);
    //power_i2c_read_reg(BQ2588X_REG_VBAT_ADC_0, &reg_value, 2);
    //power_i2c_read_reg(BQ2588X_REG_VSYS_ADC_1, &reg_value, 2);
    //power_i2c_read_reg(BQ2588X_REG_VSYS_ADC_0, &reg_value, 2);
    //power_i2c_read_reg(BQ2588X_REG_TS_ADC_1, &reg_value, 2);
    //power_i2c_read_reg(BQ2588X_REG_TS_ADC_0, &reg_value, 2);
    //power_i2c_read_reg(BQ2588X_REG_TDIE_ADC_1, &reg_value, 2);
    //power_i2c_read_reg(BQ2588X_REG_TDIE_ADC_0, &reg_value, 2);
    //power_i2c_read_reg(BQ2588X_REG_PART_NUM, &reg_value, 2);
}
/* [] END OF FILE */
