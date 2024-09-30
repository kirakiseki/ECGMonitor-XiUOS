/**
 * @file HashTable.h
 * @brief HashTable
 * @version 0.1
 * @author SYS Lab
 * @date 2022.11.01
 */

#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdint.h>

#include <algorithm>
#include <vector>
#include <iostream>
#include <string>

#include "Common.h"
#include "HashBucket.h"

namespace LightTable {

class HashTable {
 public:
  HashTable();

  ~HashTable();

  uint64_t getMapKey(const char *primaryKey);

  uint32_t hashToBucket(uint64_t key);

  uint32_t getRowLocation(uint32_t bucketID, uint64_t key,
                          RowLocation &rowLocation);

  uint32_t addItem(uint32_t bucketID, uint64_t key, RowLocation rowLocation);

  uint32_t deleteItem(uint32_t bucketID, uint64_t key);

  uint32_t updateItem(uint32_t bucketID, uint64_t key, RowLocation rowLocation);

  uint64_t getItemCount();

  uint32_t getAllBuckets(std::vector<HashBucket *> &buckets);
  uint32_t refreshRowLocations(uint64_t blockID, uint64_t deleteOffset,
                               uint64_t rowSize,
                               std::map<uint64_t, RowLocation> &locationItems);

  uint32_t clear();

 private:
  std::vector<HashBucket *> buckets;

  DISALLOW_COPY_AND_ASSIGN(HashTable);

 public:
  friend class MetaHandle;
};

}  // namespace LightTable

#endif  // HASHTABLE_H
