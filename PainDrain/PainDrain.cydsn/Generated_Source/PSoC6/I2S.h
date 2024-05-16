/***************************************************************************//**
* \file     I2S.h
* \version  2.0
*
* \brief
*  This file provides constants, parameter values, and API definition for the
*  I2S Component.
*
********************************************************************************
* \copyright
* Copyright 2016-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined CY_I2S_I2S_H
#define CY_I2S_I2S_H

#include "cyfitter.h"
#include "i2s/cy_i2s.h"

#if defined(__cplusplus)
extern "C" {
#endif
    

/***************************************
*   Variables with external linkage
***************************************/

/**
* \addtogroup group_globals
* @{
*/
extern const cy_stc_i2s_config_t I2S_config;
/** @} globals */

extern uint8_t I2S_initVar;
extern cy_stc_i2s_context_t I2S_backup;


/***************************************
*        Function Prototypes
***************************************/

/**
* \addtogroup group_general
* @{
*/
                void     I2S_Start(void);
                void     I2S_Stop(void);
                
__STATIC_INLINE void     I2S_Init(void);
__STATIC_INLINE void     I2S_DeInit(void);

__STATIC_INLINE void     I2S_EnableTx(void);
__STATIC_INLINE void     I2S_PauseTx(void);
__STATIC_INLINE void     I2S_ResumeTx(void);
__STATIC_INLINE void     I2S_DisableTx(void);
__STATIC_INLINE void     I2S_EnableRx(void);
__STATIC_INLINE void     I2S_DisableRx(void);
__STATIC_INLINE uint32_t I2S_GetCurrentState(void);

__STATIC_INLINE void     I2S_ClearTxFifo(void);
__STATIC_INLINE uint32_t I2S_GetNumInTxFifo(void);
__STATIC_INLINE void     I2S_WriteTxData(uint32_t data);
__STATIC_INLINE uint8_t  I2S_GetTxReadPointer(void);
__STATIC_INLINE uint8_t  I2S_GetTxWritePointer(void);

__STATIC_INLINE void     I2S_ClearRxFifo(void);
__STATIC_INLINE uint32_t I2S_GetNumInRxFifo(void);
__STATIC_INLINE uint32_t I2S_ReadRxData(void);
__STATIC_INLINE uint32_t I2S_ReadRxDataSilent(void);
__STATIC_INLINE uint8_t  I2S_GetRxReadPointer(void);
__STATIC_INLINE uint8_t  I2S_GetRxWritePointer(void);

__STATIC_INLINE uint32_t I2S_GetInterruptStatus(void);
__STATIC_INLINE void     I2S_ClearInterrupt(uint32_t interrupt);
__STATIC_INLINE void     I2S_SetInterrupt(uint32_t interrupt);
__STATIC_INLINE uint32_t I2S_GetInterruptMask(void);
__STATIC_INLINE void     I2S_SetInterruptMask(uint32_t interrupt);
__STATIC_INLINE uint32_t I2S_GetInterruptStatusMasked(void);
/** @} general */


/***************************************
*           API Constants
***************************************/

/**
* \addtogroup group_constants
* @{
*/
/** The pointer to the base address of the HW I2S instance. */
#define I2S_HW             (I2S)
/** The Tx FIFO Write register pointer for DMA initialization. */
#define I2S_TX_FIFO_WR_PTR ((volatile uint32_t *) &I2S_HW->TX_FIFO_WR)
/** The Rx FIFO Read register pointer for DMA initialization. */
#define I2S_RX_FIFO_RD_PTR ((volatile const uint32_t *) &I2S_HW->RX_FIFO_RD)

/** \cond Internal definitions */
#define I2S_TX_ENABLED      (true)
#define I2S_RX_ENABLED      (false)

#define I2S_CMD             ((I2S_TX_ENABLED ? I2S_CMD_TX_START_Msk : 0UL) | \
                                          (I2S_RX_ENABLED ? I2S_CMD_RX_START_Msk : 0UL))

