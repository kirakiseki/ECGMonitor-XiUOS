/****************************************************************************
 * drivers/input/gt9xx.c
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/* References:
 *   Dec. 18, 2012
 */

/* The FT5x06 Series ICs are single-chip capacitive touch panel controller
 * ICs with a built-in 8 bit Micro-controller unit (MCU).  They adopt the
 * mutual capacitance approach, which supports true multi-touch capability.
 * In conjunction with a mutual capacitive touch panel, the FT5x06 have
 * user-friendly input functions, which can be applied on many portable
 * devices, such as cellular phones, MIDs, netbook and notebook personal
 * computers.
 */

/**
 * @file gt9xx.c
 * @brief refer to imxrt_gt9xx.c codes.
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022.5.31
 */

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <sys/types.h>

#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <poll.h>
#include <errno.h>
#include <assert.h>
#include <debug.h>

#include <nuttx/irq.h>
#include <nuttx/kmalloc.h>
#include <nuttx/arch.h>
#include <nuttx/fs/fs.h>
#include <nuttx/i2c/i2c_master.h>
#include <nuttx/semaphore.h>
#include <nuttx/wqueue.h>
#include <nuttx/wdog.h>

#include <nuttx/input/touchscreen.h>
#include <nuttx/input/gt9xx.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Driver support ***********************************************************/

/* This format is used to construct the /dev/input[n] device driver path.  It
 * defined here so that it will be used consistently in all places.
 */

#define DEV_FORMAT     "/dev/input%d"
#define DEV_NAMELEN    16

#define GT9XX_VER_LEN  8
#define GT9XX_INF_LEN  6
#define GT9XX_CFG_LEN  3

/* In polled mode, the polling rate will decrease when there is no touch
 * activity.  These definitions represent the maximum and the minimum
 * polling rates.
 */

#define POLL_MINDELAY  MSEC2TICK(50)
#define POLL_MAXDELAY  MSEC2TICK(200)
#define POLL_INCREMENT MSEC2TICK(10)

#define gt_print printf

/****************************************************************************
 * Private Types
 ****************************************************************************/

/* This structure describes the state of one GT9xx driver instance */

struct gt9xx_dev_s
{
  uint8_t crefs;                            /* Number of times the device
                                             * has been opened */
  uint8_t nwaiters;                         /* Number of threads waiting for
                                             * GT9xx data */
  volatile bool valid;                      /* True:  New, valid touch data
                                             * in touchbuf[] */
#ifdef CONFIG_GT9XX_SINGLEPOINT
  uint8_t lastid;                           /* Last reported touch id */
  uint8_t lastevent;                        /* Last reported event */
  int16_t lastx;                            /* Last reported X position */
  int16_t lasty;                            /* Last reported Y position */
#endif
  sem_t devsem;                             /* Manages exclusive access to
                                             * this structure */
  sem_t waitsem;                            /* Used to wait for the
                                             * availability of data */
  uint32_t frequency;                       /* Current I2C frequency */
#ifdef CONFIG_GT9XX_POLLMODE
  uint32_t delay;                           /* Current poll delay */
#endif

  FAR const struct gt9xx_config_s *config; /* Board configuration data */
  FAR struct i2c_master_s *i2c;             /* Saved I2C driver instance */
  struct work_s work;                       /* Supports the interrupt
                                             * handling "bottom half" */
#ifdef CONFIG_GT9XX_POLLMODE
  struct wdog_s polltimer;                  /* Poll timer */
#endif
  uint8_t touchbuf[GT9XX_TOUCH_DATA_LEN];  /* Raw touch data */

  /* The following is a list if poll structures of threads waiting for
   * driver events. The 'struct pollfd' reference for each open is also
   * retained in the f_priv field of the 'struct file'.
   */

  struct pollfd *fds[CONFIG_GT9XX_NPOLLWAITERS];
};

