/*
 * Copyright (c) OTH Regensburg, 2017
 *   Author: Andreas Kölbl
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <unistd.h>

#include <rtems/console.h>
#include <rtems/termiostypes.h>
#include <rtems/bspIo.h>

#include <bsp.h>
#include <bspopts.h>
#include <bsp/car.h>
#include <bsp/console.h>
#include <bsp/fatal.h>
#include <bsp/irq.h>
#include <bsp/memory.h>

#define UART_TX 0x0
#define UART_DLL 0x0
#define UART_RBR 0x0
#define UART_DLM 0x4
#define UART_IER 0x4
#define UART_IIR 0x8
#define UART_LCR 0xc
#define  UART_LCR_8N1 ((1 << 0) | (1 << 1))
#define  UART_LCR_DLAB (1 << 7)
#define UART_LSR 0x14
#define  UART_LSR_RDR (1 << 0)
#define  UART_LSR_THRE (1 << 5)
#define UART_MSR 0x18

#define UART_IER_IE_RHR (1 << 0)
#define UART_IER_IE_THR (1 << 1)

#define BSP_UART_DEFAULT_BAUD_RATE 115200L
#define BSP_UART_SPEED 408000000L

typedef struct {
  rtems_termios_device_context base;
  void *regs;
  uint32_t car;
  bool console;
  const char *device_name;
#if NS8250_CONSOLE_USE_INTERRUPTS == 1
  rtems_vector_number irq;
  volatile bool transmitting;
#endif
} ns8250_uart_context;

static ns8250_uart_context ns8250_uart_instances[] = {
  {
    .regs = UARTD,
    .car = CAR_DEV_UARTD,
    .device_name = "/dev/ttyS0",
#if NS8250_CONSOLE_USE_INTERRUPTS == 1
    .irq = UARTD_IRQ,
    .transmitting = false,
#endif
  },
  {
    .regs = UARTA,
    .car = CAR_DEV_UARTA,
    .device_name = "/dev/ttyS1",
#if NS8250_CONSOLE_USE_INTERRUPTS == 1
    .irq = UARTA_IRQ,
    .transmitting = false,
#endif
  },
};

static void ns8250_uart_write(
  rtems_termios_device_context *context,
  const char                   *buf,
  size_t                        len
)
{
  ns8250_uart_context *ctx = (ns8250_uart_context *) context;

#if NS8250_CONSOLE_USE_INTERRUPTS == 1

  if ( len ) {
    ctx->transmitting = true;
    mmio_write(ctx->regs + UART_TX, buf[0]);
    mmio_write(ctx->regs + UART_IER,
      mmio_read(ctx->regs + UART_IER) | UART_IER_IE_THR);
  } else {
    mmio_write(ctx->regs + UART_IER,
      mmio_read(ctx->regs + UART_IER) & ~UART_IER_IE_THR);
    ctx->transmitting = false;
  }

#else
  unsigned int i = 0;

  for ( i = 0; i < len; i++ ) {
    while ( !(mmio_read(ctx->regs + UART_LSR) & UART_LSR_THRE) ) {
      asm volatile("nop");
    }

    if ( buf[i] == '\n' ) {
      mmio_write(ctx->regs + UART_TX, '\r');
    }

    mmio_write(ctx->regs + UART_TX, buf[i]);
  }

#endif
}

#if NS8250_CONSOLE_USE_INTERRUPTS == 1
static void ns8250_uart_interrupt_handler(void *arg)
{
  char                 input;
  rtems_termios_tty   *tty = arg;
  ns8250_uart_context *ctx = rtems_termios_get_device_context(tty);
  uint32_t             lsr = mmio_read(ctx->regs + UART_LSR);

  if ( ctx->transmitting && lsr & UART_LSR_THRE ) {
    rtems_termios_dequeue_characters(tty, 1);
  }

  if ( lsr & UART_LSR_RDR ) {
    input = mmio_read(ctx->regs + UART_RBR);
    rtems_termios_enqueue_raw_characters(tty, &input, 1);
  }
}
#endif

static int ns8250_uart_poll_read(rtems_termios_device_context *context)
{
  int                  result;
  ns8250_uart_context *ctx = (ns8250_uart_context *) context;

  /* Block until character appears */
  while ( !(mmio_read(ctx->regs + UART_LSR) & UART_LSR_RDR) ) {
    asm volatile("nop");
  }

  /* Save read input to result */
  result = mmio_read(ctx->regs + UART_RBR);

  return result;
}

