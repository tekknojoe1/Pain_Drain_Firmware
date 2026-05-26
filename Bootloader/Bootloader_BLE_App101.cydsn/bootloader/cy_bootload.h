/*******************************************************************************
* \file cy_bootload.h
*
* Compatibility shim: routes all Cy_Bootload_* calls to the DFU SDK
* (cy_dfu.h + cy_dfu_bwc_macro.h).
*
* The DFU SDK backward-compatibility header maps every Cy_Bootload_* name to
* the equivalent Cy_DFU_* name, so existing code compiles unchanged.
*******************************************************************************/

#ifndef BOOTLOADER_CY_BOOTLOAD_H
#define BOOTLOADER_CY_BOOTLOAD_H

#include "cy_dfu.h"

#endif /* BOOTLOADER_CY_BOOTLOAD_H */

/* [] END OF FILE */
