/*
 * Copyright (c) OTH Regensburg, 2017
 *   Author: Andreas Kölbl
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_JETSONTK1_CAR_H
#define LIBBSP_ARM_JETSONTK1_CAR_H

#include <bspopts.h>

#define CAR ((void *) 0x60006000)

/* UART clock source: PLLC_OUT0 */
#define CAR_SOURCE_UART 0

#define CAR_DEV_UARTD 0 /* U */
#define CAR_DEV_UARTA 1 /* L */

#define CAR_GATE_MAX 31
#define CAR_GATE_OFFSET 29

int tegra_car_set(uint32_t device_num);
int tegra_car_gate(uint32_t device_num, uint32_t car_source);
int tegra_car_clear(uint32_t device_num);

#endif /* LIBBSP_ARM_JETSONTK1_CAR_H */
