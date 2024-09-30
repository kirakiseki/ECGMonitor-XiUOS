/****************************************************************************
 * boards/arm/hc32/hc32f4a0/src/hc32f4a0_bringup.c
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

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdbool.h>
#include <stdio.h>
#include <debug.h>
#include <errno.h>

#include <nuttx/fs/fs.h>

#include <hc32_common.h>

/****************************************************************************
 * Name: hc32_bringup
 *
 * Description:
 *   Perform architecture-specific initialization
 *
 *   CONFIG_BOARD_LATE_INITIALIZE=y :
 *     Called from board_late_initialize().
 *
 *   CONFIG_BOARD_LATE_INITIALIZE=n && CONFIG_BOARDCTL=y :
 *     Called from the NSH library
 *
 ****************************************************************************/

int hc32_bringup(void)
{
  int ret = 0;

#ifdef CONFIG_FS_PROCFS
  /* Mount the procfs file system */

  ret = nx_mount(NULL, HC32_PROCFS_MOUNTPOINT, "procfs", 0, NULL);
  if (ret < 0)
  {
  serr("ERROR: Failed to mount procfs at %s: %d\n",
       HC32_PROCFS_MOUNTPOINT, ret);
  }
#endif

#ifdef CONFIG_HC32_ROMFS
  ret = hc32_romfs_initialize();
  if (ret < 0)
  {
  serr("ERROR: Failed to mount romfs at %s: %d\n",
       CONFIG_HC32_ROMFS_MOUNTPOINT, ret);
  }
#endif

  return ret;
}
