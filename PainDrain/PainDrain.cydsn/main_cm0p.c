/*******************************************************************************
* File Name: main_cm0p.c
*
* Version: 1.10
*
* Description:
*  This is the source code CM0p core for the BLE project (for dual cores MPN).
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

#include "project.h"
#include "common.h"
#include <stdint.h>
#include "version.h"

/* Application metadata block — lives at 0x10040000, the very start of App1's flash.
 * The linker pads this section to 256 bytes so the vector table that follows remains
 * on a 256-byte boundary as required by the CM0+ VTOR register.
 * Bootloader reads this at 0x10040000 to confirm App1 is present before jumping. */
#define APP1_META_MAGIC   0xA1B2C3D4u

typedef struct {
    uint32_t magic;
    uint32_t version;
} app1_meta_t;

__attribute__((section(".cy_app_header"), used))
static const app1_meta_t app1_meta = {
    .magic   = APP1_META_MAGIC,
    .version = APP1_VERSION_PACKED,
};


/*
 * TOC2 at supervisory flash 0x16007C00.
 * Tells the silicon ROM where to find the CM0+ application.
 * Structure: 508 bytes of fields (covered by CRC16-CCITT-FALSE) + 4-byte CRC.
 * CRC16 (poly=0x1021, init=0xFFFF, MSB-first) of the first 508 bytes = 0xE8BF.
 * Stored as upper 16 bits of the CRC field → 0xE8BF0000.
 */
typedef struct {
    uint32_t objSize;        /* 0x00: size of object in bytes (excl. CRC) */
    uint32_t magicNum;       /* 0x04: 0x01211220 */
    uint32_t keyBlockAddr;   /* 0x08: user key address (0xFFFFFFFF = none) */
    uint32_t smifCfgAddr;    /* 0x0C: SMIF config pointer (0xFFFFFFFF = none) */
    uint32_t appAddr1;       /* 0x10: CM0+ app start address */
    uint32_t appFormat1;     /* 0x14: 0 = Basic (jump directly to address) */
    uint32_t appAddr2;       /* 0x18: second app address (0xFFFFFFFF = unused) */
    uint32_t appFormat2;     /* 0x1C: second app format */
    uint32_t shashObj;       /* 0x20: S-HASH object count (0 = none) */
    uint32_t sigKeyAddr;     /* 0x24: signature key address (0xFFFFFFFF = none) */
    uint32_t addObj[116];    /* 0x28..0x1F7: additional objects (all zero) */
    uint32_t tocFlags;       /* 0x1F8: 0 = no validation, 25MHz, 20ms listen */
    uint32_t crc;            /* 0x1FC: CRC16-CCITT in upper 16 bits, lower 16 = 0 */
} cy_toc2_t;

__attribute__((section(".cy_toc_part2"), used))
static const cy_toc2_t cy_toc2 = {
    .objSize     = 0x000001FCu,   /* 508 bytes */
    .magicNum    = 0x01211220u,
    .keyBlockAddr = 0xFFFFFFFFu,
    .smifCfgAddr  = 0xFFFFFFFFu,
    .appAddr1    = 0x10000000u,   /* App0 bootloader CM0+ origin */
    .appFormat1  = 0x00000000u,   /* Basic */
    .appAddr2    = 0xFFFFFFFFu,
    .appFormat2  = 0x00000000u,
    .shashObj    = 0x00000000u,
    .sigKeyAddr  = 0xFFFFFFFFu,
    .addObj      = {0},           /* 116 zero entries */
    .tocFlags    = 0x00000000u,   /* no app validation, default clock/listen */
    .crc         = 0xF1AD0000u,   /* CRC16=0xF1AD in upper 16 bits */
};

/* Redundant TOC2 at 0x16007E00 — identical content, separate section */
__attribute__((section(".cy_rtoc_part2"), used))
static const cy_toc2_t cy_rtoc2 = {
    .objSize     = 0x000001FCu,
    .magicNum    = 0x01211220u,
    .keyBlockAddr = 0xFFFFFFFFu,
    .smifCfgAddr  = 0xFFFFFFFFu,
    .appAddr1    = 0x10040000u,
    .appFormat1  = 0x00000000u,
    .appAddr2    = 0xFFFFFFFFu,
    .appFormat2  = 0x00000000u,
    .shashObj    = 0x00000000u,
    .sigKeyAddr  = 0xFFFFFFFFu,
    .addObj      = {0},
    .tocFlags    = 0x00000000u,
    .crc         = 0xE8BF0000u,
};

