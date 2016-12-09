/*
 * Copyright (c) OTH Regensburg, 2017
 *   Author: Andreas Kölbl
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <rtems.h>
#include <rtems/btimer.h>
#include <bsp/memory.h>

#define TMR1 (0x0)
#define TMR2 (0x8)
#define TIMERUS (0x10)
#define TMR3 (0x50)
#define TMR4 (0x58)
#define TMR5 (0x60)
#define TMR6 (0x68)
#define TMR7 (0x70)
#define TMR8 (0x78)
#define TMR9 (0x80)
#define TMR0 (0x88)

#define TIMERUS_BASE (TIMER_BASE + TIMERUS)

#define TIMERUS_CFG 0x4
#define TIMERUS_CNTR 0x0
#define TIMERUS_USEC_CFG_12MHZ 0x000b

static bool benchmark_timer_find_average_overhead = true;

static uint32_t benchmark_timer_base;

void benchmark_timer_initialize(void)
{
  /* Set clock speed */
  mmio_write(TIMERUS_BASE + TIMERUS_CFG, TIMERUS_USEC_CFG_12MHZ);
  /* Write timeout into Virtual Timer TimerValue register */
  arm_write_cp15_32(0, c14, c3, 0, UINT32_MAX );
  /* Run the timer with Virtual Timer Control register */
  arm_write_cp15_32(0, c14, c3, 1, 1);
  benchmark_timer_base = mmio_read(TIMERUS_BASE + TIMERUS_CNTR);
}

benchmark_timer_t benchmark_timer_read(void)
{
  benchmark_timer_t delta;

  delta = mmio_read(TIMERUS_BASE + TIMERUS_CNTR) - benchmark_timer_base;

  if ( benchmark_timer_find_average_overhead == true) {
    return delta;
  } else {
    return mmio_read(TIMERUS_BASE + TIMERUS_CNTR);
  }
}

void benchmark_timer_disable_subtracting_average_overhead(bool find_flag)
{
  benchmark_timer_find_average_overhead = find_flag;
}
