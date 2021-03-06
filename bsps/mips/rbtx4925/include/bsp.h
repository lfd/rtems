/**
 * @file
 *
 * @ingroup RTEMSBSPsMIPSRBTX4925
 *
 * @brief Global BSP definitions.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_MIPS_RBTX4925_BSP_H
#define LIBBSP_MIPS_RBTX4925_BSP_H

/**
 * @defgroup RTEMSBSPsMIPSRBTX4925 RBTX4925
 *
 * @ingroup RTEMSBSPsMIPS
 *
 * @brief RBTX4925 Board Support Package.
 *
 * @{
 */

#ifndef ASM

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#include <rtems.h>
#include <libcpu/tx4925.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BSP_FEATURE_IRQ_EXTENSION
#define BSP_SHARED_HANDLER_SUPPORT      1

/*
 * Prototypes for methods called from .S for dependency tracking
 */
void init_tlb(void);
void resettlb(int i);

#ifdef __cplusplus
}
#endif

#endif /* !ASM */

/** @} */

#endif
