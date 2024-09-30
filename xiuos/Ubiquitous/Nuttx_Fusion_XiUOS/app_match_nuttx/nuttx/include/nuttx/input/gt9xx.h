/*
 * Copyright (c) 2022 AIIT XUOS Lab
 * XiUOS is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *        http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */

/**
 * @file gt9xx.h
 * @brief API for gt9xx.
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022.5.31
 */


#ifndef __GT9XX_H_
#define __GT9XX_H_

#define GT9XX_MAX_TOUCHES                (1)
#define GT9XX_TOUCH_DATA_LEN             (8 * GT9XX_MAX_TOUCHES + 4)

#define TOUCH_POINT_GET_EVENT(t)          ((t).xh >> 6)
#define TOUCH_POINT_GET_ID(t)             ((t).yh >> 4)
#define TOUCH_POINT_GET_X(t)              ((((t).xh & 0x0f) << 8) | (t).xl)
#define TOUCH_POINT_GET_Y(t)              ((((t).yh & 0x0f) << 8) | (t).yl)


#define GT9XX_MAX_HEIGHT         272
#define GT9XX_MAX_WIDTH          480
#define GT9XX_INT_TRIGGER        0

#define GT9XX_ADDR_LENGTH        2
#define GT9XX_CONFIG_MIN_LENGTH  186
#define GT9XX_CONFIG_MAX_LENGTH  240

#define GT9XX_REG_BAK_REF        0x99D0
#define GT9XX_REG_CHIP_TYPE      0x8000
#define GT9XX_REG_MAIN_CLK       0x8020
#define GT9XX_REG_COMMAND        0x8040
#define GT9XX_REG_CONFIG_DATA    0x8047
#define GT9XX_REG_CONFIG_CHKSUM  0x80FF
#define GT9XX_REG_VERSION        0x8140
#define GT9XX_REG_SENSOR_ID      0x814A
#define GT9XX_REG_READ_COOR      0x814E

#define GT9XX_COMMAND_READSTATUS 0
#define GT9XX_COMMAND_DIFFERENCE 1
#define GT9XX_COMMAND_SOFTRESET  2
#define GT9XX_COMMAND_UPDATE     3
#define GT9XX_COMMAND_CALCULATE  4
#define GT9XX_COMMAND_TURNOFF    5

enum touch_event_e
{
  GT9XX_DOWN    = 0,  /* The state changed to touched */
  GT9XX_UP      = 1,  /* The state changed to not touched */
  GT9XX_CONTACT = 2,  /* There is a continuous touch being detected */
  GT9XX_INVALID = 3   /* No touch information available */
};

/* Describes on touchpoint returned by the GT9xx */

struct gt9xx_touch_point_s
{
  uint8_t xh;
  uint8_t xl;
  uint8_t yh;
  uint8_t yl;
  uint8_t weight;
  uint8_t area;
};

/* Describes all touch data returned by the GT9xx */

struct gt9xx_touch_data_s
{
  uint8_t tdstatus;    /* Touch status */
  struct gt9xx_touch_point_s touch[GT9XX_MAX_TOUCHES];
};

/* Maximum number of threads than can be waiting for POLL events */

#ifndef CONFIG_GT9XX_NPOLLWAITERS
#  define CONFIG_GT9XX_NPOLLWAITERS 2
#endif

struct gt9xx_config_s
{
  uint8_t  address;    /* 7-bit I2C address (only bits 0-6 used) */
  uint32_t frequency;  /* Default I2C frequency */

#ifndef CONFIG_GT9XX_POLLMODE
  int  (*attach)(FAR const struct gt9xx_config_s *config, xcpt_t isr,
                 FAR void *arg);
  void (*enable)(FAR const struct gt9xx_config_s *config, bool enable);
  void (*clear)(FAR const struct gt9xx_config_s *config);
#endif
  void (*wakeup)(FAR const struct gt9xx_config_s *config);
  void (*nreset)(FAR const struct gt9xx_config_s *config,
                 bool state);
};

int gt9xx_register(FAR struct i2c_master_s *i2c,
                    FAR const struct gt9xx_config_s *config, int minor);


#endif /* __GT9XX_H__ */
