/*******************************************************************************
* File Name: main_cm4.c
*
* Version 1.10
*
* Description:
*  This is the source code CM4 core for the BLE project (for dual cores MPN).
*
* Hardware Dependency:
*  CY8CKIT-062 PSoC6 BLE Pioneer Kit
*  CY8CPROTO-063-BLE PSoC 6 BLE Prototyping Kit
*
******************************************************************************
* Copyright (2018), Cypress Semiconductor Corporation.
******************************************************************************
* This software is owned by Cypress Semiconductor Corporation (Cypress) and is
* protected by and subject to worldwide patent protection (United States and
* foreign), United States copyright laws and international treaty provisions.
* Cypress hereby grants to licensee a personal, non-exclusive, non-transferable
* license to copy, use, modify, create derivative works of, and compile the
* Cypress Source Code and derivative works for the sole purpose of creating
* custom software in support of licensee product to be used only in conjunction
* with a Cypress integrated circuit as specified in the applicable agreement.
* Any reproduction, modification, translation, compilation, or representation of
* this software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH
* REGARD TO THIS MATERIAL, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* Cypress reserves the right to make changes without further notice to the
* materials described herein. Cypress does not assume any liability arising out
* of the application or use of any product or circuit described herein. Cypress
* does not authorize its products for use as critical components in life-support
* systems where a malfunction or failure may reasonably be expected to result in
* significant injury to the user. The inclusion of Cypress' product in a life-
* support systems application implies that the manufacturer assumes all risk of
* such use and in doing so indemnifies Cypress against all charges. Use may be
* limited by and subject to the applicable Cypress software license agreement.
*****************************************************************************/

#include "common.h"
#include "user_interface.h"
#include "power.h"

/* WDT demo options */
#define WDT_RESET_DEMO						1
#define WDT_INTERRUPT_DEMO 					2
#define WDT_DEMO 							WDT_INTERRUPT_DEMO

/* WDT Interrupt Number */
#define WDT_IRQ__INTC_NUMBER 				srss_interrupt_IRQn
#define WDT_IRQ__INTC_CORTEXM4_PRIORITY 	7

/* WDT periodic interrupt timing */
#define WDT_INTERRUPT_INTERVAL				10  //ms

/* Match count =  Desired interrupt interval in seconds x ILO Frequency in Hz */
#define WDT_MATCH_COUNT 					(WDT_INTERRUPT_INTERVAL*32000/1000)

/* WDT interrupt configuration structure */
const cy_stc_sysint_t WDT_IRQ_cfg = {
	.intrSrc = (IRQn_Type)WDT_IRQ__INTC_NUMBER,
	.intrPriority = WDT_IRQ__INTC_CORTEXM4_PRIORITY
};

/* Function prototypes */
void InitializeWDT(void);
void WdtInterruptHandler(void);


/*******************************************************************************
* Function Name: main()
********************************************************************************
*
* Summary:
*  Main function for the project.
*
*******************************************************************************/
int main()
{
    
    /* Initialize WDT */
	InitializeWDT();
    
    /* Unfreeze IO after Hibernate */
    if(Cy_SysPm_IoIsFrozen())
    {
        Cy_SysPm_IoUnfreeze();
    }
    
    /* Enable global interrupts */
    __enable_irq(); 

#if(CY_BLE_CONFIG_HOST_CORE == CY_BLE_CORE_CORTEX_M4)   
    
    /* Run Host main */
    HostMain();

#else
    for(;;)
    {
        /* To achieve low power */
        Cy_SysPm_DeepSleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
    }
#endif /* (CY_BLE_CONFIG_HOST_CORE == CY_BLE_CORE_CORTEX_M4) */
}

/******************************************************************************
* Function Name: InitializeWDT
*******************************************************************************
*
* Summary: This function initializes the WDT block
*
* Parameters:
*  None
*
* Return:
*  None
*
******************************************************************************/
void InitializeWDT()
{
	 /* Step 1- Unlock WDT */
	Cy_WDT_Unlock();

	/* Step 2- Write the ignore bits - operate with full 16 bits */
	Cy_WDT_SetIgnoreBits(0);

	/* Step 3- Write match value */
	#if(WDT_DEMO == WDT_INTERRUPT_DEMO)
		Cy_WDT_SetMatch(WDT_MATCH_COUNT);
	#else
		Cy_WDT_SetMatch(0);
	#endif

	/* Step 4- Clear match event interrupt, if any */
	Cy_WDT_ClearInterrupt();

	/* Step 5- Enable ILO */
	Cy_SysClk_IloEnable();

	/* Step 6 - Enable interrupt if periodic interrupt mode selected */
	#if(WDT_DEMO == WDT_INTERRUPT_DEMO)
		Cy_SysInt_Init(&WDT_IRQ_cfg, WdtInterruptHandler);
		NVIC_EnableIRQ(WDT_IRQ_cfg.intrSrc);
		Cy_WDT_UnmaskInterrupt();
	#endif

	/* Step 7- Enable WDT */
	Cy_WDT_Enable();

	/* Step 8- Lock WDT configuration */
	Cy_WDT_Lock();
}

/******************************************************************************
* Function Name: WdtInterruptHandler
*******************************************************************************
*
* Summary: This function is the handler for the WDT interrupt
*
* Parameters:
*  None
*
* Return:
*  None
*
******************************************************************************/
void WdtInterruptHandler(void)
{
    
    
    
	/* Check if the interrupt is from WDT */
	if(SRSS->SRSS_INTR & SRSS_SRSS_INTR_WDT_MATCH_Msk)
	{
		/* Clear WDT Interrupt */
		Cy_WDT_ClearInterrupt();

		/* Update the match count */
		Cy_WDT_Unlock();

		/* Update the match count. Note that, if the ignore bits in the match is set
		 * to a non-zero value, the below code requires a modification. */
		Cy_WDT_SetMatch((uint16)(Cy_WDT_GetMatch() + WDT_MATCH_COUNT));
		Cy_WDT_Lock();

		/* Invert the state of LED */
		//Cy_GPIO_Inv(BLUE_LED_WDT_INT_0_PORT, BLUE_LED_WDT_INT_0_NUM);
        
        power_timer();
        //ui_timer();
	}
}




/* [] END OF FILE */
