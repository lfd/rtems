/*
 * Copyright (c) OTH Regensburg, 2017
 *   Author: Andreas Kölbl
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_JETSONTK1_MMU_H
#define LIBBSP_ARM_JETSONTK1_MMU_H

#include <bsp/start.h>
#include <bsp/arm-cp15-start.h>
#include <libcpu/arm-cp15.h>

static __attribute__((always_inline)) void jetson_setup_mmu_and_cache(void)
{
  unsigned int sctlr;

  arm_cp15_start_setup_translation_table(
    (uint32_t *) bsp_translation_table_base,
    ARM_MMU_DEFAULT_CLIENT_DOMAIN,
    arm_cp15_start_mmu_config_table,
    arm_cp15_start_mmu_config_table_size
  );
  sctlr = arm_cp15_get_control();

  arm_cp15_instruction_cache_invalidate();
  arm_cp15_branch_predictor_invalidate_all();
  arm_cp15_tlb_invalidate();
  arm_cp15_flush_prefetch_buffer();

  arm_cp15_set_translation_table_base_control_register(0);

  sctlr |= ARM_CP15_CTRL_I | ARM_CP15_CTRL_C | ARM_CP15_CTRL_M |
           ARM_CP15_CTRL_Z | ARM_CP15_CTRL_A | ARM_CP15_CTRL_AFE;
  arm_cp15_set_control( sctlr );
  arm_cp15_tlb_invalidate();
}

#endif /* LIBBSP_ARM_JETSONTK1_MMU_H */
