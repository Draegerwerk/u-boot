/*
 * (C) Copyright 2015 Draegerwerk AG
 */

#include <common.h>
#include <post.h>
#include <draeger_m48_pmstruct.h>
#include <version.h>

#ifdef CONFIG_POST

#ifdef CONFIG_SYS_POST_BSPEC3

int board_test_run_always (int flags) {
    m48PmData->timestamp_post = get_timer (0);
    m48PmData->timestamp_kernelloaded = 0;
    m48PmData->bootmode = 0;
    updateM48PmStructChecksum();
    return 0;
}

#endif

static int do_scTestResult(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    uint32_t result;

    if (argc != 2)
        return cmd_usage(cmdtp);

    result = simple_strtoul(argv[1], NULL, 16);

    m48PmBspData->post_scriptTest.magic = PM_MEMORY_MAGIC;
    m48PmBspData->post_scriptTest.result = result;
    m48PmBspData->checkSum =
            crc32 (0, (const char *) m48PmBspData, offsetof(PmBspData, checkSum));

    return 0;
}

U_BOOT_CMD(
        scrtest, 2, 0, do_scTestResult,
       "store the result of the script test into m48PmBspData",
       "scrtest <result>"
       );


#endif
