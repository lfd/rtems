/*
 * Copyright (c) OTH Regensburg, 2017
 *   Author: Andreas Kölbl
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <stdint.h>
#include <bsp/car.h>
#include <bsp/memory.h>
#include <rtems/score/basedefs.h>

#define CLK_ENB_L_SET (CAR + 0x320)
#define CLK_ENB_L_CLR (CAR + 0x324)
#define CLK_ENB_U_SET (CAR + 0x330)
#define CLK_ENB_U_CLR (CAR + 0x334)
#define CLK_SOURCE_UARTA (CAR + 0x178)
#define CLK_SOURCE_UARTD (CAR + 0x1c0)

#define RST_DEV_L_SET (CAR + 0x300)
#define RST_DEV_L_CLR (CAR + 0x304)
#define RST_DEV_U_SET (CAR + 0x310)
#define RST_DEV_U_CLR (CAR + 0x314)

#define GATE_UARTA (1 << 6)
#define GATE_UARTD (1 << 0)

typedef struct {
  void *enb_set;
  void *enb_clear;
} clock_dev;

typedef struct {
  void *enb_set;
  void *enb_clear;
} reset_dev;

typedef const struct {
  clock_dev clock;
  reset_dev reset;
  void *src_reg;
  uint32_t gate;
} car_dev;

static car_dev tegra124_car_dev[] = {
  {
    .clock = {
      .enb_set = CLK_ENB_U_SET,
      .enb_clear = CLK_ENB_U_CLR,
    },
    .reset = {
      .enb_set = RST_DEV_U_SET,
      .enb_clear = RST_DEV_U_CLR,
    },
    .src_reg = CLK_SOURCE_UARTD,
    .gate = GATE_UARTD,
  },
  {
    .clock = {
      .enb_set = CLK_ENB_L_SET,
      .enb_clear = CLK_ENB_L_CLR,
    },
    .reset = {
      .enb_set = RST_DEV_L_SET,
      .enb_clear = RST_DEV_L_CLR,
    },
    .src_reg = (void *) CLK_SOURCE_UARTA,
    .gate = GATE_UARTA,
  },
};

int tegra_car_set(uint32_t device_num)
{
  if (device_num > RTEMS_ARRAY_SIZE(tegra124_car_dev)) {
    return -1;
  }

  mmio_write(tegra124_car_dev[device_num].clock.enb_set,
    tegra124_car_dev[device_num].gate);

  return 0;
}

int tegra_car_gate(uint32_t device_num, uint32_t car_source)
{
  uint32_t source;

  if ((device_num > RTEMS_ARRAY_SIZE(tegra124_car_dev)) ||
    (car_source > CAR_GATE_MAX)) {
    return -1;
  }

  source = mmio_read(tegra124_car_dev[device_num].src_reg);
  source &= ~(7 << CAR_GATE_OFFSET);
  source |= car_source << CAR_GATE_OFFSET;

  mmio_write(tegra124_car_dev[device_num].src_reg, source);

  return 0;
}

int tegra_car_clear(uint32_t device_num)
{
  if (device_num > RTEMS_ARRAY_SIZE(tegra124_car_dev)) {
    return -1;
  }

  mmio_write(
    tegra124_car_dev[device_num].reset.enb_clear,
    mmio_read(tegra124_car_dev[device_num].reset.enb_clear) |
    tegra124_car_dev[device_num].gate
  );

  return 0;
}