/*******************************************************************************
* Function Name: main()
********************************************************************************
*
* Summary:
*  Main function for the project.
*
*******************************************************************************/
int main(void)
{

    /* Enable global interrupts */
    __enable_irq();

#if(CY_BLE_CONFIG_HOST_CORE == CY_BLE_CORE_CORTEX_M0P)

    /* Enable CM4.  CY_CORTEX_M4_APPL_ADDR must be updated if CM4 memory layout is changed. */
    Cy_SysEnableCM4(CY_CORTEX_M4_APPL_ADDR);

    /* Run Host main */
    HostMain();

#else
    
    #if(CY_BLE_STACK_MODE_IPC)
        /* App1's ClockInit() switches LFCLK from WCO (running from ROM/bootloader)
         * to PILO while the ROM BLE listen-window shutdown is still using that LFCLK
         * for timing (Cypress Bug #307068).  This LFCLK glitch can leave BLESS in a
         * permanently stuck state that a simple ENABLE_BLERD clear cannot undo.
         * Fix: issue a BLESS hardware soft reset first (clears all internal state),
         * then fall through to the standard BLESS-settle + BLE_Start sequence. */
        {
            cy_en_ble_api_result_t r = (cy_en_ble_api_result_t)0xFFu;

            /* BLESS soft reset: equivalent to Cy_BLE_EcoStop().  Puts BLESS back to
             * power-on state regardless of whatever corrupted state it is in. */
            BLE->BLESS.LL_CLK_EN |= BLE_BLESS_LL_CLK_EN_BLESS_RESET_Msk;
            Cy_SysLib_DelayUs(500u);   /* wait for reset pulse to complete */

            for (uint8_t attempt = 0u; attempt < 5u; attempt++) {
                /* Unconditionally disable BLESS radio domain and clear interrupt mask */
                BLE->BLESS.MT_CFG   &= ~BLE_BLESS_MT_CFG_ENABLE_BLERD_Msk;
                BLE->BLESS.INTR_MASK  = 0u;

                /* Wait up to 200ms for BLESS to reach deep-sleep (BLESS_STATE == 0) */
                {
                    uint32_t t = 200000u;
                    while (t-- > 0u) {
                        if ((BLE->BLESS.MT_STATUS & BLE_BLESS_MT_STATUS_BLESS_STATE_Msk) == 0u) {
                            break;
                        }
                        Cy_SysLib_DelayUs(1u);
                    }
                }

                r = Cy_BLE_Start(NULL);
                if (r == CY_BLE_SUCCESS) {
                    break;
                }
                /* Short delay before retry */
                Cy_SysLib_Delay(200u);
            }
            /* Write result to shared ram_common+4 so CM4 can print it.
             * 0xBBEE0000 = success; other = failure error code OR'd in. */
            __DSB();
            *(volatile uint32_t *)0x08000004u = 0xBBEE0000u | (uint32_t)r;
        }
    #endif /* (CY_BLE_STACK_MODE_IPC)*/
    
    /* Enable CM4.  CY_CORTEX_M4_APPL_ADDR must be updated if CM4 memory layout is changed. */
    Cy_SysEnableCM4(CY_CORTEX_M4_APPL_ADDR); 
    
    for(;;)
    {
       
    #if(CY_BLE_STACK_MODE_IPC)
        /* Process BLE events continuously for controller in dual core mode */
        Cy_BLE_ProcessEvents();
        
        /* To achieve low power */
        Cy_SysPm_DeepSleep(CY_SYSPM_WAIT_FOR_INTERRUPT); 
    #endif /* CY_BLE_STACK_MODE_IPC */
    }
    
#endif /* (CY_BLE_CONFIG_HOST_CORE == CY_BLE_CORE_CORTEX_M0P) */  
}


/* [] END OF FILE */