static bool ns8250_uart_set_attributes(
  rtems_termios_device_context *context,
  const struct termios         *term
)
{
  ns8250_uart_context *ctx = (ns8250_uart_context *) context;
  rtems_termios_baud_t baud;
  unsigned int         lcr;
  uint16_t             divider;

  /* We only have baud rate support yet */
  baud = rtems_termios_baud_to_number(term->c_ospeed);
  divider = BSP_UART_SPEED / (baud * 16);
  lcr = mmio_read(ctx->regs + UART_LCR) | UART_LCR_DLAB;
  mmio_write(ctx->regs + UART_LCR, lcr);

  mmio_write(ctx->regs + UART_DLL, divider & 0xff);
  mmio_write(ctx->regs + UART_DLM, (divider >> 8) & 0xff);
  lcr = UART_LCR_8N1;
  mmio_write(ctx->regs + UART_LCR, lcr);

  return true;
}

static bool ns8250_uart_first_open(
  rtems_termios_tty             *tty,
  rtems_termios_device_context  *context,
  struct termios                *term,
  rtems_libio_open_close_args_t *args
)
{
#if NS8250_CONSOLE_USE_INTERRUPTS == 1
  rtems_status_code status;
#endif
  ns8250_uart_context *ctx = (ns8250_uart_context *) context;

  tegra_car_clear(ctx->car);
  tegra_car_gate(ctx->car, CAR_SOURCE_UART);
  tegra_car_set(ctx->car);

  rtems_termios_set_initial_baud(tty, BSP_UART_DEFAULT_BAUD_RATE);

  if ( !ns8250_uart_set_attributes(context, term) ) {
    return false;
  }

#if NS8250_CONSOLE_USE_INTERRUPTS == 1
  /* Read everything pending */
  mmio_read(ctx->regs + UART_RBR);
  mmio_read(ctx->regs + UART_IIR);
  mmio_read(ctx->regs + UART_LSR);
  mmio_read(ctx->regs + UART_MSR);
  status = rtems_interrupt_handler_install(
    ctx->irq,
    ctx->device_name,
    RTEMS_INTERRUPT_UNIQUE,
    ns8250_uart_interrupt_handler,
    tty
  );
  if ( status != RTEMS_SUCCESSFUL ) {
    return false;
  }

  mmio_write(ctx->regs + UART_LCR, UART_LCR_8N1);
  mmio_write(ctx->regs + UART_IER, UART_IER_IE_RHR);
#endif

  return true;
}

static void ns8250_uart_last_close(
  rtems_termios_tty             *tty,
  rtems_termios_device_context  *context,
  rtems_libio_open_close_args_t *args
)
{
#if NS8250_CONSOLE_USE_INTERRUPTS == 1
  rtems_status_code    status;
  ns8250_uart_context *ctx = (ns8250_uart_context *) context;

  tegra_car_clear(ctx->car);

  status = rtems_interrupt_handler_remove(
    ctx->irq,
    ns8250_uart_interrupt_handler,
    tty
  );

  if ( status != RTEMS_SUCCESSFUL ) {
    rtems_fatal_error_occurred(status);
  }

#endif
}

static const rtems_termios_device_handler ns8250_uart_handler = {
  .first_open = ns8250_uart_first_open,
  .last_close = ns8250_uart_last_close,
  .write = ns8250_uart_write,
  .set_attributes = ns8250_uart_set_attributes,
#if NS8250_CONSOLE_USE_INTERRUPTS == 1
  .mode = TERMIOS_IRQ_DRIVEN,
#else
  .poll_read = ns8250_uart_poll_read,
  .mode = TERMIOS_POLLED
#endif
};

rtems_status_code console_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                     *arg
)
{
  rtems_status_code status;

  rtems_termios_initialize();
  unsigned int i = 0;
  int          err = 0;

  for ( i = 0; i < RTEMS_ARRAY_SIZE(ns8250_uart_instances); i++ ) {
    status = rtems_termios_device_install(
      ns8250_uart_instances[i].device_name,
      &ns8250_uart_handler,
      NULL,
      &ns8250_uart_instances[i].base
    );

    if ( status != RTEMS_SUCCESSFUL ) {
      rtems_fatal_error_occurred(status);
    }
  }

  err = link(ns8250_uart_instances[0].device_name, CONSOLE_DEVICE_NAME);

  if ( err ) {
    printk("link: %d", err);
  }

  return RTEMS_SUCCESSFUL;
}
