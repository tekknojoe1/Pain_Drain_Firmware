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
*/
#include <project.h>
#include "debug.h"

void set_temp(int temperatureValue)
{
    if (temperatureValue == 0){
        DBG_PRINTF("Temperature off\r\n");   
    }
    else if(temperatureValue > 0 && temperatureValue <= 100){
        DBG_PRINTF("Heat on by %d percent\r\n", temperatureValue);   
    }
    else if(temperatureValue < 0 && temperatureValue >= -100){
        DBG_PRINTF("Cold on by %d percent\r\n", temperatureValue);   
    }
    else {
        DBG_PRINTF("ERROR\r\n");
    }
    
}



/* [] END OF FILE */
