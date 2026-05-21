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

// Writes a 16-bit word little-endian to a register (required for ManufacturerAccess commands)
static void bq28Z610_write_word(uint8_t reg, uint16_t word) {
    myI2C_I2CMasterClearStatus();
    myI2C_I2CMasterSendStart(BQ28Z610_I2C_ADDR, 0);
    myI2C_I2CMasterWriteByte(reg);
    myI2C_I2CMasterWriteByte((uint8_t)(word & 0xFF));
    myI2C_I2CMasterWriteByte((uint8_t)(word >> 8));
    myI2C_I2CMasterSendStop();
}

void bq28Z610_read_reg(uint8_t reg, uint8_t* d, int num_regs) {
    power_i2c_read_reg(BQ28Z610_I2C_ADDR, reg, d, num_regs);
}

void bq28Z610_read_all_reg() {
    //power_i2c_read_reg(BQ28Z610_I2C_ADDR, 0x00, &reg_value, 2);
}

// Returns SEC1:SEC0 value: 3=sealed, 2=unsealed, 1=full-access
static int bq28Z610_read_sec_mode(void) {
    uint8_t d[2] = {0xFF, 0xFF};
    bq28Z610_read_reg(0x00, d, 2);
    uint16_t mac = (uint16_t)((d[1] << 8) | d[0]);
    return (mac >> 8) & 0x03;  // SEC1:SEC0 at bits 9:8 (OperationStatus format)
}

void bq28Z610_init( void ){
    bq28Z610_write_word(0x3E, 0x0414);
    CyDelay(2);
    bq28Z610_write_word(0x3E, 0x3672);
    CyDelay(100);  // give the gauge time to process the unseal

    int sec = bq28Z610_read_sec_mode();
    // SEC1:SEC0: 3=sealed, 2=unsealed, 1=full-access
    if (sec == 2 || sec == 1) {
        DBG_PRINTF("bq28Z610 unsealed (SEC=%d)\r\n", sec);
    } else {
        DBG_PRINTF("bq28Z610 STILL SEALED (SEC=%d) — unseal failed\r\n", sec);
    }
}

uint8_t bq28Z610_get_soc(void) {
    uint8_t d[2];
    bq28Z610_read_reg(BQ28Z610_REG_SOC, d, 2);
    return d[0];  // low byte is 0-100 %
}

uint16_t bq28Z610_get_voltage_mv(void) {
    uint8_t d[2];
    bq28Z610_read_reg(BQ28Z610_REG_VOLTAGE, d, 2);
    return (uint16_t)((d[1] << 8) | d[0]);  // little-endian mV
}

/* [] END OF FILE */
