/**
 *Copyright (c) 2015 - present LibDriver All rights reserved
 * 
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. 
 *
 * @file      driver_st7789_interface_template.c
 * @brief     driver st7789 interface template source file
 * @version   1.0.0
 * @author    Shifeng Li
 * @date      2023-04-15
 *
 * <h3>history</h3>
 * <table>
 * <tr><th>Date        <th>Version  <th>Author      <th>Description
 * <tr><td>2023/04/15  <td>1.0      <td>Shifeng Li  <td>first upload
 * </table>
 */

#include "driver_st7789_interface.h"
#include <project.h>
#include "debug.h"
//#include "st7735.c"

/**
 * @brief  interface spi bus init
 * @return status code
 *         - 0 success
 *         - 1 spi init failed
 * @note   none
 */
uint8_t st7789_interface_spi_init(void)
{
    cy_en_scb_spi_status_t initStatus;
        
    /* Configure component */
    initStatus = Cy_SCB_SPI_Init(SPI_HW, &SPI_config, NULL);
        
    Cy_SCB_SPI_Enable(SPI_HW);
    
    
    __enable_irq(); /* Enable global interrupts. */
    return 0;
}

/**
 * @brief  interface spi bus deinit
 * @return status code
 *         - 0 success
 *         - 1 spi deinit failed
 * @note   none
 */
uint8_t st7789_interface_spi_deinit(void)
{
    return 0;
}

/**
 * @brief     interface spi bus write
 * @param[in] *buf points to a data buffer
 * @param[in] len is the length of data buffer
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      none
 */
uint8_t st7789_interface_spi_write_cmd(uint8_t *buf, uint16_t len)
{
    uint32_t masterStatus;
    uint8_t d;
    uint8_t res;
    int count;
    
    // chip enable - active low
    //CLR_BIT (*(lcd->cs->port), lcd->cs->pin);
    //Cy_GPIO_Write(CS_PORT, CS_NUM, 0);
    // command (active low)
    //CLR_BIT (*(lcd->dc->port), lcd->dc->pin);
    //Cy_GPIO_Write(DC_PORT, DC_NUM, 0);
    // transmitting data
      
    /* Clear Master status and Tx FIFO status. */
    Cy_SCB_SPI_ClearSlaveMasterStatus(SPI_HW, masterStatus);
    Cy_SCB_SPI_ClearTxFifoStatus(SPI_HW, CY_SCB_SPI_TX_INTR_MASK );
    Cy_SCB_SPI_ClearTxFifo(SPI_HW);
    
    /* Clear Rx FIFO status. */
    Cy_SCB_SPI_ClearRxFifoStatus(SPI_HW, CY_SCB_SPI_RX_INTR_MASK );
    Cy_SCB_SPI_ClearRxFifo(SPI_HW);
    
    /*
    for(unsigned int i = 0; i < len; i++){
        res = Cy_SCB_SPI_Write(SPI_HW, buf[i]);
        DBG_PRINTF("value: %d\r\n",buf[i]);
    }
    */
    res = Cy_SCB_SPI_WriteArray(SPI_HW, buf, len);
    res = (res > 0) ? 0 : 1;

    //Cy_SCB_SPI_Write(SPI_HW, data);
    
    do {
        count = Cy_SCB_SPI_GetNumInRxFifo(SPI_HW);
    } while (count < 1);
        
    //do
    //{
    //    masterStatus  = Cy_SCB_SPI_GetSlaveMasterStatus(SPI_HW);
    //    
    //}while ( (masterStatus != (CY_SCB_SPI_MASTER_DONE)) );
    
    d = Cy_SCB_SPI_Read(SPI_HW);  
    
    
    //DBG_PRINTF("return status %d\r\n",res);
    //SPDR = data;
    // wait till data transmit
    //WAIT_UNTIL_BIT_IS_SET (SPSR, SPIF);
    // chip disable - idle high
    //SET_BIT (*(lcd->cs->port), lcd->cs->pin);
    //Cy_GPIO_Write(CS_PORT, CS_NUM, 1);
    // return received data
    return res;
}

/**
 * @brief     interface delay ms
 * @param[in] ms
 * @note      none
 */
void st7789_interface_delay_ms(uint32_t ms)
{
  // loop through real time
  while (ms--) {
    // 1 s delay
    //_delay_ms(1);
  }
}

/**
 * @brief     interface print format data
 * @param[in] fmt is the format data
 * @note      none
 */
void st7789_interface_debug_print(const char *const fmt, ...)
{
    
}

/**
 * @brief  interface command && data gpio init
 * @return status code
 *         - 0 success
 *         - 1 gpio init failed
 * @note   none
 */
uint8_t st7789_interface_cmd_data_gpio_init(void)
{
    return 0;
}

/**
 * @brief  interface command && data gpio deinit
 * @return status code
 *         - 0 success
 *         - 1 gpio deinit failed
 * @note   none
 */
uint8_t st7789_interface_cmd_data_gpio_deinit(void)
{
    return 0;
}

/**
 * @brief     interface command && data gpio write
 * @param[in] value is the written value
 * @return    status code
 *            - 0 success
 *            - 1 gpio write failed
 * @note      none
 */
uint8_t st7789_interface_cmd_data_gpio_write(uint8_t value)
{
    
    return 0;
}

/**
 * @brief  interface reset gpio init
 * @return status code
 *         - 0 success
 *         - 1 gpio init failed
 * @note   none
 */
uint8_t st7789_interface_reset_gpio_init(void)
{
    return 0;
}

/**
 * @brief  interface reset gpio deinit
 * @return status code
 *         - 0 success
 *         - 1 gpio deinit failed
 * @note   none
 */
uint8_t st7789_interface_reset_gpio_deinit(void)
{
    return 0;
}

/**
 * @brief     interface reset gpio write
 * @param[in] value is the written value
 * @return    status code
 *            - 0 success
 *            - 1 gpio write failed
 * @note      none
 */
uint8_t st7789_interface_reset_gpio_write(uint8_t value)
{
    return 0;
}
