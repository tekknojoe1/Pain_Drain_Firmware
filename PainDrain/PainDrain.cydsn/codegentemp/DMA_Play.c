/***************************************************************************//**
* \file DMA_Play.c
* \version 2.0
*
*  This file provides the source code to the API for the
*  DMA_Play component.
*
********************************************************************************
* \copyright
* Copyright 2016-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "DMA_Play.h"

#if defined(__cplusplus)
extern "C" {
#endif

/* Generated code */
const cy_stc_dma_descriptor_config_t DMA_Play_SRAM_to_I2S_config =
{
    .retrigger       = CY_DMA_RETRIG_16CYC,
    .interruptType   = CY_DMA_DESCR_CHAIN,
    .triggerOutType  = CY_DMA_1ELEMENT,
    .channelState    = CY_DMA_CHANNEL_DISABLED,
    .triggerInType   = CY_DMA_1ELEMENT,
    .dataSize        = CY_DMA_HALFWORD,
    .srcTransferSize = CY_DMA_TRANSFER_SIZE_DATA,
    .dstTransferSize = CY_DMA_TRANSFER_SIZE_WORD,
    .descriptorType  = CY_DMA_2D_TRANSFER,
    .srcAddress      = NULL,
    .dstAddress      = NULL,
    .srcXincrement   = 1L,
    .dstXincrement   = 0L,
    .xCount          = 256UL,
    .srcYincrement   = 256L,
    .dstYincrement   = 0L,
    .yCount          = 256UL,
    .nextDescriptor  = &DMA_Play_SRAM_to_I2S
};

cy_stc_dma_descriptor_t DMA_Play_SRAM_to_I2S =
{
    .ctl = 0UL,
    .src = 0UL,
    .dst = 0UL,
    .xCtl = 0UL,
    .yCtl = 0UL,
    .nextPtr = 0UL
};


/** DMA_Play_initVar indicates whether the DMA_Play 
*  component has been initialized. The variable is initialized to 0 
*  and set to 1 the first time DMA_Play_Start() is called. This allows 
*  the component to restart without reinitialization after the first 
*  call to the DMA_Play_Start() routine.
*
*  If re-initialization of the component is required, then the 
*  DMA_Play_Init() function can be called before the 
*  DMA_Play_Start() or DMA_Play_ChEnable() function.
*/
uint8 DMA_Play_initVar = 0u;


/*******************************************************************************
* Function Name: DMA_Play_Start
****************************************************************************//**
*
* Based on the settings for descriptor in the customizer this function runs the
* DMA_Descriptor_Init() and then initializes the channel using
* DMA_Chnl_Init(). Enables the DMA_Play block using the DMA_Chnl_Enable().
*  
*******************************************************************************/
void DMA_Play_Start(void const * srcAddress, void const * dstAddress)
{
    if (0U == DMA_Play_initVar)
    {
        DMA_Play_Init();
        DMA_Play_initVar = 1u;
    }
    
    Cy_DMA_Descriptor_SetSrcAddress(&DMA_Play_SRAM_to_I2S, srcAddress);
    Cy_DMA_Descriptor_SetDstAddress(&DMA_Play_SRAM_to_I2S, dstAddress);
    Cy_DMA_Channel_Enable(DMA_Play_HW, DMA_Play_DW_CHANNEL);
}


/*******************************************************************************
* Function Name: DMA_Play_Init
****************************************************************************//**
*
* Based on the settings for the descriptor in the customizer this function runs the
* DMA_Descriptor_Init() and then initializes the channel using
* DMA_Chnl_Init().
*  
*******************************************************************************/
void DMA_Play_Init(void)
{
    cy_stc_dma_channel_config_t channelConfig;

    /* Init all descriptors */
    (void)Cy_DMA_Descriptor_Init(&DMA_Play_SRAM_to_I2S, &DMA_Play_SRAM_to_I2S_config);


    channelConfig.descriptor  = &DMA_Play_SRAM_to_I2S;
    channelConfig.preemptable = DMA_Play_PREEMPTABLE;
    channelConfig.priority    = DMA_Play_PRIORITY;
    channelConfig.enable      = false;
    channelConfig.bufferable  = DMA_Play_BUFFERABLE;

    (void)Cy_DMA_Channel_Init(DMA_Play_HW, DMA_Play_DW_CHANNEL, &channelConfig);

    Cy_DMA_Enable(DMA_Play_HW);
}


#if defined(__cplusplus)
}
#endif

/* [] END OF FILE */
