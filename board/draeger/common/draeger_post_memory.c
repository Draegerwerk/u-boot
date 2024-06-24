/*
 * memory.c
 *
 * SPDX-License-Identifier:        GPL-2.0+
 *
 * (C) Copyright 2015 Draeger and Licensors, info@draeger.com
 *
 */

#include <common.h>
#include <post.h>
#include <draeger_m48_pmstruct.h>

int arch_memory_test_advance(u32 *vstart, u32 *size, phys_addr_t *phys_offset)
{
    m48PmData->post_memtest.magic  = PM_MEMORY_MAGIC;
    m48PmData->post_memtest.result = M48_TS_PASS;
    updateM48PmStructChecksum();
    return 1;
}

void arch_memory_failure_handle(void)
{
    m48PmData->post_memtest.magic  = PM_MEMORY_MAGIC;
    m48PmData->post_memtest.result = M48_TS_FAIL;
    updateM48PmStructChecksum();
    return;
}
