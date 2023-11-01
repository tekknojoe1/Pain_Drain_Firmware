/*******************************************************************************
* File Name: POWER_BUTTON.h
* Version 1.0
*
* Description:
*  This file provides constants and parameter values for the POWER_BUTTON
*  component.
*
********************************************************************************
* Copyright 2016-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(POWER_BUTTON_CY_TCPWM_PWM_PDL_H)
#define POWER_BUTTON_CY_TCPWM_PWM_PDL_H

#include "cyfitter.h"
#include "tcpwm/cy_tcpwm_pwm.h"

   
/*******************************************************************************
* Variables
*******************************************************************************/
/**
* \addtogroup group_globals
* @{
*/
extern uint8_t  POWER_BUTTON_initVar;
extern cy_stc_tcpwm_pwm_config_t const POWER_BUTTON_config;
/** @} group_globals */


/***************************************
*        Function Prototypes
****************************************/
/**
* \addtogroup group_general
* @{
*/
void POWER_BUTTON_Start(void);
__STATIC_INLINE cy_en_tcpwm_status_t POWER_BUTTON_Init(cy_stc_tcpwm_pwm_config_t const *config);
__STATIC_INLINE void POWER_BUTTON_DeInit(void);
__STATIC_INLINE void POWER_BUTTON_Enable(void);
__STATIC_INLINE void POWER_BUTTON_Disable(void);
__STATIC_INLINE uint32_t POWER_BUTTON_GetStatus(void);
__STATIC_INLINE void POWER_BUTTON_SetCompare0(uint32_t compare0);
__STATIC_INLINE uint32_t POWER_BUTTON_GetCompare0(void);
__STATIC_INLINE void POWER_BUTTON_SetCompare1(uint32_t compare1);
__STATIC_INLINE uint32_t POWER_BUTTON_GetCompare1(void);
__STATIC_INLINE void POWER_BUTTON_EnableCompareSwap(bool enable);
__STATIC_INLINE void POWER_BUTTON_SetCounter(uint32_t count);
__STATIC_INLINE uint32_t POWER_BUTTON_GetCounter(void);
__STATIC_INLINE void POWER_BUTTON_SetPeriod0(uint32_t period0);
__STATIC_INLINE uint32_t POWER_BUTTON_GetPeriod0(void);
__STATIC_INLINE void POWER_BUTTON_SetPeriod1(uint32_t period1);
__STATIC_INLINE uint32_t POWER_BUTTON_GetPeriod1(void);
__STATIC_INLINE void POWER_BUTTON_EnablePeriodSwap(bool enable);
__STATIC_INLINE void POWER_BUTTON_TriggerStart(void);
__STATIC_INLINE void POWER_BUTTON_TriggerReload(void);
__STATIC_INLINE void POWER_BUTTON_TriggerKill(void);
__STATIC_INLINE void POWER_BUTTON_TriggerSwap(void);
__STATIC_INLINE uint32_t POWER_BUTTON_GetInterruptStatus(void);
__STATIC_INLINE void POWER_BUTTON_ClearInterrupt(uint32_t source);
__STATIC_INLINE void POWER_BUTTON_SetInterrupt(uint32_t source);
__STATIC_INLINE void POWER_BUTTON_SetInterruptMask(uint32_t mask);
__STATIC_INLINE uint32_t POWER_BUTTON_GetInterruptMask(void);
__STATIC_INLINE uint32_t POWER_BUTTON_GetInterruptStatusMasked(void);
/** @} general */


/***************************************
*           API Constants
***************************************/

/**
* \addtogroup group_macros
* @{
*/
/** This is a ptr to the base address of the TCPWM instance */
#define POWER_BUTTON_HW                 (POWER_BUTTON_TCPWM__HW)

/** This is a ptr to the base address of the TCPWM CNT instance */
#define POWER_BUTTON_CNT_HW             (POWER_BUTTON_TCPWM__CNT_HW)

/** This is the counter instance number in the selected TCPWM */
#define POWER_BUTTON_CNT_NUM            (POWER_BUTTON_TCPWM__CNT_IDX)

/** This is the bit field representing the counter instance in the selected TCPWM */
#define POWER_BUTTON_CNT_MASK           (1UL << POWER_BUTTON_CNT_NUM)
/** @} group_macros */

#define POWER_BUTTON_INPUT_MODE_MASK    (0x3U)
#define POWER_BUTTON_INPUT_DISABLED     (7U)


