/*
 * Copyright (c) OTH Regensburg, 2017
 *   Author: Andreas Kölbl
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/mmu.h>
#include <bsp/arm-cache-l1.h>
#include <rtems.h>

void BSP_START_TEXT_SECTION bsp_start_hook_0(void)
{
}

void BSP_START_TEXT_SECTION bsp_start_hook_1(void)
{
  uint32_t sctlr;

  sctlr = arm_cp15_get_control();
  /*
   * Do not use bsp_vector_table_begin == 0, since this will get optimized away.
   */
  if (bsp_vector_table_end != bsp_vector_table_size) {
    arm_cp15_set_vector_base_address(bsp_vector_table_begin);

    sctlr &= ~ARM_CP15_CTRL_V;
    arm_cp15_set_control(sctlr);
  }

  /*
   * Current U-boot loader seems to start kernel image
   * with I and D caches on and MMU enabled.
   * If RTEMS application image finds that cache is on
   * during startup then disable caches.
   */
  if ( sctlr & (ARM_CP15_CTRL_I | ARM_CP15_CTRL_C | ARM_CP15_CTRL_M |
    ARM_CP15_CTRL_A | ARM_CP15_CTRL_Z) ) {
    if ( sctlr & (ARM_CP15_CTRL_C | ARM_CP15_CTRL_M) ) {
      /*
       * If the data cache is on then ensure that it is clean
       * before switching off to be extra carefull.
       */
      arm_cp15_data_cache_clean_all_levels();
    }
    rtems_cache_invalidate_entire_data();
    arm_cp15_flush_prefetch_buffer();
    sctlr &= ~(ARM_CP15_CTRL_I | ARM_CP15_CTRL_C | ARM_CP15_CTRL_M |
               ARM_CP15_CTRL_Z | ARM_CP15_CTRL_A);
    arm_cp15_set_control(sctlr);

  }
  bsp_start_copy_sections();
  jetson_setup_mmu_and_cache();
  bsp_start_clear_bss();
}
