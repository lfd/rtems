/*
 * Copyright (c) OTH Regensburg, 2017
 *   Author: Andreas Kölbl
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/console.h>
#include <bsp/memory.h>
#include <rtems/bspIo.h>

#define UART_LSR 0x14
#define  UART_LSR_RDR (1 << 0)

#define UART_RBR (0 << 0)

static void ns8250_debug_console_out(char c)
{
  while ( !(mmio_read(UARTD + UART_LSR) & UART_LSR_THRE) ) {
    asm volatile ("nop");
  }

  if ( c == '\n' ) {
    mmio_write(UARTD + UART_TX, '\r');
  }

  mmio_write( UARTD + UART_TX, c );
}

static int ns8250_debug_console_in(void)
{
  int result;

  while ( !(mmio_read( UARTD + UART_LSR ) & UART_LSR_RDR) ) {
    asm volatile ( "nop" );
  }

  /* Read from UARTD */
  result = mmio_read(UARTD + UART_RBR);
  /* Clear interrupts */
  mmio_write(UARTD + UART_LSR, 0);

  return result;
}

BSP_output_char_function_type     BSP_output_char = ns8250_debug_console_out;
BSP_polling_getchar_function_type BSP_poll_char = ns8250_debug_console_in;
