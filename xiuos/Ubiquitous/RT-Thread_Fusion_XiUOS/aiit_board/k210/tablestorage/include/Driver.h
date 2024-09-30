/**
 * @file Driver.h
 * @brief driver
 * @version 0.1
 * @author SYS Lab
 * @date 2022.11.01
 */
#ifndef DRIVER_H
#define DRIVER_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <atomic>
#include <iostream>
#include <thread>

#include "Common.h"
#include "Error.h"

// #include <mutex>
// #include <pthread.h>
// #include <condition_variable>

#ifdef __cplusplus
extern "C" {
#endif
uint32_t MakeTableStorage(const char *device_name);
#ifdef __cplusplus
}
#endif

namespace LightTable {

class Driver {
 public:
  static uint32_t driver_init(const char *device_name);

  static void driver_cleanup(const char *device_name);

  static uint32_t driver_read(uint8_t *buf, uint64_t blockID);

  static uint32_t driver_write(uint8_t *buf, uint64_t blockID);

  static uint32_t read(uint8_t *buf, uint64_t blockID);

  static uint32_t write(uint8_t *buf, uint64_t blockID);

  Driver(){};

#ifdef TABLE_STORAGE_CACHE
  static BlockCache *blkCache;
#endif
};

}  // namespace LightTable

#endif  // DRIVER_H
