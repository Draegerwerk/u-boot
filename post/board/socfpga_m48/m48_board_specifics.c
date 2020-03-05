/* m48_board_specifics.c
 *
 * SPDX-License-Identifier:        GPL-2.0+
 *
 * (C) Copyright 2015 Draeger and Licensors, info@draeger.com
 *
 */

#include <common.h>
#include <post.h>
#include <draeger_m48_pmstruct.h>
#include <version.h>

#ifdef CONFIG_POST

#ifdef CONFIG_SYS_POST_BSPEC3

int board_test_run_always (int flags) {
	ulong currentTime;
	currentTime = get_timer (0);
    if(   (m48PmData->post_board_reset.result == M48_TS_PASS)
       && (m48PmData->timestamp_post          == 0          )) /* hack: timestamp_post is a flag that idicates cold start if set to 0 */
	{
		/* hack: m48PmData->timestamp_kernelloaded contains the time it took
		 * to perform the two reset cycles of POST reset test
		 *
		 * 0----->t_reset_by_up1
		 * ^             0--------->t_reset_by_self (up2)
		 * |             			   0--------------------->t_post_done-------------->t_kernelloaded
		 * |             			   ^							^
		 * |						   |                            |
		 * |-- timestamp_kernelloaded -|-------- currentTime -------|
		*/
		m48PmData->timestamp_post = currentTime + m48PmData->timestamp_kernelloaded;
	}
    else
    {
		/* POST reset test has not been performed therefore the entire POST test time is from last reset to now */
    	m48PmData->timestamp_post = currentTime;
    }

    /* hack: for now we store the time it took to reach this point (POST done) since the last reset occurred
			 this information will be used in board_test_run_always() */
    m48PmData->timestamp_kernelloaded = currentTime;
    m48PmData->bootmode = 0;
    updateM48PmStructChecksum();
    return 0;
}

#endif

#endif
