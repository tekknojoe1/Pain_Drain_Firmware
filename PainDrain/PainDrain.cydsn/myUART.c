/* ========================================
 *
 * Copyright FinkyDoodle, 2020
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF FinkyDoodle
 *
 * ========================================
*/

#include "myUART.h"

//Requires a GPIO PIN named MY_UART_PIN that is STRONG drive set high

#define BAUD_RATE 9600
#define BIT_TIME (1000000 / BAUD_RATE - 3)  //9600 -> 10us

void MY_UART_PIN_Write(int state) {
    Cy_GPIO_Write(MY_UART_PIN_PORT, MY_UART_PIN_NUM, state);
}


void myUART_PutChar(char c) {
    
    int shift_reg = (int)c;
    int i;
    
    //Start bit
    MY_UART_PIN_Write(0);
    CyDelayUs(BIT_TIME);
    
    for (i=0; i<8; i++) {
        MY_UART_PIN_Write(shift_reg & 0x1);
        shift_reg >>= 1;
        CyDelayUs(BIT_TIME);
    }
    //Stop bit
    MY_UART_PIN_Write(1);
    CyDelayUs(BIT_TIME);
    
}


/* [] END OF FILE */
