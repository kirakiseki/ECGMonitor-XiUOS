/**
 * @file HashBucket.h
 * @brief HashBucket
 * @version 0.1
 * @author SYS Lab
 * @date 2022.11.01
 */

#ifndef HASHBUCKET_H
#define HASHBUCKET_H

#include <stdint.h>
// #include <pthread.h>

#include <algorithm>
#include <map>
#include <iostream>
#include <utility>
// #include <mutex>

#include "Common.h"
#include "Error.h"

namespace LightTable {

typedef struct rowLocation {
  uint64_t blockID;
  uint64_t rowOffset;
} RowLocation;

class HashBucket {
 public:
  HashBucket();

  uint32_t addBucketItem(uint64_t key, RowLocation rowLocation);

  uint32_t deleteBucketItem(uint64_t key);

  uint32_t updateRowLocation(uint64_t key, RowLocation rowLocation);

  uint32_t getRowLocation(uint64_t key, RowLocation &rowLocation);

  uint64_t getBucketSize();

  uint32_t setBucketSize(uint64_t bucketSize);

  uint32_t getAllBucketItems(std::map<uint64_t, RowLocation> &bucketItems);

 private:
  std::map<uint64_t, RowLocation> bucketItems;
  uint64_t bucketSize;
  // pthread_mutex_t mtx;

  DISALLOW_COPY_AND_ASSIGN(HashBucket);
};

}  // namespace LightTable

#endif  // HASHBUCKET_H
