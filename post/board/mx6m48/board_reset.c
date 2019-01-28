/*
 * (C) Copyright 2015 Draegerwerk AG
 */

#include <common.h>
#include <post.h>
#include <draeger_m48_pmstruct.h>
#include <asm/gpio.h>
#include <stddef.h>

#define CONFIG_SYS_BRESET_MAGIC			0xfdb9
#define POST_BRESET_STATE_UP1_RESET		0
#define POST_BRESET_STATE_UP2_RESET		1
#define POST_BRESET_STATE_BOTH_DONE		2

#if CONFIG_POST & CONFIG_SYS_POST_BSPEC1

/* time to wait for reboot of uP2 */
#define POST_REMOTE_WAIT_INTERVAL_1 1500
/* time to wait for reset triggered by uP2 */
#define POST_REMOTE_WAIT_INTERVAL_2 2000

int board_reset_post_test (int flags)
{
    int ints;
    ulong base,time;



    if ((m48PmData->post_board_reset.magic != CONFIG_SYS_BRESET_MAGIC) || ((flags & POST_MANUAL) != 0)) {
        /* PowerOn */
        m48PmData->post_board_reset.result = M48_TS_NOT_RUN;
        m48PmData->post_board_reset.state  = POST_BRESET_STATE_UP1_RESET;
        m48PmData->post_board_reset.magic  = CONFIG_SYS_BRESET_MAGIC;
        updateM48PmStructChecksum();
    }

    if (m48PmData->post_board_reset.result == M48_TS_NOT_RUN) {
        switch (m48PmData->post_board_reset.state) {
        case POST_BRESET_STATE_UP1_RESET:
            m48PmData->post_board_reset.state = POST_BRESET_STATE_UP2_RESET;
            updateM48PmStructChecksum();

            while ((time = post_time_ms (0)) < POST_REMOTE_WAIT_INTERVAL_1) {;}	/* give other UP time to settle */

            post_log("hw reset time: %u ms\n", time);

            ints = disable_interrupts ();

            if (gpio_direction_output(CONFIG_GPIO_BOARD_COLD_RESET, 0) != 0) {
                post_log("hw reset : failed to access GPIO %u ", CONFIG_GPIO_BOARD_COLD_RESET);
                m48PmData->post_board_reset.result = M48_TS_FAIL;
                updateM48PmStructChecksum();
                return 4;
            } else {
                base = post_time_ms (0);
                while ((time = post_time_ms (0)) < base + 150) { ; }
                post_log("hw reset time : %u ms, failed while waiting for own reset \n", time);
            }

            /* we should never reach this code if the test succeeds */
            if (ints) { enable_interrupts (); }

            m48PmData->post_board_reset.result = M48_TS_FAIL;
            updateM48PmStructChecksum();

            return 1;
        case POST_BRESET_STATE_UP2_RESET:
            m48PmData->post_board_reset.state = POST_BRESET_STATE_BOTH_DONE;
            updateM48PmStructChecksum();

            post_log("\nhw reset test time : %u ms waiting for reboot from other UP\n", post_time_ms (0));
            while ((m48PmData->remoteDelay = post_time_ms (0)) < POST_REMOTE_WAIT_INTERVAL_2) { flush_dcache_all(); }
            post_log("hw reset time : %u ms, failed while waiting for reset from other UP\n", m48PmData->remoteDelay);

            m48PmData->post_board_reset.result = M48_TS_FAIL;
            updateM48PmStructChecksum();

            return 2;
        case POST_BRESET_STATE_BOTH_DONE:
            m48PmData->post_board_reset.result = M48_TS_PASS;
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
