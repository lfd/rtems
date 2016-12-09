/*
 * Copyright (c) OTH Regensburg, 2017
 *   Author: Andreas Kölbl
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/timecounter.h>
#include <rtems/bspIo.h>
#include <bsp.h>
#include <bsp/memory.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>

#define USEC_PER_SEC 1000000L
#define GICD_ICPENDR 0x1280
#define TIMER_IRQ 27

/* This is defined in ../../../shared/clockdrv_shell.h */
void Clock_isr(rtems_irq_hdl_param arg);
static uint32_t           timecounter_ticks_per_clock_tick;
static struct timecounter clock_tc;

static void gic_timer_start(uint32_t timeout)
{
  /* Write timeout into Virtual Timer TimerValue register */
  arm_write_cp15_32(0, c14, c3, 0, timeout);
  /* Run the timer with Virtual Timer Control register */
  arm_write_cp15_32(0, c14, c3, 1, 1);
}

static uint32_t jetson_clock_get_timecount(struct timecounter *tc)
{
  uint64_t ret = 0;

  /* Read the Physical Count Register */
  arm_read_cp15_64(0, c14, ret);

  /* This should be safe for rtems */
  return (uint32_t) ret;
}

static void jetson_clock_at_tick(void)
{
  uint32_t CNTV_TVAL = 0;

  arm_read_cp15_32(0, c14, c3, 0, CNTV_TVAL);

  if ( CNTV_TVAL >= timecounter_ticks_per_clock_tick ) {
    /* Clear pending interrupt */
    mmio_write(BSP_ARM_GIC_DIST_BASE + GICD_ICPENDR, 1);
    /* Reset the Virtual Timer */
    arm_write_cp15_32(0, c14, c3, 1, 0);
    gic_timer_start(timecounter_ticks_per_clock_tick);
  }
}

static void jetson_clock_handler_install_isr(rtems_isr_entry clock_isr)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if ( clock_isr != NULL ) {
    sc = rtems_interrupt_handler_install(
      TIMER_IRQ,
      "Clock",
      RTEMS_INTERRUPT_UNIQUE,
      (rtems_interrupt_handler) clock_isr,
      NULL
    );
  } else {
    sc = rtems_interrupt_handler_remove(
      TIMER_IRQ,
      (rtems_interrupt_handler) Clock_isr,
      NULL
    );
  }

  if ( sc != RTEMS_SUCCESSFUL ) {
    printk("fatal: %p\n", (uint32_t *) clock_isr);
    rtems_fatal_error_occurred(0xc10cd1f3);
  }
}

static void jetson_clock_initialize_hardware(void)
{
  uint32_t us_per_tick = rtems_configuration_get_microseconds_per_tick();
  uint32_t frequency;

  /* Get frequency of Generic Timer (CNTFRQ) */
  arm_read_cp15_32(0, c14, c0, 0, frequency);
  timecounter_ticks_per_clock_tick =
    ((uint64_t) frequency * us_per_tick) / (uint64_t) USEC_PER_SEC;

  gic_timer_start(timecounter_ticks_per_clock_tick);
  clock_tc.tc_get_timecount = jetson_clock_get_timecount;
  clock_tc.tc_counter_mask = 0xffffffff;
  clock_tc.tc_frequency = frequency;
  clock_tc.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
  rtems_timecounter_install(&clock_tc);
}

static void jetson_clock_cleanup(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  /* Stop the timer */
  arm_write_cp15_32(0, c14, c3, 1, 0);

  sc = rtems_interrupt_handler_remove(
    TIMER_IRQ,
    (rtems_interrupt_handler) Clock_isr,
    NULL
  );

  if ( sc != RTEMS_SUCCESSFUL ) {
    rtems_fatal_error_occurred(0xdeadbeef);
  }
}

#define Clock_driver_support_at_tick() jetson_clock_at_tick()

#define Clock_driver_support_initialize_hardware() \
  jetson_clock_initialize_hardware()

#define Clock_driver_support_shutdown_hardware() jetson_clock_cleanup()

#define Clock_driver_support_install_isr(clock_isr, old_isr) \
  do {                                                 \
    jetson_clock_handler_install_isr(clock_isr);  \
    old_isr = NULL;                                    \
  } while ( 0 )

#define CLOCK_DRIVER_USE_ONLY_BOOT_PROCESSOR 1

#include "../../../shared/clockdrv_shell.h"