/*******************************************************************************
* Function Name: POWER_BUTTON_Init
****************************************************************************//**
*
* Invokes the Cy_TCPWM_PWM_Init() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE cy_en_tcpwm_status_t POWER_BUTTON_Init(cy_stc_tcpwm_pwm_config_t const *config)
{
    return(Cy_TCPWM_PWM_Init(POWER_BUTTON_HW, POWER_BUTTON_CNT_NUM, config));
}


/*******************************************************************************
* Function Name: POWER_BUTTON_DeInit
****************************************************************************//**
*
* Invokes the Cy_TCPWM_PWM_DeInit() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void POWER_BUTTON_DeInit(void)                   
{
    Cy_TCPWM_PWM_DeInit(POWER_BUTTON_HW, POWER_BUTTON_CNT_NUM, &POWER_BUTTON_config);
}

/*******************************************************************************
* Function Name: POWER_BUTTON_Enable
****************************************************************************//**
*
* Invokes the Cy_TCPWM_Enable_Multiple() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void POWER_BUTTON_Enable(void)                   
{
    Cy_TCPWM_Enable_Multiple(POWER_BUTTON_HW, POWER_BUTTON_CNT_MASK);
}


/*******************************************************************************
* Function Name: POWER_BUTTON_Disable
****************************************************************************//**
*
* Invokes the Cy_TCPWM_Disable_Multiple() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void POWER_BUTTON_Disable(void)                  
{
    Cy_TCPWM_Disable_Multiple(POWER_BUTTON_HW, POWER_BUTTON_CNT_MASK);
}


/*******************************************************************************
* Function Name: POWER_BUTTON_GetStatus
****************************************************************************//**
*
* Invokes the Cy_TCPWM_PWM_GetStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t POWER_BUTTON_GetStatus(void)                
{
    return(Cy_TCPWM_PWM_GetStatus(POWER_BUTTON_HW, POWER_BUTTON_CNT_NUM));
}


/*******************************************************************************
* Function Name: POWER_BUTTON_SetCompare0
****************************************************************************//**
*
* Invokes the Cy_TCPWM_PWM_SetCompare0() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void POWER_BUTTON_SetCompare0(uint32_t compare0)      
{
    Cy_TCPWM_PWM_SetCompare0(POWER_BUTTON_HW, POWER_BUTTON_CNT_NUM, compare0);
}


/*******************************************************************************
* Function Name: POWER_BUTTON_GetCompare0
****************************************************************************//**
*
* Invokes the Cy_TCPWM_PWM_GetCompare0() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t POWER_BUTTON_GetCompare0(void)              
{
    return(Cy_TCPWM_PWM_GetCompare0(POWER_BUTTON_HW, POWER_BUTTON_CNT_NUM));
}


/*******************************************************************************
* Function Name: POWER_BUTTON_SetCompare1
****************************************************************************//**
*
* Invokes the Cy_TCPWM_PWM_SetCompare1() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void POWER_BUTTON_SetCompare1(uint32_t compare1)      
{
    Cy_TCPWM_PWM_SetCompare1(POWER_BUTTON_HW, POWER_BUTTON_CNT_NUM, compare1);
}


/*******************************************************************************
* Function Name: POWER_BUTTON_GetCompare1
****************************************************************************//**
*
* Invokes the Cy_TCPWM_PWM_GetCompare1() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t POWER_BUTTON_GetCompare1(void)              
{
    return(Cy_TCPWM_PWM_GetCompare1(POWER_BUTTON_HW, POWER_BUTTON_CNT_NUM));
}


/*******************************************************************************
* Function Name: POWER_BUTTON_EnableCompareSwap
****************************************************************************//**
*
* Invokes the Cy_TCPWM_PWM_EnableCompareSwap() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void POWER_BUTTON_EnableCompareSwap(bool enable)  
{
    Cy_TCPWM_PWM_EnableCompareSwap(POWER_BUTTON_HW, POWER_BUTTON_CNT_NUM, enable);
}


/*******************************************************************************
* Function Name: POWER_BUTTON_SetCounter
****************************************************************************//**
*
* Invokes the Cy_TCPWM_PWM_SetCounter() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void POWER_BUTTON_SetCounter(uint32_t count)          
{
    Cy_TCPWM_PWM_SetCounter(POWER_BUTTON_HW, POWER_BUTTON_CNT_NUM, count);
}


/*******************************************************************************
* Function Name: POWER_BUTTON_GetCounter
****************************************************************************//**
*
* Invokes the Cy_TCPWM_PWM_GetCounter() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t POWER_BUTTON_GetCounter(void)               
{
    return(Cy_TCPWM_PWM_GetCounter(POWER_BUTTON_HW, POWER_BUTTON_CNT_NUM));
}


/*******************************************************************************
* Function Name: POWER_BUTTON_SetPeriod0
****************************************************************************//**
*
* Invokes the Cy_TCPWM_PWM_SetPeriod0() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void POWER_BUTTON_SetPeriod0(uint32_t period0)          
{
    Cy_TCPWM_PWM_SetPeriod0(POWER_BUTTON_HW, POWER_BUTTON_CNT_NUM, period0);
}


/*******************************************************************************
* Function Name: POWER_BUTTON_GetPeriod0
****************************************************************************//**
*
* Invokes the Cy_TCPWM_PWM_GetPeriod0() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t POWER_BUTTON_GetPeriod0(void)                
{
    return(Cy_TCPWM_PWM_GetPeriod0(POWER_BUTTON_HW, POWER_BUTTON_CNT_NUM));
}


/*******************************************************************************
* Function Name: POWER_BUTTON_SetPeriod1
****************************************************************************//**
*
* Invokes the Cy_TCPWM_PWM_SetPeriod1() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void POWER_BUTTON_SetPeriod1(uint32_t period1)
{
    Cy_TCPWM_PWM_SetPeriod1(POWER_BUTTON_HW, POWER_BUTTON_CNT_NUM, period1);
}


/*******************************************************************************
* Function Name: POWER_BUTTON_GetPeriod1
****************************************************************************//**
*
* Invokes the Cy_TCPWM_PWM_GetPeriod1() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t POWER_BUTTON_GetPeriod1(void)                
{
    return(Cy_TCPWM_PWM_GetPeriod1(POWER_BUTTON_HW, POWER_BUTTON_CNT_NUM));
}


/*******************************************************************************
* Function Name: POWER_BUTTON_EnablePeriodSwap
****************************************************************************//**
*
* Invokes the Cy_TCPWM_PWM_EnablePeriodSwap() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void POWER_BUTTON_EnablePeriodSwap(bool enable)
{
    Cy_TCPWM_PWM_EnablePeriodSwap(POWER_BUTTON_HW, POWER_BUTTON_CNT_NUM, enable);
}


/*******************************************************************************
* Function Name: POWER_BUTTON_TriggerStart
****************************************************************************//**
*
* Invokes the Cy_TCPWM_TriggerStart() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void POWER_BUTTON_TriggerStart(void)             
{
    Cy_TCPWM_TriggerStart(POWER_BUTTON_HW, POWER_BUTTON_CNT_MASK);
}


/*******************************************************************************
* Function Name: POWER_BUTTON_TriggerReload
****************************************************************************//**
*
* Invokes the Cy_TCPWM_TriggerReloadOrIndex() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void POWER_BUTTON_TriggerReload(void)     
{
    Cy_TCPWM_TriggerReloadOrIndex(POWER_BUTTON_HW, POWER_BUTTON_CNT_MASK);
}


/*******************************************************************************
* Function Name: POWER_BUTTON_TriggerKill
****************************************************************************//**
*
* Invokes the Cy_TCPWM_TriggerStopOrKill() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void POWER_BUTTON_TriggerKill(void)
{
    Cy_TCPWM_TriggerStopOrKill(POWER_BUTTON_HW, POWER_BUTTON_CNT_MASK);
}


/*******************************************************************************
* Function Name: POWER_BUTTON_TriggerSwap
****************************************************************************//**
*
* Invokes the Cy_TCPWM_TriggerCaptureOrSwap() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void POWER_BUTTON_TriggerSwap(void)     
{
    Cy_TCPWM_TriggerCaptureOrSwap(POWER_BUTTON_HW, POWER_BUTTON_CNT_MASK);
}


/*******************************************************************************
* Function Name: POWER_BUTTON_GetInterruptStatus
****************************************************************************//**
*
* Invokes the Cy_TCPWM_GetInterruptStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t POWER_BUTTON_GetInterruptStatus(void)       
{
    return(Cy_TCPWM_GetInterruptStatus(POWER_BUTTON_HW, POWER_BUTTON_CNT_NUM));
}


/*******************************************************************************
* Function Name: POWER_BUTTON_ClearInterrupt
****************************************************************************//**
*
* Invokes the Cy_TCPWM_ClearInterrupt() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void POWER_BUTTON_ClearInterrupt(uint32_t source)     
{
    Cy_TCPWM_ClearInterrupt(POWER_BUTTON_HW, POWER_BUTTON_CNT_NUM, source);
}


/*******************************************************************************
* Function Name: POWER_BUTTON_SetInterrupt
****************************************************************************//**
*
* Invokes the Cy_TCPWM_SetInterrupt() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void POWER_BUTTON_SetInterrupt(uint32_t source)
{
    Cy_TCPWM_SetInterrupt(POWER_BUTTON_HW, POWER_BUTTON_CNT_NUM, source);
}


/*******************************************************************************
* Function Name: POWER_BUTTON_SetInterruptMask
****************************************************************************//**
*
* Invokes the Cy_TCPWM_SetInterruptMask() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void POWER_BUTTON_SetInterruptMask(uint32_t mask)     
{
    Cy_TCPWM_SetInterruptMask(POWER_BUTTON_HW, POWER_BUTTON_CNT_NUM, mask);
}


/*******************************************************************************
* Function Name: POWER_BUTTON_GetInterruptMask
****************************************************************************//**
*
* Invokes the Cy_TCPWM_GetInterruptMask() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t POWER_BUTTON_GetInterruptMask(void)         
{
    return(Cy_TCPWM_GetInterruptMask(POWER_BUTTON_HW, POWER_BUTTON_CNT_NUM));
}


/*******************************************************************************
* Function Name: POWER_BUTTON_GetInterruptStatusMasked
****************************************************************************//**
*
* Invokes the Cy_TCPWM_GetInterruptStatusMasked() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t POWER_BUTTON_GetInterruptStatusMasked(void)
{
    return(Cy_TCPWM_GetInterruptStatusMasked(POWER_BUTTON_HW, POWER_BUTTON_CNT_NUM));
}

#endif /* POWER_BUTTON_CY_TCPWM_PWM_PDL_H */


/* [] END OF FILE */