#if(I2S_TX_ENABLED)
    #define I2S_INT_MASK_TX ( \
        (_BOOL2FLD(I2S_INTR_MASK_TX_TRIGGER,   false)) | \
        (_BOOL2FLD(I2S_INTR_MASK_TX_NOT_FULL,  false)) | \
        (_BOOL2FLD(I2S_INTR_MASK_TX_EMPTY,     false)) | \
        (_BOOL2FLD(I2S_INTR_MASK_TX_OVERFLOW,  false)) | \
        (_BOOL2FLD(I2S_INTR_MASK_TX_UNDERFLOW, false)) | \
        (_BOOL2FLD(I2S_INTR_MASK_TX_WD,        false)))
#else /* I2S_TX_ENABLED */
    #define I2S_INT_MASK_TX (0UL)
#endif /* I2S_TX_ENABLED */
        
#if(I2S_RX_ENABLED)
    #define I2S_INT_MASK_RX ( \
        (_BOOL2FLD(I2S_INTR_MASK_RX_TRIGGER,   false)) | \
        (_BOOL2FLD(I2S_INTR_MASK_RX_NOT_EMPTY, false)) | \
        (_BOOL2FLD(I2S_INTR_MASK_RX_FULL,      false)) | \
        (_BOOL2FLD(I2S_INTR_MASK_RX_OVERFLOW,  false)) | \
        (_BOOL2FLD(I2S_INTR_MASK_RX_UNDERFLOW, false)) | \
        (_BOOL2FLD(I2S_INTR_MASK_RX_WD,        false)))
#else /* I2S_RX_ENABLED */
    #define I2S_INT_MASK_RX (0UL)
#endif /* I2S_RX_ENABLED */
/** \endcond */

/** The default (configured in GUI) interrupt mask. */
#define I2S_INT_MASK (I2S_INT_MASK_TX | I2S_INT_MASK_RX)

#if(I2S_TX_ENABLED)
    /** Tx FIFO Trigger Level. */
    #define I2S_TX_FIFO_TRG_LVL (0U)
    /** The number of Tx channels. */
    #define I2S_TX_CHANNELS     (2U)
#endif /* I2S_TX_ENABLED */

#if(I2S_RX_ENABLED)
    /** Rx FIFO Trigger Level. */
    #define I2S_RX_FIFO_TRG_LVL (0U)
    /** The number of Rx channels. */
    #define I2S_RX_CHANNELS     (2U)
#endif /* I2S_RX_ENABLED */

/** @} globals */


/***************************************
*    In-line Function Implementation
***************************************/

/*******************************************************************************
* Function Name: I2S_Init
****************************************************************************//**
* Invokes the Cy_I2S_Init() PDL driver function.
*******************************************************************************/
__STATIC_INLINE void I2S_Init(void)
{
    (void)Cy_I2S_Init(I2S_HW, &I2S_config);
}

/*******************************************************************************
* Function Name: I2S_DeInit
****************************************************************************//**
* Invokes the Cy_I2S_DeInit() PDL driver function.
*******************************************************************************/
__STATIC_INLINE void I2S_DeInit(void)
{
    I2S_initVar = 0U;
    Cy_I2S_DeInit(I2S_HW);
}


/*******************************************************************************
* Function Name: I2S_EnableTx
****************************************************************************//**
* Invokes the Cy_I2S_EnableTx() PDL driver function.
*******************************************************************************/
__STATIC_INLINE void I2S_EnableTx(void)
{
    Cy_I2S_EnableTx(I2S_HW);
}


/*******************************************************************************
* Function Name: I2S_PauseTx
****************************************************************************//**
* Invokes the Cy_I2S_PauseTx() PDL driver function.
*******************************************************************************/
__STATIC_INLINE void I2S_PauseTx(void)
{
    Cy_I2S_PauseTx(I2S_HW);
}


