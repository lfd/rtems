/*
 * Copyright (c) OTH Regensburg, 2017
 *   Author: Andreas Kölbl
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_JETSONTK1_MEMORY_H
#define LIBBSP_ARM_JETSONTK1_MEMORY_H

#define arm_write_cp15_32(op1, crn, crm, op2, val) \
  asm volatile("mcr	p15, "#op1 ", %0, "#crn ", "#crm ", "#op2 "\n" \
                 : : "r" ((uint32_t) (val)))
#define arm_write_cp15_64(op1, crm, val) \
  asm volatile("mcrr	p15, "#op1 ", %Q0, %R0, "#crm "\n" \
                 : : "r" ((uint64_t) (val)))

#define arm_read_cp15_32(op1, crn, crm, op2, val) \
  asm volatile("mrc	p15, "#op1 ", %0, "#crn ", "#crm ", "#op2 "\n" \
                 : "=r" ((uint32_t) (val)))
#define arm_read_cp15_64(op1, crm, val) \
  asm volatile("mrrc	p15, "#op1 ", %Q0, %R0, "#crm "\n" \
                 : "=r" ((uint64_t) (val)))

#define mmio_read(address) (*(volatile uint32_t *) (address))
#define mmio_write(address, value) \
  (*(volatile uint32_t *) (address) = (value))

#endif /* LIBBSP_ARM_JETSONTK1_MEMORY_H */
