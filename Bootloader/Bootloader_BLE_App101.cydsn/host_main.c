/*******************************************************************************
* File Name: host_main.c
*
* Description: Minimal dual-app bootloader (CM0+ only, no BLE).
*   On every reset it inspects the metadata of both app slots and jumps into
*   the valid slot with the highest version (App0 by default). The selected
*   app's CM0+ is responsible for enabling CM4.
*
*   Debug output is a self-contained bit-bang UART on P0.5 @ 9600 8N1,
*   independent of any PSoC Creator component (the UART_DEB component does not
*   initialize reliably this early in the bootloader).
*******************************************************************************/

#include "project.h"
#include "common.h"
#include <stdbool.h>
#include <stdint.h>

/*****************************************************************************
* Bit-bang UART debug (P0.5, 9600 8N1)
*****************************************************************************/
#define DBG_PORT        GPIO_PRT0
#define DBG_PIN         (5u)
#define DBG_BAUD        (9600u)
#define DBG_BIT_US      ((1000000u / DBG_BAUD) - 3u)   /* loop-overhead compensated */

static void dbg_init(void)
{
    Cy_GPIO_Pin_FastInit(DBG_PORT, DBG_PIN, CY_GPIO_DM_STRONG_IN_OFF, 1u, HSIOM_SEL_GPIO);
}

static void dbg_putc(char c)
{
    uint32_t v = (uint32_t)(uint8_t)c | 0x100u;     /* trailing stop-bit guard */

    Cy_GPIO_Write(DBG_PORT, DBG_PIN, 0u);           /* start bit */
    Cy_SysLib_DelayUs(DBG_BIT_US);
    for (int i = 0; i < 8; i++)                     /* 8 data bits, LSB first */
    {
        Cy_GPIO_Write(DBG_PORT, DBG_PIN, v & 1u);
        v >>= 1;
        Cy_SysLib_DelayUs(DBG_BIT_US);
    }
    Cy_GPIO_Write(DBG_PORT, DBG_PIN, 1u);           /* stop bit */
    Cy_SysLib_DelayUs(DBG_BIT_US);
}

static void dbg_print(const char *s)
{
    while (*s) { dbg_putc(*s++); }
}

static void dbg_hex32(uint32_t val)
{
    static const char hx[] = "0123456789ABCDEF";
    char buf[11];

    buf[0] = '0';
    buf[1] = 'x';
    for (int i = 0; i < 8; i++) { buf[2 + i] = hx[(val >> ((7 - i) * 4)) & 0xFu]; }
    buf[10] = '\0';
    dbg_print(buf);
}

/*****************************************************************************
* App slot metadata (occupies the reserved last 256 B of each slot)
*****************************************************************************/
#define APP_VALID_MAGIC   (0xDEADBEEFu)

typedef struct {
    uint32_t magic;     /* APP_VALID_MAGIC = valid */
    uint32_t version;   /* higher version wins */
    uint32_t size;      /* image size in bytes (informational) */
    uint32_t crc;       /* CRC32 (not validated yet) */
} fw_metadata_t;

/*****************************************************************************
* Jump into the selected app's CM0+ image.
*****************************************************************************/
static void jump_to_app(uint32_t slot_addr)
{
    uint32_t sp = *(volatile uint32_t *)(slot_addr);        /* initial MSP */
    uint32_t pc = *(volatile uint32_t *)(slot_addr + 4u);   /* Reset_Handler */

    __disable_irq();
    SCB->VTOR = slot_addr;          /* point CM0+ at the app's vector table */
    __DSB();
    __ISB();
    __set_MSP(sp);
    ((void (*)(void))pc)();         /* enter the app's CM0+ Reset_Handler */
}

/*****************************************************************************
* Bootloader entry point (called from main_cm0p.c).
*****************************************************************************/
int HostMain(void)
{
    const fw_metadata_t *m0 = (const fw_metadata_t *)APP0_META_ADDR;
    const fw_metadata_t *m1 = (const fw_metadata_t *)APP1_META_ADDR;
    bool     v0 = (m0->magic == APP_VALID_MAGIC);
    bool     v1 = (m1->magic == APP_VALID_MAGIC);
    uint32_t boot_addr;

    dbg_init();
    dbg_print("\r\n=== PainDrain bootloader ===\r\n");

    if (v0 && v1)
    {
        boot_addr = (m1->version > m0->version) ? APP1_START_ADDR : APP0_START_ADDR;
        dbg_print((m1->version > m0->version) ? "select App1 (newer)\r\n"
                                              : "select App0 (newer/equal)\r\n");
    }
    else if (v1) { boot_addr = APP1_START_ADDR; dbg_print("select App1 (only valid)\r\n"); }
    else if (v0) { boot_addr = APP0_START_ADDR; dbg_print("select App0 (only valid)\r\n"); }
    else         { boot_addr = APP0_START_ADDR; dbg_print("select App0 (default, no metadata)\r\n"); }

    dbg_print("jump -> ");
    dbg_hex32(boot_addr);
    dbg_print("\r\n");

    jump_to_app(boot_addr);

    for (;;) { }   /* never reached */
}

/* [] END OF FILE */
