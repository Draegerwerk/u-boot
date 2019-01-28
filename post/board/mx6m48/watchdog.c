/*
 * (C) Copyright 2015 Draegerwerk AG
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
#define TEST_SIZE (0xa00 * 0x1000)
#define PM_BSP_MAGIC        0x07101973

void initializePMSaveMemory(void)
{
    post_log("\ninitializePMSaveMemory\n");
    memset(m48PmUsrData, 0x96, TEST_SIZE);
    flush_dcache_all();
}

void checkPMSaveMemory(void)
{
    int i;

    for (i=0; i < TEST_SIZE; i++) {
        if (m48PmUsrData[i] != 0x96) {
            break;
        }
    }
    if (i == TEST_SIZE) {
    	m48PmData->post_pmTest.result = M48_TS_PASS;
        post_log("\ncheckPMSaveMemory, PASSED\n");
    } else {
    	m48PmData->post_pmTest.result = M48_TS_FAIL;
        post_log("\ncheckPMSaveMemory, failed %08x+%d data %02x\n", m48PmUsrData, i, m48PmUsrData[i]);
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
