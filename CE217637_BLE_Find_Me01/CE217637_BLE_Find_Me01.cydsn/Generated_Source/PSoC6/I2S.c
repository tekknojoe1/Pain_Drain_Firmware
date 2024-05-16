/***************************************************************************//**
* \file     I2S.c
* \version  2.0
*
* \brief
*  This file provides the source code to the API for the
*  I2S component.
*
********************************************************************************
* \copyright
* Copyright 2016-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "I2S.h"

#if defined(__cplusplus)
extern "C" {
#endif

/***************************************
*     Global variables
***************************************/

/** An instance-specific configuration structure.
*   It should be used in the associated Cy_I2S_Init() function.
*/
const cy_stc_i2s_config_t I2S_config =
{
    .txEnabled          = true,
    .rxEnabled          = false,
    .txDmaTrigger       = false,
    .rxDmaTrigger       = false,
    .clkDiv             = 16U,
    .extClk             = false,
    .txMasterMode       = true,
    .txAlignment        = CY_I2S_I2S_MODE,
    .txWsPulseWidth     = CY_I2S_WS_ONE_CHANNEL_LENGTH,
    .txWatchdogEnable   = false,
    .txWatchdogValue    = 0xFFFFFFFFUL,
    .txSdoLatchingTime  = false,
    .txSckoInversion    = false,
    .txSckiInversion    = false,
    .txChannels         = 2U,
    .txChannelLength    = CY_I2S_LEN16,
    .txWordLength       = CY_I2S_LEN16,
    .txOverheadValue    = CY_I2S_OVHDATA_ZERO,
    .txFifoTriggerLevel = 0U,
    .rxMasterMode       = true,
    .rxAlignment        = CY_I2S_I2S_MODE,
    .rxWsPulseWidth     = CY_I2S_WS_ONE_CHANNEL_LENGTH,
    .rxWatchdogEnable   = false,
    .rxWatchdogValue    = 0xFFFFFFFFUL,
    .rxSdiLatchingTime  = false,
    .rxSckoInversion    = false,
    .rxSckiInversion    = false,
    .rxChannels         = 2U,
    .rxChannelLength    = CY_I2S_LEN16,
    .rxWordLength       = CY_I2S_LEN16,
    .rxSignExtension    = false,
    .rxFifoTriggerLevel = 0U
};

/* An internal variable indicates whether the I2S has been
*  initialized. The variable is initialized to 0 and set to 1 the first time
*  I2S_Start() is called. This allows the Component to
*  restart without reinitialization after the first call to the
*  I2S_Start() routine. The variable is cleared by the
*  I2S_DeInit() routine call.
*/
uint8_t I2S_initVar = 0U;

/* A structure to save the current TX/RX enable/disable Component state and
*  interrupt settings (the Component settings that can be changed on the fly
*  by the component API) during a stop/start sequence, and also can be used as
*  a SysPm callback context.
*/
cy_stc_i2s_context_t I2S_backup;


/*******************************************************************************
*     Functions
*******************************************************************************/


/*******************************************************************************
* Function Name: I2S_Start
****************************************************************************//**
*
* Calls the Cy_I2S_Init() function when called the first time or after the
* \ref I2S_DeInit() call, then clears FIFOs, fills the first TX 
* FIFO frame with zeros, enables the TX and/or RX (accordingly to the
* configuration) and enables interrupts. For subsequent calls (after the
* \ref I2S_Stop() call) the configuration is left unchanged, the
* FIFOs are cleared (with the first TX FIFO frame filled with zeros), TX and/or
* RX operation and interrupts are enabled accordingly to the state before
* previous \ref I2S_Stop() calling.
*
*******************************************************************************/
void I2S_Start(void)
{
#if(I2S_TX_ENABLED)
    /* The local Tx channel counter to set the first data transmission */
    uint32_t locChanCnt;
#endif /* I2S_TX_ENABLED */
    
    if(0U == I2S_initVar)
    {
        I2S_initVar = 1U;
        I2S_backup.enableState = I2S_CMD;
        I2S_backup.interruptMask = I2S_INT_MASK;

        (void)Cy_I2S_Init(I2S_HW, &I2S_config);
    }

#if(I2S_TX_ENABLED)
    if(0UL != (I2S_backup.enableState & I2S_CMD_TX_START_Msk))
    {
        Cy_I2S_ClearTxFifo(I2S_HW);
        
        /* The first transmission data setting. Write at least one frame to the FIFO */
        for(locChanCnt = 0UL; locChanCnt < I2S_config.txChannels; locChanCnt++)
        {
            Cy_I2S_WriteTxData(I2S_HW, 0UL);
        }
        
        Cy_I2S_EnableTx(I2S_HW);
    }
#endif /* I2S_TX_ENABLED */

#if(I2S_RX_ENABLED)
    if(0UL != (I2S_backup.enableState & I2S_CMD_RX_START_Msk))
    {
        Cy_I2S_ClearRxFifo(I2S_HW);
        
        Cy_I2S_EnableRx(I2S_HW);
    }
#endif /* I2S_TX_ENABLED */
    
    /* Clear possible pending interrupts */
    Cy_I2S_ClearInterrupt(I2S_HW, I2S_backup.interruptMask);

    /* Enable interrupts */
    Cy_I2S_SetInterruptMask(I2S_HW, I2S_backup.interruptMask);
}


/*******************************************************************************
* Function Name: I2S_Stop
****************************************************************************//**
*
* Saves the current TX/RX/interrupts state (which can be changed in runtime by
* the PDL API) and then disables TX and/or RX operation and interrupts.
*
*******************************************************************************/
void I2S_Stop(void)
{
    if(0UL != ((I2S_CMD_TX_START_Msk | I2S_CMD_RX_START_Msk) & Cy_I2S_GetCurrentState(I2S_HW)))
    {
        /* Store the current component state */
        I2S_backup.enableState = Cy_I2S_GetCurrentState(I2S_HW);
        I2S_backup.interruptMask = Cy_I2S_GetInterruptMask(I2S_HW);
    
    #if(I2S_TX_ENABLED)
        if(0UL != (I2S_backup.enableState & I2S_CMD_TX_START_Msk))
        {
            Cy_I2S_DisableTx(I2S_HW);
        }
    #endif /* I2S_TX_ENABLED */

    #if(I2S_RX_ENABLED)
        if(0UL != (I2S_backup.enableState & I2S_CMD_RX_START_Msk))
        {
            Cy_I2S_DisableRx(I2S_HW);
        }
    #endif /* I2S_RX_ENABLED */
    
        /* Disable interrupts */
        Cy_I2S_SetInterruptMask(I2S_HW, 0UL);
    }
}
    

#if defined(__cplusplus)
}
#endif

/* [] END OF FILE */
