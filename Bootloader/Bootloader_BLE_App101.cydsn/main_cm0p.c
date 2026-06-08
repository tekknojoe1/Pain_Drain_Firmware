/*******************************************************************************
* File Name: main_cm0p.c
*
* Description:
*   App0 bootloader — runs at 0x10000000 (CM0+).
*
*   Boot logic:
*     Cold reset:    check App1 vector table at 0x10040000 — if present, set
*                    APP1_LAUNCH_MAGIC in NOINIT RAM and soft-reset so App1 gets
*                    clean peripheral state (ECO/PLL need a hardware-reset-like start).
*     Soft reset, magic set:  jump to App1 CM0+ reset handler.
*     Soft reset, OTA path:   magic clear (App1 called Cy_DFU_ExecuteApp(0)) → DFU hold.
*
*   Phase 1: DFU hold is a UART message loop.  Program App1 via SWD.
*   Phase 2 (TODO): start CM4 at 0x10010000 and run BLE DFU receive.
*******************************************************************************/

#include "project.h"
#include "cy_syslib.h"
#include "cy_gpio.h"
#include "cy_sysclk.h"
#include <stdint.h>
#include "version.h"

/* NOINIT: survives soft reset, random on power-on.  4-byte magic makes accidental
 * match astronomically unlikely so no explicit init is needed. */
__attribute__((section(".cy_boot_noinit"), used))
static volatile uint32_t app0_boot_magic;

#define APP1_LAUNCH_MAGIC  0xA15A5AA1u

/* App1 identity: App1's flash starts with a 256-byte header block, then the vector table.
 * The vector table must be VTOR-aligned (256 bytes), so APP1_VEC_OFFSET = 0x100.
 * APP1_META_MAGIC must match the value in App1's main_cm0p.c. */
#define APP1_FLASH_BASE   0x10040000u
#define APP1_VEC_OFFSET   0x100u
#define APP1_META_MAGIC   0xA1B2C3D4u

typedef struct {
    uint32_t magic;
    uint32_t version;
} app1_meta_t;

/*
 * TOC2 at supervisory flash 0x16007C00.
 * appAddr1 = 0x10000000 (App0 CM0+).
 * CRC16-CCITT-FALSE over first 508 bytes = 0xF1AD.
 */
typedef struct {
    uint32_t objSize;
    uint32_t magicNum;
    uint32_t keyBlockAddr;
    uint32_t smifCfgAddr;
    uint32_t appAddr1;
    uint32_t appFormat1;
    uint32_t appAddr2;
    uint32_t appFormat2;
    uint32_t shashObj;
    uint32_t sigKeyAddr;
    uint32_t addObj[116];
    uint32_t tocFlags;
    uint32_t crc;
} cy_toc2_t;

__attribute__((section(".cy_toc_part2"), used))
static const cy_toc2_t cy_toc2 = {
    .objSize     = 0x000001FCu,
    .magicNum    = 0x01211220u,
    .keyBlockAddr = 0xFFFFFFFFu,
    .smifCfgAddr  = 0xFFFFFFFFu,
    .appAddr1    = 0x10000000u,   /* App0 CM0+ start */
    .appFormat1  = 0x00000000u,
    .appAddr2    = 0xFFFFFFFFu,
    .appFormat2  = 0x00000000u,
    .shashObj    = 0x00000000u,
    .sigKeyAddr  = 0xFFFFFFFFu,
    .addObj      = {0},
    .tocFlags    = 0x00000000u,
    .crc         = 0xF1AD0000u,   /* CRC16=0xF1AD in upper 16 bits */
};

/* Bit-bang UART on P0.5 at 9600 baud */
#define DBG_TX_PORT  GPIO_PRT0
#define DBG_TX_PIN   5u
#define DBG_BIT_US   ((1000000u / 9600u) - 3u)

static void boot_uart_init(void) {
    Cy_GPIO_Pin_FastInit(DBG_TX_PORT, DBG_TX_PIN, CY_GPIO_DM_STRONG_IN_OFF, 1u, HSIOM_SEL_GPIO);
}

static void boot_putchar(char c) {
    Cy_GPIO_Write(DBG_TX_PORT, DBG_TX_PIN, 0u);
    Cy_SysLib_DelayUs(DBG_BIT_US);
    for (int i = 0; i < 8; i++) {
        Cy_GPIO_Write(DBG_TX_PORT, DBG_TX_PIN, (uint32_t)(c & 1));
        c >>= 1;
        Cy_SysLib_DelayUs(DBG_BIT_US);
    }
    Cy_GPIO_Write(DBG_TX_PORT, DBG_TX_PIN, 1u);
    Cy_SysLib_DelayUs(DBG_BIT_US);
}