/*******************************************************************************
* Function Name: I2S_ResumeTx
****************************************************************************//**
* Invokes the Cy_I2S_ResumeTx() PDL driver function.
*******************************************************************************/
__STATIC_INLINE void I2S_ResumeTx(void)
{
    Cy_I2S_ResumeTx(I2S_HW);
}


/*******************************************************************************
* Function Name: I2S_DisableTx
****************************************************************************//**
* Invokes the Cy_I2S_DisableTx() PDL driver function.
* *****************************************************************************/
__STATIC_INLINE void I2S_DisableTx(void)
{
    Cy_I2S_DisableTx(I2S_HW);
}


/*******************************************************************************
* Function Name: I2S_GetCurrentState
****************************************************************************//**
* Invokes the Cy_I2S_GetCurrentState() PDL driver function.
*******************************************************************************/
__STATIC_INLINE uint32_t I2S_GetCurrentState(void)
{
    return (Cy_I2S_GetCurrentState(I2S_HW));
}



/*******************************************************************************
* Function Name: I2S_ClearTxFifo
****************************************************************************//**
* Invokes the Cy_I2S_ClearTxFifo() PDL driver function.
*******************************************************************************/
__STATIC_INLINE void I2S_ClearTxFifo(void)
{
    Cy_I2S_ClearTxFifo(I2S_HW);
}


/*******************************************************************************
* Function Name: I2S_GetNumInTxFifo
****************************************************************************//**
* Invokes the Cy_I2S_GetNumInTxFifo() PDL driver function.
*******************************************************************************/
__STATIC_INLINE uint32_t I2S_GetNumInTxFifo(void)
{
    return (Cy_I2S_GetNumInTxFifo(I2S_HW));
}


/*******************************************************************************
* Function Name: I2S_WriteTxData
****************************************************************************//**
* Invokes the Cy_I2S_WriteTxData() PDL driver function.
*******************************************************************************/
__STATIC_INLINE void I2S_WriteTxData(uint32_t data)
{
    Cy_I2S_WriteTxData(I2S_HW, data);
}


/*******************************************************************************
* Function Name: I2S_GetTxReadPointer
****************************************************************************//**
* Invokes the Cy_I2S_GetTxReadPointer() PDL driver function.
*******************************************************************************/
__STATIC_INLINE uint8_t I2S_GetTxReadPointer(void)
{
    return (Cy_I2S_GetTxReadPointer(I2S_HW));
}


/*******************************************************************************
* Function Name: I2S_GetTxWritePointer
****************************************************************************//**
* Invokes the Cy_I2S_GetTxWritePointer() PDL driver function.
*******************************************************************************/
__STATIC_INLINE uint8_t I2S_GetTxWritePointer(void)
{
    return (Cy_I2S_GetTxWritePointer(I2S_HW));
}


/*******************************************************************************
* Function Name: I2S_EnableRx
****************************************************************************//**
* Invokes the Cy_I2S_EnableRx() PDL driver function.
*******************************************************************************/
__STATIC_INLINE void I2S_EnableRx(void)
{
    Cy_I2S_EnableRx(I2S_HW);
}


/*******************************************************************************
* Function Name: I2S_DisableRx
****************************************************************************//**
* Invokes the Cy_I2S_DisableRx() PDL driver function.
*******************************************************************************/
__STATIC_INLINE void I2S_DisableRx(void)
{
    Cy_I2S_DisableRx(I2S_HW);
}


/*******************************************************************************
* Function Name: I2S_ClearRxFifo
****************************************************************************//**
* Invokes the Cy_I2S_ClearRxFifo() PDL driver function.
*******************************************************************************/
__STATIC_INLINE void I2S_ClearRxFifo(void)
{
    Cy_I2S_ClearRxFifo(I2S_HW);
}


