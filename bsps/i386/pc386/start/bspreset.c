/*
 *  COPYRIGHT (c) 1989-2017.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <bsp.h>
#include <bsp/bootcard.h>

void bsp_reset(void)
{
  /* shutdown and reboot */
  //outport_byte(0x64, 0xFE);        /* use keyboard controller */
  asm volatile("1:\t\n"
	       "hlt\t\n"
	       "jmp 1b\t\n");
}
