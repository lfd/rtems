/*
 * Copyright (c) OTH Regensburg, 2017
 *   Author: Andreas Kölbl
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/memory.h>
#include <bsp/bootcard.h>

#define MAIN_RST 4

void bsp_reset(void)
{
  mmio_write(PMC_BASE, mmio_read(PMC_BASE) | (1 << MAIN_RST));
}