static void boot_puts(const char *s) {
    while (*s) boot_putchar(*s++);
}

static void boot_puthex(uint32_t v) {
    boot_puts("0x");
    for (int i = 28; i >= 0; i -= 4) {
        uint8_t n = (v >> i) & 0xF;
        boot_putchar(n < 10 ? '0' + n : 'A' + n - 10);
    }
}

static void boot_putdec8(uint8_t v) {
    if (v >= 100u) boot_putchar((char)('0' + v / 100u));
    if (v >= 10u)  boot_putchar((char)('0' + (v / 10u) % 10u));
    boot_putchar((char)('0' + v % 10u));
}

/* Print a packed version uint32 (major<<16 | minor<<8 | patch) as "X.Y.Z" */
static void boot_putver(uint32_t v) {
    boot_putdec8((uint8_t)((v >> 16u) & 0xFFu));
    boot_putchar('.');
    boot_putdec8((uint8_t)((v >> 8u) & 0xFFu));
    boot_putchar('.');
    boot_putdec8((uint8_t)(v & 0xFFu));
}


int main(void)
{
    if (Cy_SysPm_GetIoFreezeStatus()) {
        Cy_SysPm_IoUnfreeze();
    }
    __enable_irq();

    boot_uart_init();
    boot_puts("\r\n=== App0 Bootloader v" APP0_VERSION_STR " ===\r\n");

    /* RES_CAUSE is sticky — it accumulates bits across resets (e.g. HIB_WAKEUP persists
     * into the subsequent soft reset). Use a bitmask check and clear immediately so
     * stale bits from hibernate wakeup don't poison future reset-reason reads. */
    uint32_t reset_reason = Cy_SysLib_GetResetReason();
    Cy_SysLib_ClearResetReason();
    bool soft_reset = (reset_reason & CY_SYSLIB_RESET_SOFT) != 0u;

    /* Soft reset triggered by App0 itself: peripherals now clean, jump to App1. */
    if (soft_reset && app0_boot_magic == APP1_LAUNCH_MAGIC) {
        app0_boot_magic = 0u;

        volatile const app1_meta_t *meta = (volatile const app1_meta_t *)APP1_FLASH_BASE;
        boot_puts("Launching App1 v");
        boot_putver(meta->version);
        boot_puts("\r\n");

        /* Switch CPU back to IMO so App1's cyfitter_cfg starts with a clean clock
         * state (FLL off, PLL off).  Without this, PLL1 in App1's ClockInit fails
         * to lock within its 10 ms timeout and spins forever in CyClockStartupError. */
        Cy_SysClk_FllDisable();   /* HFCLK0 falls back to path-0 source = IMO (8 MHz) */

        volatile const uint32_t *vec = (volatile const uint32_t *)(APP1_FLASH_BASE + APP1_VEC_OFFSET);
        uint32_t app1_sp = vec[0];
        uint32_t app1_pc = vec[1];
        __disable_irq();
        SCB->VTOR = APP1_FLASH_BASE + APP1_VEC_OFFSET;
        __DSB();
        __ISB();
        __asm volatile (
            "MSR MSP, %[sp]\n\t"
            "BX  %[pc]\n\t"
            : : [sp] "r"(app1_sp), [pc] "r"(app1_pc)
        );
        /* Not reached */
    }

    app0_boot_magic = 0u;  /* clear stale magic on any other path */

    /* Check for App1 regardless of reset type.  PSoC Creator's programmer always
     * triggers a soft reset after programming, so a soft-reset-without-magic looks
     * identical to a cold boot from App0's perspective.  Always do the presence check
     * so that programming App1 then letting the programmer reset is enough to launch it.
     * (OTA will set a separate DFU magic value when that feature is implemented.) */
    volatile const app1_meta_t *app1_meta = (volatile const app1_meta_t *)APP1_FLASH_BASE;
    if (app1_meta->magic == APP1_META_MAGIC) {
        boot_puts("App1 v");
        boot_putver(app1_meta->version);
        boot_puts(" found — resetting for clean launch\r\n");
        app0_boot_magic = APP1_LAUNCH_MAGIC;
        NVIC_SystemReset();
        /* Not reached */
    }

    boot_puts("App1 not found — entering DFU hold\r\n");

    /* Phase 1 DFU hold: reprogram App1 via SWD, then reset.
     * Phase 2 TODO: start CM4 here and run BLE DFU receive loop. */
    boot_puts("DFU hold: program App1 via SWD, then reset device\r\n");

    for (;;) {
        Cy_SysLib_Delay(5000u);
        boot_puts("Waiting for App1 programming...\r\n");
    }
}

/* [] END OF FILE */
