/*
 * (C) Copyright 2015 Draegerwerk AG
 */

#include <common.h>
#include <post.h>
#include <draeger_m48_pmstruct.h>
#include <asm/arch/reset_manager.h>
#include <watchdog.h>

#define CONFIG_SYS_WATCHDOG_MAGIC		0x12480000
#define CONFIG_SYS_WATCHDOG_MAGIC_MASK	0xFFFF0000

#if CONFIG_POST & CONFIG_SYS_POST_WATCHDOG

/* this test is part of the reset test in order to avoid an additional
*  reboot of the board
*/
#define TEST_SIZE  (0xa00 * 0x1000)
#define PM_BSP_MAGIC        0x07101973

void initializePMSaveMemory(void)
{
    char* pm_usr_addr = (char*) CONFIG_SYS_PM_USR_ADDR;
    post_log("\ninitializePMSaveMemory\n");
    memset(pm_usr_addr, 0x96, TEST_SIZE);
    flush_dcache_all();
}

void checkPMSaveMemory(void)
{
    int i;
    char* pm_usr_addr = (char*) CONFIG_SYS_PM_USR_ADDR;

    for (i=0; i < TEST_SIZE; i++) {
        if (pm_usr_addr[i] != 0x96) {
            break;
        }
    }
    if (i == TEST_SIZE) {
    	m48PmData->post_pmTest.result = M48_TS_PASS;
        post_log("\ncheckPMSaveMemory PASSED\n");
    } else {
    	m48PmData->post_pmTest.result = M48_TS_FAIL;
        post_log("\ncheckPMSaveMemory, failed %08x+%d data %02x\n", pm_usr_addr, i, pm_usr_addr[i]);
    }
    m48PmData->post_pmTest.magic = PM_MEMORY_MAGIC;
    updateM48PmStructChecksum();

}


int watchdog_post_test (int flags)
{
    int ints;
    uint32_t cause;
    ulong base,time;

    static const struct socfpga_reset_manager *reset_manager_base =
            (void *)SOCFPGA_RSTMGR_ADDRESS;

    if ((m48PmData->post_watchdog.magic != CONFIG_SYS_WATCHDOG_MAGIC) || ((flags & POST_MANUAL) != 0)) {
        /* PowerOn */
        m48PmData->post_watchdog.magic  = CONFIG_SYS_WATCHDOG_MAGIC;
        m48PmData->post_watchdog.result = M48_TS_NOT_RUN;
        updateM48PmStructChecksum();

        initializePMSaveMemory();

        ints = disable_interrupts ();
        setbits_le32(&reset_manager_base->ctrl, (1 << RSTMGR_CTRL_SWWARMRSTREQ_LSB)); /* configure for warm reset */

        base = post_time_ms (0);
        while ((time = post_time_ms (base)) < 500)
            ;

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
