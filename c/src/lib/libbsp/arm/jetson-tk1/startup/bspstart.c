/*
 * Copyright (c) OTH Regensburg, 2017
 *   Author: Andreas Kölbl
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp/irq-generic.h>
#include <bsp/bootcard.h>

void bsp_start(void)
{
  bsp_interrupt_initialize();
}