/*******************************************************************************
* Function Name: I2S_GetNumInRxFifo
****************************************************************************//**
* Invokes the Cy_I2S_GetNumInRxFifo() PDL driver function.
*******************************************************************************/
__STATIC_INLINE uint32_t I2S_GetNumInRxFifo(void)
{
    return (Cy_I2S_GetNumInRxFifo(I2S_HW));
}

 
/*******************************************************************************
* Function Name: I2S_ReadRxData
****************************************************************************//**
* Invokes the Cy_I2S_ReadRxData() PDL driver function.
*******************************************************************************/
__STATIC_INLINE uint32_t I2S_ReadRxData(void)
{
    return (Cy_I2S_ReadRxData(I2S_HW));
}


/*******************************************************************************
* Function Name: I2S_ReadRxDataSilent
****************************************************************************//**
* Invokes the Cy_I2S_ReadRxDataSilent() PDL driver function.
*******************************************************************************/
__STATIC_INLINE uint32_t I2S_ReadRxDataSilent(void)
{
    return (Cy_I2S_ReadRxDataSilent(I2S_HW));
}


/*******************************************************************************
* Function Name: I2S_GetRxReadPointer
****************************************************************************//**
* Invokes the Cy_I2S_GetRxReadPointer() PDL driver function.
*******************************************************************************/
__STATIC_INLINE uint8_t I2S_GetRxReadPointer(void)
{
    return (Cy_I2S_GetRxReadPointer(I2S_HW));
}

 
/*******************************************************************************
* Function Name: I2S_GetRxWritePointer
****************************************************************************//**
* Invokes the Cy_I2S_GetRxWritePointer() PDL driver function.
*******************************************************************************/
__STATIC_INLINE uint8_t I2S_GetRxWritePointer(void)
{
    return (Cy_I2S_GetRxWritePointer(I2S_HW));
}


/*******************************************************************************
* Function Name: I2S_GetInterruptStatus
****************************************************************************//**
* Invokes the Cy_I2S_GetInterruptStatus() PDL driver function.
*******************************************************************************/
__STATIC_INLINE uint32_t I2S_GetInterruptStatus(void)
{
    return (Cy_I2S_GetInterruptStatus(I2S_HW));
}


/*******************************************************************************
* Function Name: I2S_ClearInterrupt
****************************************************************************//**
* Invokes the Cy_I2S_ClearInterrupt() PDL driver function.
*******************************************************************************/
__STATIC_INLINE void I2S_ClearInterrupt(uint32_t interrupt)
{
    Cy_I2S_ClearInterrupt(I2S_HW, interrupt);
}


/*******************************************************************************
* Function Name: I2S_SetInterrupt
****************************************************************************//**
* Invokes the Cy_I2S_SetInterrupt() PDL driver function.
*******************************************************************************/
__STATIC_INLINE void I2S_SetInterrupt(uint32_t interrupt)
{
    Cy_I2S_SetInterrupt(I2S_HW, interrupt);
}


/*******************************************************************************
* Function Name: I2S_GetInterruptMask
****************************************************************************//**
* Invokes the Cy_I2S_GetInterruptMask() PDL driver function.
*******************************************************************************/
__STATIC_INLINE uint32_t I2S_GetInterruptMask(void)
{
    return (Cy_I2S_GetInterruptMask(I2S_HW));
}


/*******************************************************************************
* Function Name: I2S_SetInterruptMask
****************************************************************************//**
* Invokes the Cy_I2S_SetInterruptMask() PDL driver function.
*******************************************************************************/
__STATIC_INLINE void I2S_SetInterruptMask(uint32_t interrupt)
{
    Cy_I2S_SetInterruptMask(I2S_HW, interrupt);
}


/*******************************************************************************
* Function Name: I2S_GetInterruptStatusMasked
****************************************************************************//**
* Invokes the Cy_I2S_GetInterruptStatusMasked() PDL driver function.
*******************************************************************************/
__STATIC_INLINE uint32_t I2S_GetInterruptStatusMasked(void)
{
    return (Cy_I2S_GetInterruptStatusMasked(I2S_HW));
}


#if defined(__cplusplus)
}
#endif

#endif /* CY_I2S_I2S_H */


/* [] END OF FILE */