// support 480 * 272
uint8_t gt9xx_cfg_data[] =  {
    0x5B, 0xE0, 0x01, 0x10, 0x01, 0x0A, 0x0D, 0x00, 0x01, 0x0A,
    0x28, 0x0F, 0x5A, 0x3C, 0x03, 0x05, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x87, 0x28, 0x09,
    0x32, 0x34, 0x0C, 0x08, 0x00, 0x00, 0x00, 0x02, 0x02, 0x1D,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x28, 0x55, 0x94, 0xC5, 0x02, 0x07, 0x00, 0x00, 0x04,
    0x8D, 0x2B, 0x00, 0x80, 0x32, 0x00, 0x75, 0x3A, 0x00, 0x6C,
    0x43, 0x00, 0x64, 0x4F, 0x00, 0x64, 0x00, 0x00, 0x00, 0x00,
    0xF0, 0x4A, 0x3A, 0xFF, 0xFF, 0x27, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E, 0x10, 0x12,
    0x14, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x26, 0x24, 0x22, 0x21, 0x20, 0x1F, 0x1E, 0x1D,
    0x0C, 0x0A, 0x08, 0x06, 0x04, 0x02, 0x00, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x81, 0x01
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static void gt9xx_notify(FAR struct gt9xx_dev_s *priv);
static void gt9xx_data_worker(FAR void *arg);
#ifdef CONFIG_GT9XX_POLLMODE
static void gt9xx_poll_timeout(wdparm_t arg);
#else
static int  gt9xx_data_interrupt(int irq, FAR void *context, FAR void *arg);
#endif
static ssize_t gt9xx_sample(FAR struct gt9xx_dev_s *priv, FAR char *buffer,
                             size_t len);
static ssize_t gt9xx_waitsample(FAR struct gt9xx_dev_s *priv,
                                 FAR char *buffer, size_t len);
static int  gt9xx_bringup(FAR struct gt9xx_dev_s *priv);
static void gt9xx_shutdown(FAR struct gt9xx_dev_s *priv);

/* Character driver methods */

static int  gt9xx_open(FAR struct file *filep);
static int  gt9xx_close(FAR struct file *filep);
static ssize_t gt9xx_read(FAR struct file *filep, FAR char *buffer,
                           size_t len);
static int  gt9xx_ioctl(FAR struct file *filep, int cmd,
                         unsigned long arg);
static int  gt9xx_poll(FAR struct file *filep, struct pollfd *fds,
                        bool setup);

/****************************************************************************
 * Private Data
 ****************************************************************************/

/* This the vtable that supports the character driver interface */

static const struct file_operations gt9xx_fops =
{
  gt9xx_open,    /* open */
  gt9xx_close,   /* close */
  gt9xx_read,    /* read */
  NULL,           /* write */
  NULL,           /* seek */
  gt9xx_ioctl,   /* ioctl */
  gt9xx_poll     /* poll */
#ifndef CONFIG_DISABLE_PSEUDOFS_OPERATIONS
  , NULL          /* unlink */
#endif
};

/* Maps GT9xx touch events into bit encoded representation used by NuttX */

static const uint8_t g_event_map[4] =
{
  (TOUCH_DOWN | TOUCH_ID_VALID | TOUCH_POS_VALID), /* GT9XX_DOWN */
  (TOUCH_UP   | TOUCH_ID_VALID),                   /* GT9XX_UP */
  (TOUCH_MOVE | TOUCH_ID_VALID | TOUCH_POS_VALID), /* GT9XX_CONTACT */
  TOUCH_ID_VALID                                   /* GT9XX_INVALID */
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int gt9xx_write_reg(FAR struct gt9xx_dev_s *priv, uint8_t *reg_val, uint16_t len)
{
    FAR const struct gt9xx_config_s *config;
    struct i2c_msg_s msg[2];
    int ret;
    int retries = 0;

    config = priv->config;
    DEBUGASSERT(config != NULL);

    msg[0].frequency = priv->frequency;
    msg[0].addr      = config->address;
    msg[0].flags     = 0;
    msg[0].buffer    = reg_val;
    msg[0].length    = len;

    while(retries < 5)
    {
        ret = I2C_TRANSFER(priv->i2c, msg, 1);
        if(ret == 2) break;
        retries ++;
    }
    if (ret < 0)
      {
        ierr("gt: [%s] failed freq %ld addr %x ret %d\n", __func__, priv->frequency, config->address, ret);
        return ret;
      }
    return ret;
}

static int gt9xx_read_reg(FAR struct gt9xx_dev_s *priv, uint8_t *reg_addr, uint8_t *reg_val, uint16_t len)
{
    FAR const struct gt9xx_config_s *config;
    struct i2c_msg_s msg[2];
    int ret;
    int retries = 0;

    config = priv->config;
    DEBUGASSERT(config != NULL);

    msg[0].frequency = priv->frequency;
    msg[0].addr      = config->address;
    msg[0].flags     = 0;
    msg[0].buffer    = reg_addr;
    msg[0].length    = GT9XX_ADDR_LENGTH;

    msg[1].frequency = priv->frequency;
    msg[1].addr      = config->address;
    msg[1].flags     = I2C_M_READ;
    msg[1].buffer    = reg_val;
    msg[1].length    = len - GT9XX_ADDR_LENGTH;
    while(retries < 5)
    {
        ret = I2C_TRANSFER(priv->i2c, msg, 2);
        if(ret == 2) break;
        retries ++;
    }
    if (ret < 0)
    {
        ierr("gt: [%s] failed freq %ld addr %x ret %d\n", __func__, priv->frequency, config->address, ret);
        return ret;
    }
    return ret;
}

/****************************************************************************
 * Name: gt9xx_write_config
 ****************************************************************************/

static int gt9xx_write_config(FAR struct gt9xx_dev_s *priv)
{
    int i, ret = -1;
    uint8_t check_sum = 0;
    uint8_t offet = GT9XX_REG_CONFIG_CHKSUM - GT9XX_REG_CONFIG_DATA;

    const uint8_t* cfg_info = gt9xx_cfg_data;
    uint8_t cfg_info_len = sizeof(gt9xx_cfg_data) / sizeof(gt9xx_cfg_data[0]);

    uint8_t reg_data[GT9XX_CONFIG_MAX_LENGTH + GT9XX_ADDR_LENGTH]
                    = {GT9XX_REG_CONFIG_DATA >> 8, GT9XX_REG_CONFIG_DATA & 0xff};

    memset(&reg_data[GT9XX_ADDR_LENGTH], 0, GT9XX_CONFIG_MAX_LENGTH);
    memcpy(&reg_data[GT9XX_ADDR_LENGTH], cfg_info, cfg_info_len);

    for (i = GT9XX_ADDR_LENGTH; i < offet + GT9XX_ADDR_LENGTH; i++)
    {
        check_sum += reg_data[i];
    }

    reg_data[offet + GT9XX_ADDR_LENGTH] = (~check_sum) + 1; //checksum
    reg_data[offet + GT9XX_ADDR_LENGTH + 1] =  1; //refresh

    gt_print("Driver send config.\n");

    ret = gt9xx_write_reg(priv, reg_data, offet + GT9XX_ADDR_LENGTH + 2);

    return ret;
}


/****************************************************************************
 * Name: gt9xx_notify
 ****************************************************************************/

static void gt9xx_notify(FAR struct gt9xx_dev_s *priv)
{
  int i;

  /* If there are threads waiting on poll() for GT9xx data to become
   * available, then wake them up now.  NOTE: we wake up all waiting threads
   * because we do not know that they are going to do.  If they all try to
   * read the data, then some make end up blocking after all.
   */

  for (i = 0; i < CONFIG_GT9XX_NPOLLWAITERS; i++)
    {
      struct pollfd *fds = priv->fds[i];
      if (fds)
        {
          fds->revents |= POLLIN;
          iinfo("Report events: %02x\n", fds->revents);
          nxsem_post(fds->sem);
        }
    }

  /* If there are threads waiting for read data, then signal one of them
   * that the read data is available.
   */

  if (priv->nwaiters > 0)
    {
      /* After posting this semaphore, we need to exit because the GT9xx
       * is no longer available.
       */

      nxsem_post(&priv->waitsem);
    }
}

/****************************************************************************
 * Name: gt9xx_data_worker
 ****************************************************************************/

static void gt9xx_data_worker(FAR void *arg)
{
  FAR struct gt9xx_dev_s *priv = (FAR struct gt9xx_dev_s *)arg;
  FAR const struct gt9xx_config_s *config;
  FAR struct gt9xx_touch_data_s *sample;
  uint8_t regaddr[3];
  int ret;

  /* Get a pointer the callbacks for convenience */

  DEBUGASSERT(priv != NULL && priv->config != NULL);
  config = priv->config;

  /* We need to have exclusive access to the touchbuf so that we do not
   * corrupt any read operation that is in place.
   */
  do
    {
      ret = nxsem_wait_uninterruptible(&priv->devsem);

      /* This would only fail if something canceled the worker thread?
       * That is not expected.
       */

      DEBUGASSERT(ret == OK || ret == -ECANCELED);
    }
  while (ret < 0);

  /* Read touch data */

  /* Set up the address write operation */

  regaddr[0] = GT9XX_REG_READ_COOR >> 8;
  regaddr[1] = GT9XX_REG_READ_COOR & 0xFF;

  ret = gt9xx_read_reg(priv, regaddr, priv->touchbuf, GT9XX_TOUCH_DATA_LEN);

  if (ret >= 0)
    {
      /* In polled mode, we may read invalid touch data.  If there is
       * no touch data, the GT9xx returns all 0xff the very first time.
       * After that, it returns the same old stale data when there is
       * no touch data.
       */

      if(priv->touchbuf[0] & 0xf)
        {
          gt_print("get i2c %x reg %x %x ret %d ", config->address, regaddr[0], regaddr[1], ret);
          for(int i = 0; i < GT9XX_TOUCH_DATA_LEN; i ++)
            {
                gt_print("%x ", priv->touchbuf[i]);
            }
          gt_print("\n");
        }

      sample = (FAR struct gt9xx_touch_data_s *)priv->touchbuf;

      /* Notify waiters (only if we ready some valid data).
       *
       * REVISIT: For improved performance consider moving the duplicate
       * report and thresholding logic from gt9xx_sample() to here.  That
       * would save a context switch.
       */
      sample->tdstatus &= 0xf;

      if (sample->tdstatus <= GT9XX_MAX_TOUCHES)
        {
          /* Notify any waiters that new GT9xx data is available */

          priv->valid = true;
          gt9xx_notify(priv);
        }

#ifdef CONFIG_GT9XX_POLLMODE
      /* Update the poll rate */

      if (sample->tdstatus > 0 && sample->tdstatus <= GT9XX_MAX_TOUCHES)
        {
          /* Keep it at the minimum if touches are detected. */

          priv->delay = POLL_MINDELAY;
        }
      else if (priv->delay < POLL_MAXDELAY)
        {
          /* Otherwise, let the poll rate rise gradually up to the maximum
           * if there is no touch.
           */

          priv->delay += POLL_INCREMENT;
        }
#endif
    }

  /* cmd end */
  gt9xx_write_reg(priv, regaddr, 3);

#ifdef CONFIG_GT9XX_POLLMODE
  /* Exit, re-starting the poll. */

  wd_start(&priv->polltimer, priv->delay,
           gt9xx_poll_timeout, (wdparm_t)priv);

#else
  /* Exit, re-enabling GT9xx interrupts */
  config->enable(config, true);
#endif

  nxsem_post(&priv->devsem);
}

/****************************************************************************
 * Name: gt9xx_poll_timeout
 ****************************************************************************/

#ifdef CONFIG_GT9XX_POLLMODE
static void gt9xx_poll_timeout(wdparm_t arg)
{
  FAR struct gt9xx_dev_s *priv = (FAR struct gt9xx_dev_s *)arg;
  int ret;

  /* Transfer processing to the worker thread.  Since GT9xx poll timer is
   * disabled while the work is pending, no special action should be
   * required to protected the work queue.
   */

  DEBUGASSERT(priv->work.worker == NULL);
  ret = work_queue(HPWORK, &priv->work, gt9xx_data_worker, priv, 0);
  if (ret != 0)
    {
      ierr("ERROR: Failed to queue work: %d\n", ret);
    }
}
#endif

/****************************************************************************
 * Name: gt9xx_data_interrupt
 ****************************************************************************/

#ifndef CONFIG_GT9XX_POLLMODE
static int gt9xx_data_interrupt(int irq, FAR void *context, FAR void *arg)
{
  FAR struct gt9xx_dev_s *priv = (FAR struct gt9xx_dev_s *)arg;
  FAR const struct gt9xx_config_s *config;
  int ret;

  /* Get a pointer the callbacks for convenience (and so the code is not so
   * ugly).
   */

  config = priv->config;
  DEBUGASSERT(config != NULL);

  /* Disable further interrupts */

  config->enable(config, false);

  /* Transfer processing to the worker thread.  Since GT9xx interrupts are
   * disabled while the work is pending, no special action should be required
   * to protected the work queue.
   */

  DEBUGASSERT(priv->work.worker == NULL);
  ret = work_queue(HPWORK, &priv->work, gt9xx_data_worker, priv, 0);
  if (ret != 0)
    {
      ierr("ERROR: Failed to queue work: %d\n", ret);
    }

  /* Clear any pending interrupts and return success */

  config->clear(config);
  return OK;
}
#endif

/****************************************************************************
 * Name: gt9xx_sample
 ****************************************************************************/

#ifdef CONFIG_GT9XX_SINGLEPOINT
static ssize_t gt9xx_sample(FAR struct gt9xx_dev_s *priv, FAR char *buffer,
                             size_t len)
{
  FAR struct gt9xx_touch_data_s *raw;
  FAR struct gt9xx_touch_point_s *touch;
  FAR struct touch_sample_s *sample;
  FAR struct touch_point_s *point;
  int16_t x;
  int16_t y;
  uint8_t event;
  uint8_t id;
  uint8_t finger;

  if (!priv->valid)
    {
      return 0;  /* Nothing to read */
    }

  /* Raw data pointers (source) */

  raw = (FAR struct gt9xx_touch_data_s *)priv->touchbuf;
  touch = raw->touch;

  finger = priv->touchbuf[0];

  if(finger == 0x0)
    {
      goto reset_and_drop;
    }

  if((finger & 0x80) == 0)
    {
      goto reset_and_drop;
    }

  if((finger & 0xf) == 0)
    {
      goto reset_and_drop;
    }

  /* Get the reported X and Y positions */
#ifdef CONFIG_GT9XX_SWAPXY
  y = TOUCH_POINT_GET_X(touch[0]);
  x = TOUCH_POINT_GET_Y(touch[0]);
#else
  x = TOUCH_POINT_GET_X(touch[0]);
  y = TOUCH_POINT_GET_Y(touch[0]);
#endif

  /* Get the touch point ID and event */

  event = GT9XX_DOWN;
  id    = TOUCH_POINT_GET_ID(touch[0]);

  if (event == GT9XX_INVALID)
    {
      priv->lastevent = GT9XX_INVALID;
      goto reset_and_drop;
    }

  gt_print("gt: [%s] event %d id %d\n", __func__, event, id);

  if (id == priv->lastid && event == priv->lastevent)
    {
      /* Same ID and event..  Is there positional data? */

      if (raw->tdstatus == 0 || event == GT9XX_UP)
        {
          /* No... no new touch data */

          goto reset_and_drop;
        }
      else
        {
          int16_t deltax;
          int16_t deltay;

          /* Compare the change in position from the last report. */

          deltax = (x - priv->lastx);
          if (deltax < 0)
            {
              deltax = -deltax;
            }

          if (deltax < CONFIG_GT9XX_THRESHX)
            {
              /* There as been no significant change in X, try Y */

              deltay = (y - priv->lasty);
              if (deltay < 0)
                {
                  deltay = -deltay;
                }

              if (deltax < CONFIG_GT9XX_THRESHX)
                {
                  /* Ignore... no significant change in Y either */

                  goto drop;
                }
            }
        }
    }

  priv->lastid      = id;
  priv->lastevent   = event;
  priv->lastx       = x;
  priv->lasty       = y;

  /* User data buffer points (sink) */

  /* Return the number of touches read */

  sample            = (FAR struct touch_sample_s *)buffer;
  sample->npoints   = 1;

  /* Decode and return the single touch point */

  point             = sample->point;
  point[0].id       = id;
  point[0].flags    = g_event_map[event];
  point[0].x        = x;
  point[0].y        = y;
  point[0].h        = 0;
  point[0].w        = 0;
  point[0].pressure = 0;

  priv->valid       = false;
  return SIZEOF_TOUCH_SAMPLE_S(1);

reset_and_drop:
  priv->lastx = 0;
  priv->lasty = 0;
drop:
  priv->valid = false;
  return 0;  /* No new touches read. */
}
#else
static ssize_t gt9xx_sample(FAR struct gt9xx_dev_s *priv, FAR char *buffer,
                             size_t len)
{
  FAR struct gt9xx_touch_data_s *raw;
  FAR struct gt9xx_touch_point_s *touch;
  FAR struct touch_sample_s *sample;
  FAR struct touch_point_s *point;
  unsigned int maxtouches;
  unsigned int ntouches;
  int i;

  maxtouches = (len - sizeof(int)) / sizeof(struct touch_point_s);
  DEBUGASSERT(maxtouches > 0);  /* Already verified */

  if (!priv->valid)
    {
      return 0;  /* Nothing to read */
    }

  /* Raw data pointers (source) */

  raw      = (FAR struct gt9xx_touch_data_s *)priv->touchbuf;
  touch    = raw->touch;

  /* Decode number of touches */

  ntouches = raw->tdstatus;
  DEBUGASSERT(ntouches <= GT9XX_MAX_TOUCHES);

  if (ntouches > maxtouches)
    {
      ntouches = maxtouches;
    }

  if (ntouches < 1)
    {
      priv->valid = false;
      return 0;  /* No touches read. */
    }

  /* User data buffer points (sink) */

  sample = (FAR struct touch_sample_s *)buffer;
  point  = sample->point;

  /* Return the number of touches read */

  sample->npoints = ntouches;

  /* Decode and return the touch points */

  for (i = 0; i < ntouches; i++)
    {
      int event         = TOUCH_POINT_GET_EVENT(touch[i]);

      point[i].id       = TOUCH_POINT_GET_ID(touch[i]);
      point[i].flags    = g_event_map[event];
#ifdef CONFIG_GT9XX_SWAPXY
      point[i].y        = TOUCH_POINT_GET_X(touch[i]);
      point[i].x        = TOUCH_POINT_GET_Y(touch[i]);
#else
      point[i].x        = TOUCH_POINT_GET_X(touch[i]);
      point[i].y        = TOUCH_POINT_GET_Y(touch[i]);
#endif
      point[i].h        = 0;
      point[i].w        = 0;
      point[i].pressure = 0;

//      gt_print("gt: [%s] %d - touch %x %x %x %x id %d flags %x x %d y %d\n", __func__,
//        i,
//        touch[i].xl, touch[i].xh, touch[i].yl, touch[i].yh,
//        point[i].id, point[i].flags,
//        point[i].x, point[i].y);

    }

  priv->valid = false;
  return SIZEOF_TOUCH_SAMPLE_S(ntouches);
}
#endif /* CONFIG_GT9XX_SINGLEPOINT */

/****************************************************************************
 * Name: gt9xx_waitsample
 ****************************************************************************/

static ssize_t gt9xx_waitsample(FAR struct gt9xx_dev_s *priv,
                                 FAR char *buffer, size_t len)
{
  int ret;

  /* Disable pre-emption to prevent other threads from getting control while
   * we muck with the semaphores.
   */

  sched_lock();

  /* Now release the semaphore that manages mutually exclusive access to
   * the device structure.  This may cause other tasks to become ready to
   * run, but they cannot run yet because pre-emption is disabled.
   */

  nxsem_post(&priv->devsem);

  /* Try to get the a sample... if we cannot, then wait on the semaphore
   * that is posted when new sample data is available.
   */

  while (!priv->valid)
    {
      /* Increment the count of waiters */

      priv->nwaiters++;

      /* Wait for a change in the GT9xx state */

      ret = nxsem_wait(&priv->waitsem);
      priv->nwaiters--;

      if (ret < 0)
        {
          ierr("ERROR: nxsem_wait failed: %d\n", ret);
          goto errout;
        }
    }

  /* Re-acquire the semaphore that manages mutually exclusive access to
   * the device structure.  We may have to wait here.  But we have our
   * sample.  Interrupts and pre-emption will be re-enabled while we wait.
   */

  ret = nxsem_wait(&priv->devsem);
  if (ret >= 0)
    {
      /* Now sample the data.
       *
       * REVISIT:  Is it safe to assume that priv->valid will always be
       * true?  I think that sched_lock() would protect the setting.
       */

      ret = gt9xx_sample(priv, buffer, len);
    }

errout:
  /* Restore pre-emption.  We might get suspended here but that is okay
   * because we already have our sample.  Note:  this means that if there
   * were two threads reading from the GT9xx for some reason, the data
   * might be read out of order.
   */

  sched_unlock();
  return ret;
}

int gt9xx_get_version(FAR struct gt9xx_dev_s *priv)
{
  FAR const struct gt9xx_config_s *config;
  int ret;
  uint8_t reg_addr[2];
  uint8_t reg_val[GT9XX_VER_LEN] = {0};

  config = priv->config;
  DEBUGASSERT(config != NULL);

  reg_addr[0] = (GT9XX_REG_VERSION) >> 8;   /* config address */
  reg_addr[1] = (GT9XX_REG_VERSION) & 0xFF;

  ret = gt9xx_read_reg(priv, reg_addr, reg_val, GT9XX_VER_LEN);

  if (ret < 0)
    {
      ierr("gt: [%s] failed freq %ld addr %x ret %d\n", __func__, priv->frequency, config->address, ret);
      return ret;
    }

  gt_print("IC version: %s_%x%x\n", reg_val, reg_val[4], reg_val[5]);

  return OK;
}

int gt9xx_get_info(FAR struct gt9xx_dev_s *priv)
{
  FAR const struct gt9xx_config_s *config;
  int ret;
  uint8_t reg_addr[2];
  uint8_t reg_val[GT9XX_INF_LEN] = {0};
  uint16_t abs_x_max = GT9XX_MAX_WIDTH;
  uint16_t abs_y_max = GT9XX_MAX_HEIGHT;
  uint8_t int_trigger_type = GT9XX_INT_TRIGGER;

  config = priv->config;
  DEBUGASSERT(config != NULL);

  reg_addr[0] = (GT9XX_REG_CONFIG_DATA + 1) >> 8;   /* config address */
  reg_addr[1] = (GT9XX_REG_CONFIG_DATA + 1) & 0xFF;

  ret = gt9xx_read_reg(priv, reg_addr, reg_val, GT9XX_INF_LEN);

  if (ret < 0)
    {
      ierr("gt: [%s] failed freq %ld addr %x ret %d\n", __func__, priv->frequency, config->address, ret);
      return ret;
    }

  abs_x_max = (reg_val[1] << 8) + reg_val[0];
  abs_y_max = (reg_val[3] << 8) + reg_val[2];

  reg_addr[0] = (GT9XX_REG_CONFIG_DATA + 6) >> 8;   /* config address */
  reg_addr[1] = (GT9XX_REG_CONFIG_DATA + 6) & 0xFF;

  ret = gt9xx_read_reg(priv, reg_addr, reg_val, GT9XX_CFG_LEN);

  if (ret < 0)
    {
      ierr("gt: [%s] failed freq %ld addr %x ret %d\n", __func__, priv->frequency, config->address, ret);
      return ret;
    }

  int_trigger_type = reg_val[0] & 0x03;

  iinfo("X_MAX = %d, Y_MAX = %d, TRIGGER = 0x%02x\n",
          abs_x_max, abs_y_max, int_trigger_type);

  return OK;
}


/****************************************************************************
 * Name: gt9xx_bringup
 ****************************************************************************/

static int gt9xx_bringup(FAR struct gt9xx_dev_s *priv)
{
  FAR const struct gt9xx_config_s *config;
  config = priv->config;
  DEBUGASSERT(config != NULL);

#ifndef CONFIG_GT9XX_POLLMODE
  /* Enable GT9xx interrupts */
  config->clear(config);
  config->enable(config, true);
#endif
  gt9xx_get_version(priv);
  gt9xx_get_info(priv);
  return OK;
}

/****************************************************************************
 * Name: gt9xx_shutdown
 ****************************************************************************/

static void gt9xx_shutdown(FAR struct gt9xx_dev_s *priv)
{
#ifdef CONFIG_GT9XX_POLLMODE
  /* Stop the poll timer */

  wd_cancel(&priv->polltimer);

#else
  FAR const struct gt9xx_config_s *config = priv->config;

  /* Make sure that the GT9xx interrupt is disabled */

  config->clear(config);
  config->enable(config, false);
#endif
}

/****************************************************************************
 * Name: gt9xx_open
 ****************************************************************************/

static int gt9xx_open(FAR struct file *filep)
{
  FAR struct inode *inode;
  FAR struct gt9xx_dev_s *priv;
  uint8_t tmp;
  int ret;

  DEBUGASSERT(filep);
  inode = filep->f_inode;

  DEBUGASSERT(inode && inode->i_private);
  priv  = (FAR struct gt9xx_dev_s *)inode->i_private;

  /* Get exclusive access to the driver data structure */

  ret = nxsem_wait(&priv->devsem);
  if (ret < 0)
    {
      ierr("ERROR: nxsem_wait failed: %d\n", ret);
      return ret;
    }

  /* Increment the reference count */

  tmp = priv->crefs + 1;
  if (tmp == 0)
    {
      /* More than 255 opens; uint8_t overflows to zero */

      ret = -EMFILE;
      goto errout_with_sem;
    }

  /* When the reference increments to 1, this is the first open event
   * on the driver.. and the time when we must initialize the driver.
   */

  if (tmp == 1)
    {
      ret = gt9xx_bringup(priv);
      if (ret < 0)
        {
          ierr("ERROR: gt9xx_bringup failed: %d\n", ret);
          goto errout_with_sem;
        }

      ret = gt9xx_write_config(priv);
      if (ret < 0)
        {
          ierr("ERROR: gt9xx_write_config failed: %d\n", ret);
          goto errout_with_sem;
        }
    }

  /* Save the new open count on success */

  priv->crefs = tmp;

errout_with_sem:
  nxsem_post(&priv->devsem);
  return ret;
}

/****************************************************************************
 * Name: gt9xx_close
 ****************************************************************************/

static int gt9xx_close(FAR struct file *filep)
{
  FAR struct inode *inode;
  FAR struct gt9xx_dev_s *priv;
  int ret;

  DEBUGASSERT(filep);
  inode = filep->f_inode;

  DEBUGASSERT(inode && inode->i_private);
  priv  = (FAR struct gt9xx_dev_s *)inode->i_private;

  /* Get exclusive access to the driver data structure */

  ret = nxsem_wait(&priv->devsem);
  if (ret < 0)
    {
      ierr("ERROR: nxsem_wait failed: %d\n", ret);
      return ret;
    }

  /* Decrement the reference count unless it would decrement a negative
   * value.
   */

  if (priv->crefs >= 1)
    {
      priv->crefs--;
    }

  /* When the count decrements to zero, there are no further open references
   * to the driver and it can be uninitialized.
   */

  if (priv->crefs == 0)
    {
      gt9xx_shutdown(priv);
    }

  nxsem_post(&priv->devsem);
  return OK;
}

/****************************************************************************
 * Name: gt9xx_read
 ****************************************************************************/

static ssize_t gt9xx_read(FAR struct file *filep, FAR char *buffer,
                           size_t len)
{
  FAR struct inode *inode;
  FAR struct gt9xx_dev_s  *priv;
  int ret;

  DEBUGASSERT(filep);
  inode = filep->f_inode;

  DEBUGASSERT(inode && inode->i_private);
  priv  = (FAR struct gt9xx_dev_s *)inode->i_private;

  /* Verify that the caller has provided a buffer large enough to receive
   * the touch data.
   */

  if (len < SIZEOF_TOUCH_SAMPLE_S(1))
    {
      /* We could provide logic to break up a touch report into segments and
       * handle smaller reads... but why?
       */

      return -ENOSYS;
    }

  /* Get exclusive access to the driver data structure */

  ret = nxsem_wait(&priv->devsem);
  if (ret < 0)
    {
      ierr("ERROR: nxsem_wait failed: %d\n", ret);
      return ret;
    }

  /* Try to read sample data. */

  ret = gt9xx_sample(priv, buffer, len);
  while (ret == 0)
    {
      /* Sample data is not available now.  We would have to wait to receive
       * sample data.  If the user has specified the O_NONBLOCK option, then
       * just return an error.
       */

      if (filep->f_oflags & O_NONBLOCK)
        {
          ret = -EAGAIN;
          goto errout;
        }

      /* Wait for sample data */

      ret = gt9xx_waitsample(priv, buffer, len);
      if (ret < 0)
        {
          /* We might have been awakened by a signal */

          goto errout;
        }
    }

  ret = SIZEOF_TOUCH_SAMPLE_S(1);

errout:
  nxsem_post(&priv->devsem);
  return ret;
}

/****************************************************************************
 * Name: gt9xx_ioctl
 ****************************************************************************/

static int gt9xx_ioctl(FAR struct file *filep, int cmd, unsigned long arg)
{
  FAR struct inode         *inode;
  FAR struct gt9xx_dev_s *priv;
  int                       ret;

  iinfo("cmd: %d arg: %ld\n", cmd, arg);
  DEBUGASSERT(filep);
  inode = filep->f_inode;

  DEBUGASSERT(inode && inode->i_private);
  priv  = (FAR struct gt9xx_dev_s *)inode->i_private;

  /* Get exclusive access to the driver data structure */

  ret = nxsem_wait(&priv->devsem);
  if (ret < 0)
    {
      ierr("ERROR: nxsem_wait failed: %d\n", ret);
      return ret;
    }

  /* Process the IOCTL by command */

  switch (cmd)
    {
      case TSIOC_SETFREQUENCY:  /* arg: Pointer to uint32_t frequency value */
        {
          FAR uint32_t *ptr = (FAR uint32_t *)((uintptr_t)arg);
          DEBUGASSERT(priv->config != NULL && ptr != NULL);
          priv->frequency = *ptr;
        }
        break;

      case TSIOC_GETFREQUENCY:  /* arg: Pointer to uint32_t frequency value */
        {
          FAR uint32_t *ptr = (FAR uint32_t *)((uintptr_t)arg);
          DEBUGASSERT(priv->config != NULL && ptr != NULL);
          *ptr = priv->frequency;
        }
        break;

      default:
        ret = -ENOTTY;
        break;
    }

  nxsem_post(&priv->devsem);
  return ret;
}

/****************************************************************************
 * Name: gt9xx_poll
 ****************************************************************************/

static int gt9xx_poll(FAR struct file *filep, FAR struct pollfd *fds,
                        bool setup)
{
  FAR struct inode         *inode;
  FAR struct gt9xx_dev_s *priv;
  int                       ret;
  int                       i;

  iinfo("setup: %d\n", (int)setup);
  DEBUGASSERT(filep && fds);
  inode = filep->f_inode;

  DEBUGASSERT(inode && inode->i_private);
  priv  = (FAR struct gt9xx_dev_s *)inode->i_private;

  /* Are we setting up the poll?  Or tearing it down? */

  ret = nxsem_wait(&priv->devsem);
  if (ret < 0)
    {
      ierr("ERROR: nxsem_wait failed: %d\n", ret);
      return ret;
    }

  if (setup)
    {
      /* Ignore waits that do not include POLLIN */

      if ((fds->events & POLLIN) == 0)
        {
          ierr("ERROR: Missing POLLIN: revents: %08x\n", fds->revents);
          ret = -EDEADLK;
          goto errout;
        }

      /* This is a request to set up the poll.  Find an available
       * slot for the poll structure reference
       */

      for (i = 0; i < CONFIG_GT9XX_NPOLLWAITERS; i++)
        {
          /* Find an available slot */

          if (!priv->fds[i])
            {
              /* Bind the poll structure and this slot */

              priv->fds[i] = fds;
              fds->priv    = &priv->fds[i];
              break;
            }
        }

      if (i >= CONFIG_GT9XX_NPOLLWAITERS)
        {
          ierr("ERROR: No available slot found: %d\n", i);
          fds->priv    = NULL;
          ret          = -EBUSY;
          goto errout;
        }

      /* Should we immediately notify on any of the requested events? */

      if (priv->valid)
        {
          gt9xx_notify(priv);
        }
    }
  else if (fds->priv)
    {
      /* This is a request to tear down the poll. */

      struct pollfd **slot = (struct pollfd **)fds->priv;
      DEBUGASSERT(slot != NULL);

      /* Remove all memory of the poll setup */

      *slot                = NULL;
      fds->priv            = NULL;
    }

errout:
  nxsem_post(&priv->devsem);
  return ret;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: gt9xx_register
 *
 * Description:
 *   Configure the GT9xx to use the provided I2C device instance.  This
 *   will register the driver as /dev/inputN where N is the minor device
 *   number
 *
 * Input Parameters:
 *   dev     - An I2C driver instance
 *   config  - Persistent board configuration data
 *   minor   - The input device minor number
 *
 * Returned Value:
 *   Zero is returned on success.  Otherwise, a negated errno value is
 *   returned to indicate the nature of the failure.
 *
 ****************************************************************************/

int gt9xx_register(FAR struct i2c_master_s *i2c,
                    FAR const struct gt9xx_config_s *config, int minor)
{
  FAR struct gt9xx_dev_s *priv;
  char devname[DEV_NAMELEN];
  int ret;

  iinfo("i2c: %p minor: %d\n", i2c, minor);

  /* Debug-only sanity checks */

  DEBUGASSERT(i2c != NULL && config != NULL && minor >= 0 && minor < 100);
#ifdef CONFIG_GT9XX_POLLMODE
  DEBUGASSERT(config->wakeup != NULL && config->nreset != NULL);
#else
  DEBUGASSERT(config->attach != NULL && config->enable != NULL &&
              config->clear  != NULL && config->wakeup != NULL &&
              config->nreset != NULL);
#endif

  /* Create and initialize a GT9xx device driver instance */

  priv = (FAR struct gt9xx_dev_s *)kmm_zalloc(sizeof(struct gt9xx_dev_s));
  if (!priv)
    {
      ierr("ERROR: kmm_zalloc(%d) failed\n", sizeof(struct gt9xx_dev_s));
      return -ENOMEM;
    }

  /* Initialize the GT9xx device driver instance */

  priv->i2c       = i2c;               /* Save the I2C device handle */
  priv->config    = config;            /* Save the board configuration */
  priv->frequency = config->frequency; /* Set the current I2C frequency */

  nxsem_init(&priv->devsem,  0, 1);    /* Initialize device structure semaphore */
  nxsem_init(&priv->waitsem, 0, 0);    /* Initialize pen event wait semaphore */

  /* The event wait semaphore is used for signaling and, hence, should not
   * have priority inheritance enabled.
   */

  nxsem_set_protocol(&priv->waitsem, SEM_PRIO_NONE);

#ifdef CONFIG_GT9XX_POLLMODE
  /* Allocate a timer for polling the GT9xx */
  priv->delay     = POLL_MAXDELAY;
#else
  /* Make sure that the GT9xx interrupt interrupt is disabled */

  config->clear(config);
  config->enable(config, false);

  /* Attach the interrupt handler */

  ret = config->attach(config, gt9xx_data_interrupt,
                       priv);
  if (ret < 0)
    {
      ierr("ERROR: Failed to attach interrupt\n");
      goto errout_with_priv;
    }
#endif

  /* Register the device as an input device */

  snprintf(devname, DEV_NAMELEN, DEV_FORMAT, minor);
  iinfo("Registering %s\n", devname);

  ret = register_driver(devname, &gt9xx_fops, 0666, priv);
  if (ret < 0)
    {
      ierr("ERROR: register_driver() failed: %d\n", ret);
      goto errout_with_priv;
    }

  /* Schedule work to perform the initial sampling and to set the data
   * availability conditions.
   */

  ret = work_queue(HPWORK, &priv->work, gt9xx_data_worker, priv, 0);
  if (ret < 0)
    {
      ierr("ERROR: Failed to queue work: %d\n", ret);
      goto errout_with_priv;
    }

  /* And return success */

  return OK;

errout_with_priv:
  nxsem_destroy(&priv->devsem);
  kmm_free(priv);
  return ret;
}
