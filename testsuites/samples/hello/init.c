/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <tmacros.h>

const char rtems_test_name[] = "HELLO WORLD";

uint32_t memory;

static inline void mfence(void)
{
        asm volatile("mfence" ::: "memory");
}

static inline void lfence(void)
{
        asm volatile("lfence" ::: "memory");
}

static inline void clflush(void *addr)
{
	asm volatile("clflush %0" : "+m" (*(char *)addr));
}

static inline uint64_t rdtsc(void)
{
	uint32_t lo, hi;

	asm volatile("rdtsc" : "=a" (lo), "=d" (hi));
	return (uint64_t)lo | (((uint64_t)hi) << 32);
}

static rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_print_printer_fprintf_putc(&rtems_test_printer);
  TEST_BEGIN();
  printf( "Hello World\n" );

unsigned long long before, after;
register uint32_t buffer;
for (;;){
  mfence();
  //clflush(&memory);

  before = rdtsc();
  lfence();
  buffer = memory;
  lfence();
  after = rdtsc();
  lfence();

  printf("mem: %x - delta: %llu\n", buffer, after - before);
}



  TEST_END();
  rtems_test_exit( 0 );
}


/* NOTICE: the clock driver is explicitly disabled */
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS            1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
