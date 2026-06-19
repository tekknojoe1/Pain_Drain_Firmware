/*******************************************************************************
* File Name: host_main.c
*
* Description: Minimal bootloader for dual-app OTA.
*
*******************************************************************************/

#include "project.h"
#include "common.h"
#include <stdio.h>

static const uint32_t BUILD_NUMBER = 2;

/* Firmware metadata structure at start of each app slot */
typedef struct {
    uint32_t magic;        /* 0xDEADBEEF = valid, 0x00000000 = empty */
    uint32_t version;      /* Firmware version */
    uint32_t size;         /* Firmware size in bytes */
    uint32_t crc;          /* CRC32 of firmware (not implemented yet) */
} fw_metadata_t;

/* Dual-app bootloader entry point */
int HostMain(void)
{
    /* Initialize UART debug interface */
    UART_DEB_Start();

    fw_metadata_t *app0_meta = (fw_metadata_t *)APP0_META_ADDR;
    fw_metadata_t *app1_meta = (fw_metadata_t *)APP1_META_ADDR;
    uint32_t boot_addr = APP0_START_ADDR;

    /* Bootloader output disabled */
    /* printf("\r\n=== BOOTLOADER BUILD %lu ===\r\n", (unsigned long)BUILD_NUMBER); */

    /* Determine which app to boot based on validity and version */
    if (app0_meta->magic == 0xDEADBEEF && app1_meta->magic == 0xDEADBEEF)
    {
        if (app1_meta->version > app0_meta->version) {
            boot_addr = APP1_START_ADDR;
            /* printf("Boot APP1\r\n"); */
        } else {
            boot_addr = APP0_START_ADDR;
            /* printf("Boot APP0\r\n"); */
        }
    }
    else if (app1_meta->magic == 0xDEADBEEF)
    {
        boot_addr = APP1_START_ADDR;
        /* printf("Boot APP1 only\r\n"); */
    }
    else
    {
        boot_addr = APP0_START_ADDR;
        /* printf("Boot APP0 default\r\n"); */
    }

    /* Set up app's vector table and stack pointer */
    uint32_t app_stack = *(uint32_t *)boot_addr;              /* MSP from vector table */
    uint32_t app_entry = *(uint32_t *)(boot_addr + 4);       /* Reset handler */

    __set_MSP(app_stack);                                      /* Set stack pointer */
    ((void (*)(void))app_entry)();                             /* Jump to reset handler */

    return 0;
}

/* [] END OF FILE */
