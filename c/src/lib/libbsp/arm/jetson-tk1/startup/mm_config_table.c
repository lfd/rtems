/*
 * Copyright (c) OTH Regensburg, 2017
 *   Author: Andreas Kölbl
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bspopts.h>
#include <bsp/car.h>
#include <bsp/console.h>
#include <bsp/arm-cp15-start.h>

/*
 * Pagetable initialization data
 *
 * Keep all read-only sections before read-write ones.
 * This ensures that write is allowed if one page/region
 * is partially filled by read-only section content
 * and rest is used for writeable section
 */

BSP_START_DATA_SECTION const arm_cp15_start_section_config
arm_cp15_start_mmu_config_table[] = {
  ARMV7_CP15_START_DEFAULT_SECTIONS,
  {
    .begin = (uint32_t) CAR,
    .end = (uint32_t) (CAR + 0x1000),
    .flags = ARMV7_MMU_DEVICE
  }, {
    .begin = (uint32_t) TIMER_BASE,
    .end = (uint32_t) (TIMER_BASE + 0x1000),
    .flags = ARMV7_MMU_DEVICE
  }, {
    .begin = (uint32_t) APB_MISC_BASE,
    .end = (uint32_t) (APB_MISC_BASE + 0x1000),
    .flags = ARMV7_MMU_DEVICE
  }, {
    .begin = (uint32_t) UARTA,
    .end = (uint32_t) UARTA + 0x1000,
    .flags = ARMV7_MMU_DEVICE
  }, {
    .begin = (uint32_t) BSP_ARM_GIC_DIST_BASE,
    .end = (uint32_t) (BSP_ARM_GIC_DIST_BASE + 0x1000),
    .flags = ARMV7_MMU_DEVICE
  }, {
    .begin = (uint32_t) BSP_ARM_GIC_CPUIF_BASE,
    .end = (uint32_t) (BSP_ARM_GIC_CPUIF_BASE + 0x1000),
    .flags = ARMV7_MMU_DEVICE
  }
};

const size_t arm_cp15_start_mmu_config_table_size =
  RTEMS_ARRAY_SIZE(arm_cp15_start_mmu_config_table);
