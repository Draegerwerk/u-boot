/*
 * (C) Copyright 2015 Draegerwerk AG
 */

#include <common.h>
#include <post.h>
#include <draeger_m48_pmstruct.h>

#if CONFIG_POST & CONFIG_SYS_POST_BSPEC1

/* time to wait for reboot of uP1 */
#define POST_REMOTE_WAIT_INTERVAL_1  500
/* time to wait for reset triggered by uP1 */
#define POST_REMOTE_WAIT_INTERVAL_2 2000

int board_reset_post_test (int flags)
{
    int ints;
    ulong base,time;

    if ((m48PmData->post_board_reset.magic != CONFIG_SYS_BRESET_MAGIC) || ((flags & POST_MANUAL) != 0)) {
        /* PowerOn */
        m48PmData->post_board_reset.result = M48_TS_NOT_RUN;
        if ((flags & POST_MANUAL) != 0) {
            m48PmData->post_board_reset.state  = POST_BRESET_STATE_UP2_RESET;
        } else {
            m48PmData->post_board_reset.state  = POST_BRESET_STATE_INITIAL;
        }
        m48PmData->post_board_reset.magic  = CONFIG_SYS_BRESET_MAGIC;
        /* hack: board_test_run_always() only adds the POST reset test time in case
                 of power on. The timestamp_post is used as a flag to indicate the power on condition */
        m48PmData->timestamp_post = 0;
        updateM48PmStructChecksum();
    }

    if (m48PmData->post_board_reset.result == M48_TS_NOT_RUN) {
        switch (m48PmData->post_board_reset.state) {
        case POST_BRESET_STATE_UP1_RESET:
            m48PmData->post_board_reset.state = POST_BRESET_STATE_UP2_RESET;
            updateM48PmStructChecksum();

            post_log("\nhw reset test time : %u ms waiting for reboot from other UP", post_time_ms (0));
            while ((m48PmData->remoteDelay = post_time_ms (0)) < POST_REMOTE_WAIT_INTERVAL_2) { flush_dcache_all(); }
            post_log("hw reset time : %u ms, failed while waiting for reset from other UP ", m48PmData->remoteDelay);

            m48PmData->post_board_reset.result = M48_TS_FAIL;
            updateM48PmStructChecksum();

            return 1;
        case POST_BRESET_STATE_UP2_RESET:
            m48PmData->post_board_reset.state = POST_BRESET_STATE_BOTH_DONE;
            updateM48PmStructChecksum();

            while ((time = post_time_ms (0)) < POST_REMOTE_WAIT_INTERVAL_1) { ; }	/* give other UP time to settle */

            post_log("hw reset time: %u ms\n", post_time_ms (0));
            ints = disable_interrupts ();

            m48PmData->timestamp_kernelloaded = post_time_ms (0); /* hack: to store the time that passed during POST_BRESET_STATE_UP2_RESET*/
            flush_dcache_all();
            gpio_direction_output(CONFIG_GPIO_BOARD_COLD_RESET, 0); /* no meaningful return value */
            base = post_time_ms (0);
            while ((time = post_time_ms (0)) < base + 150)
            {
                m48PmData->timestamp_kernelloaded = post_time_ms (0); /* hack: to store the time that passed during POST_BRESET_STATE_UP2_RESET*/
                flush_dcache_all();
            }
            post_log("hw reset time : %u ms, failed while waiting for own reset ", time);

            /* we should never reach this code if the test succeeds */
            if (ints) { enable_interrupts (); }

            m48PmData->post_board_reset.result = M48_TS_FAIL;
            updateM48PmStructChecksum();

            return 2;

        case POST_BRESET_STATE_BOTH_DONE:
            m48PmData->post_board_reset.result = M48_TS_PASS;
            /* hack: to store the time of the first two POST reset cycles
             * timestamp_kernelloaded is reused in order to avoid changing the PmBoot structure
             * timestamp_kernel_loaded contains now the time of up1 and up2 triggered reset stages of POST reset test */
            m48PmData->timestamp_kernelloaded += m48PmData->remoteDelay;
            updateM48PmStructChecksum();
            post_log("remote delay : %u/%u ms ", m48PmData->remoteDelay, POST_REMOTE_WAIT_INTERVAL_2);
            break;

        default:
            post_log("hw reset test out of state ");
            return 3;
        }
    }
    return 0;
}

#endif /* CONFIG_POST & CONFIG_SYS_POST_WATCHDOG */
