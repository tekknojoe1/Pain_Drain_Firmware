/*******************************************************************************
* File Name: debug.h
*
* Version: 1.50
*
* Description:
*  Provides debug API.
*
* Hardware Dependency:
*  CY8CKIT-042 BLE
*
********************************************************************************
* Copyright 2014-2016, Cypress Semiconductor Corporation. All rights reserved.
* This software is owned by Cypress Semiconductor Corporation and is protected
* by and subject to worldwide patent and copyright laws and treaties.
* Therefore, you may use this software only as provided in the license agreement
* accompanying the software package from which you obtained this software.
* CYPRESS AND ITS SUPPLIERS MAKE NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* WITH REGARD TO THIS SOFTWARE, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT,
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*******************************************************************************/

#ifndef BLE_OTA_EM_DEBUG_H_
#define BLE_OTA_EM_DEBUG_H_

//#include <stdio.h>
#include "myprintf.h"

#include "common.h"

#if (DEBUG_UART_ENABLED == YES)
    #define DBG_PRINT_TEXT(a)        do\
                                        {\
                                            myprintf((a));\
                                        } while (0)

    #define DBG_PRINT_DEC(a)         do\
                                        {\
                                           myprintf("%02d ", a);\
                                        } while (0)


    #define DBG_PRINT_HEX(a)         do\
                                        {\
                                           myprintf("%08X ", a);\
                                        } while (0)


    #define DBG_PRINT_ARRAY(a,b)     do\
                                        {\
                                            uint32 i;\
                                            \
                                            for(i = 0u; i < (b); i++)\
                                            {\
                                                myprintf("%02X ", *(a+i));\
                                            }\
                                        } while (0)

    #define DBG_PRINTF(...)          (myprintf(__VA_ARGS__))
    
    #define DBG_NEW_PAGE             iputc(0x0C)
    
    
    
#else
    #define DBG_PRINT_TEXT(a)
    #define DBG_PRINT_DEC(a)
    #define DBG_PRINT_HEX(a)
    #define DBG_PRINT_ARRAY(a,b)
    #define DBG_PRINTF(...)
    #define DBG_NEW_PAGE             
#endif /* (DEBUG_UART_ENABLED == YES) */

#endif /* BLE_OTA_EM_DEBUG_H_ */

/* [] END OF FILE */
