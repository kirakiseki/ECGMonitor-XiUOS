/**
 * @file Driver.cc
 * @brief Driver
 * @version 0.1
 * @author SYS Lab
 * @date 2022.11.01
 */

#include "Driver.h"

extern "C" {
  #include "ff.h"
  #include "ffconf.h"

  #include <diskio.h>
  #include <rtdef.h>
}

/* Definitions of physical drive number for each drive */
#define DEV_RAM 0 /* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC 1 /* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB 2 /* Example: Map USB MSD to physical drive 2 */

static rt_device_t disk[FF_VOLUMES] = {0};

namespace LightTable {

uint32_t Driver::write(uint8_t *buf, uint64_t blockID) {
  rt_size_t result;
  rt_device_t device = disk[0];

#ifdef TABLE_STORAGE_CACHE
  // TODO: cache
#endif

  if (rt_device_write(device, blockID, buf, 1) == 1)
    return RES_OK;
  else
    return RES_ERROR;
}

uint32_t Driver::read(uint8_t *buf, uint64_t blockID) {
  rt_size_t result;
  rt_device_t device = disk[0];

#ifdef TABLE_STORAGE_CACHE
  // TODO: cache
#endif

  if (rt_device_read(device, blockID, buf, 1) == 1) {
    return RES_OK;
  }

  return RES_OK;
}

void Driver::driver_cleanup(const char *device_name) {
  /* clean device */
  return;
}

uint32_t Driver::driver_init(const char *device_name) {
  /* init device */
  /* note: initialization of the device is done at the MakeTableStorage*/
  return 0;
}
}  // namespace LightTable

uint32_t MakeTableStorage(const char *device_name) {
  rt_device_t dev_id;

  /* open specific device */
  if (device_name == NULL) {
    /* which is a non-device filesystem mount */
    dev_id = NULL;
  } else if ((dev_id = rt_device_find(device_name)) == NULL) {
    /* no this device */
    rt_set_errno(-ENODEV);
    return -1;
  }

  /* open device, but do not check the status of device */
  if (dev_id != NULL) {
    if (rt_device_open(dev_id, RT_DEVICE_OFLAG_RDWR) != RT_EOK) {
      return -1;
    }
  }

  disk[0] = dev_id;

  return 0;
}
