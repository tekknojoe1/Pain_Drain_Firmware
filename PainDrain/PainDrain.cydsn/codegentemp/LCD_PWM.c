/*******************************************************************************
* File Name: LCD_PWM.c
* Version 1.0
*
* Description:
*  This file provides the source code to the API for the LCD_PWM
*  component.
*
********************************************************************************
* Copyright 2016-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "LCD_PWM.h"

/** Indicates whether or not the LCD_PWM has been initialized. 
*  The variable is initialized to 0 and set to 1 the first time 
*  LCD_PWM_Start() is called. This allows the Component to 
*  restart without reinitialization after the first call to 
*  the LCD_PWM_Start() routine.
*/
uint8_t LCD_PWM_initVar = 0U;

/** The instance-specific configuration structure. This should be used in the 
*  associated LCD_PWM_Init() function.
*/ 
cy_stc_tcpwm_pwm_config_t const LCD_PWM_config =
{
    .pwmMode = 4UL,
    .clockPrescaler = 7UL,
    .pwmAlignment = 0UL,
    .deadTimeClocks = 0UL,
    .runMode = 0UL,
    .period0 = 255UL,
    .period1 = 32768UL,
    .enablePeriodSwap = false,
    .compare0 = 0UL,
    .compare1 = 16384UL,
    .enableCompareSwap = false,
    .interruptSources = 0UL,
    .invertPWMOut = 0UL,
    .invertPWMOutN = 0UL,
    .killMode = 2UL,
    .swapInputMode = 3UL,
    .swapInput = CY_TCPWM_INPUT_CREATOR,
    .reloadInputMode = 3UL,
    .reloadInput = CY_TCPWM_INPUT_CREATOR,
    .startInputMode = 3UL,
    .startInput = CY_TCPWM_INPUT_CREATOR,
    .killInputMode = 3UL,
    .killInput = CY_TCPWM_INPUT_CREATOR,
    .countInputMode = 3UL,
    .countInput = CY_TCPWM_INPUT_CREATOR,
};


/*******************************************************************************
* Function Name: LCD_PWM_Start
****************************************************************************//**
*
*  Calls the LCD_PWM_Init() when called the first time and enables 
*  the LCD_PWM. For subsequent calls the configuration is left 
*  unchanged and the component is just enabled.
*
* \globalvars
*  \ref LCD_PWM_initVar
*
*******************************************************************************/
void LCD_PWM_Start(void)
{
    if (0U == LCD_PWM_initVar)
    {
        (void) Cy_TCPWM_PWM_Init(LCD_PWM_HW, LCD_PWM_CNT_NUM, &LCD_PWM_config);

        LCD_PWM_initVar = 1U;
    }

    Cy_TCPWM_Enable_Multiple(LCD_PWM_HW, LCD_PWM_CNT_MASK);
    
    #if (LCD_PWM_INPUT_DISABLED == 7UL)
        Cy_TCPWM_TriggerStart(LCD_PWM_HW, LCD_PWM_CNT_MASK);
    #endif /* (LCD_PWM_INPUT_DISABLED == 7UL) */    
}


/* [] END OF FILE */
