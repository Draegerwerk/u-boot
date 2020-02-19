/*
 * watchdog.c
 *
 * SPDX-License-Identifier:        GPL-2.0+
 *
 * (C) Copyright 2015 Draeger and Licensors, info@draeger.com
 *
 */

#include <common.h>
#include <post.h>
#include <draeger_m48_pmstruct.h>

#define CONFIG_SYS_WATCHDOG_MAGIC		0x12480000
#define CONFIG_SYS_WATCHDOG_MAGIC_MASK	0xFFFF0000
#define POST_REMOTE_WAIT_INTERVAL 2000

#if CONFIG_POST & CONFIG_SYS_POST_WATCHDOG


#define WDOG_WCR_WDE        (0x1 << 2)

/* this test is part of the reset test in order to avoid an additional
*  reboot of the board
*/
DECLARE_GLOBAL_DATA_PTR;
#define PERSISTEN_MEMORY_TEST_SIZE    (CONFIG_PM_RESERVED_MEM)
#define PERSISTEN_MEMORY_TEST_PATTERN (0x96)

static char* getPmMemoryRegionInDDR3(void)
{
    uint64_t address;
    address = CONFIG_SYS_SDRAM_BASE + gd->ram_size - CONFIG_PM_RESERVED_MEM; /* VxWorks PM region is at the End of DDR3 addr space*/
    if(  (address <  CONFIG_SYS_SDRAM_BASE)
       ||(address >= CONFIG_SYS_SDRAM_BASE + PHYS_SDRAM_SIZE))
    {
        return (char*)NULL;
    } else {
        return (char*)address;
    }
}

void initializePMSaveMemory(void)
{
    char* m48PmUsrData;
    m48PmUsrData = getPmMemoryRegionInDDR3();
    post_log("\ninitializePMSaveMemory\n");
    if(m48PmUsrData) {
        post_log("start addr: %p\n", m48PmUsrData);
        post_log("size: %zu\n", PERSISTEN_MEMORY_TEST_SIZE);
        memset(m48PmUsrData, PERSISTEN_MEMORY_TEST_PATTERN, PERSISTEN_MEMORY_TEST_SIZE);
        flush_dcache_all();
    } else {
        post_log("failed\n");
    }
}

void checkPMSaveMemory(void)
{
    int i;
    char* m48PmUsrData;
    m48PmUsrData = getPmMemoryRegionInDDR3();
    if(m48PmUsrData) {
        for (i = 0; i < PERSISTEN_MEMORY_TEST_SIZE; i++) {
            if (m48PmUsrData[i] != PERSISTEN_MEMORY_TEST_PATTERN) {
                break;
            }
        }
    }
    if ((i == PERSISTEN_MEMORY_TEST_SIZE) && (m48PmUsrData != NULL)) {
    	m48PmData->post_pmTest.result = M48_TS_PASS;
        post_log("\ncheckPMSaveMemory PASSED\n");
    } else {
    	m48PmData->post_pmTest.result = M48_TS_FAIL;
    	post_log("\ncheckPMSaveMemory, failed");
    	if(m48PmUsrData) {
    	    post_log(" %08x+%d data %02x\n", m48PmUsrData, i, m48PmUsrData[i]);
    	} else {
    	    post_log("\n");
    	}
    }
    m48PmData->post_pmTest.magic = PM_MEMORY_MAGIC;
    updateM48PmStructChecksum();

}


int watchdog_post_test (int flags)
{
    int ints;
    uint32_t cause;
    ulong base,time;

    struct wdog_regs* wdog1 = (struct wdog_regs *)WDOG1_BASE_ADDR;
    struct wdog_regs* wdog2 = (struct wdog_regs *)WDOG2_BASE_ADDR;
    struct src        *psrc = (struct src *)      SRC_BASE_ADDR;

    if ((m48PmData->post_watchdog.magic != CONFIG_SYS_WATCHDOG_MAGIC) || ((flags & POST_MANUAL) != 0)) {
        /* PowerOn */
        m48PmData->post_watchdog.magic  = CONFIG_SYS_WATCHDOG_MAGIC;
        m48PmData->post_watchdog.result = M48_TS_NOT_RUN;
        updateM48PmStructChecksum();

        initializePMSaveMemory();

        while ((time = post_time_ms (0)) < POST_REMOTE_WAIT_INTERVAL) {;}	/* give other UP time to settle */

        ints = disable_interrupts ();
        writew(1, &wdog1->wmcr);
        writew(1, &wdog2->wmcr);
        writew(WDOG_WCR_WDE, &wdog1->wcr);
        writew(WDOG_WCR_WDE, &wdog2->wcr);

        base = post_time_ms (0);
        while ((time = post_time_ms (base)) < 800) {}

        /* we should never reach this code if the test succeeds */
        if (ints) { enable_interrupts (); }

        m48PmData->post_watchdog.result = M48_TS_FAIL;
        updateM48PmStructChecksum();

        post_log("hw watchdog time : %u ms, failed ", time);
        return 2;

    } else {
        /* No PowerOn -> Reset */
        if (m48PmData->post_watchdog.result == M48_TS_NOT_RUN) {
            m48PmData->post_watchdog.result = M48_TS_PASS;
            updateM48PmStructChecksum();

            checkPMSaveMemory();
        }
    }
    return 0;
}

#endif /* CONFIG_POST & CONFIG_SYS_POST_WATCHDOG */
