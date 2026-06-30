/*******************************************************************************
* File Name: main_cm0p.c
*
* Version: 1.20
*
* Description: This file provides the source code for the Bootloadable (App1)
*              BLE HID Keyboard running on the Core M0+ (Core0).
*              App1 Core0 firmware does the following:
*              - Starts App1 Core1 firmware.
*              - Hosts the complete BLE component.
*
* Related Document: Code example CE216767.pdf
*                   Code example CE215121.pdf
*
* Hardware Dependency: CY8CKIT-062-BLE PSoC 6 BLE Pioneer Kit
*
******************************************************************************
* Copyright (2017), Cypress Semiconductor Corporation.
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

/*******************************************************************************
* Function Name: main()
********************************************************************************
*
* Summary:
*  Main function for the project.
*
*
*******************************************************************************/
int main(void)
{    

    /* Unfreeze IO after Hibernate */
    if(Cy_SysPm_GetIoFreezeStatus())
    {
        Cy_SysPm_IoUnfreeze();
    }

    /* Minimal chooser bootloader: select an app slot and jump to it
     * (HostMain never returns). CM4 is intentionally NOT enabled here --
     * the selected app's CM0+ enables CM4 at the app's own CM4 vector
     * table address (APP_CM4_ADDR in the app's main_cm0p.c). */
    HostMain();

    for(;;)
    {
    }
}


/* [] END OF FILE */
