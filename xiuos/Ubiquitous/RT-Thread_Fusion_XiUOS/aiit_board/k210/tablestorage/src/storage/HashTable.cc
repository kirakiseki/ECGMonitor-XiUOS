/**
 * @file HashTable.cc
 * @brief HashTable
 * @version 0.1
 * @author SYS Lab
 * @date 2022.11.01
 */

#include "HashTable.h"

namespace LightTable {

HashTable::HashTable() {
  buckets.reserve(HASH_BUCKET_SIZE);

  for (uint32_t i = 0; i < HASH_BUCKET_SIZE; i++) {
    HashBucket *tempBucket = new HashBucket();
    buckets.push_back(tempBucket);
  }
}

HashTable::~HashTable() {
  std::vector<HashBucket *>::iterator iter = buckets.begin();
  while (iter != buckets.end()) {
    iter = buckets.erase(iter);
  }
}

uint64_t HashTable::getMapKey(const char *primaryKey) {
  const std::string str = primaryKey;

  uint64_t BitsInUnsignedInt = (uint64_t)(4 * 8);
  uint64_t ThreeQuarters = (uint64_t)((BitsInUnsignedInt * 3) / 4);
  uint64_t OneEighth = (uint64_t)(BitsInUnsignedInt / 8);
  uint64_t HighBits = (uint64_t)(0xFFFFFFFF) << (BitsInUnsignedInt - OneEighth);
  uint64_t hash = 0;
  uint64_t test = 0;
  for (uint32_t i = 0; i < str.length(); i++) {
    hash = (hash << OneEighth) + str[i];
    if ((test = hash & HighBits) != 0) {
      hash = ((hash ^ (test >> ThreeQuarters)) & (~HighBits));
    }
  }
  return hash;
}

uint32_t HashTable::hashToBucket(uint64_t key) {
  return key % HASH_BUCKET_SIZE;
}

uint32_t HashTable::getRowLocation(uint32_t bucketID, uint64_t key,
                                   RowLocation &rowLocation) {
  return buckets[bucketID]->getRowLocation(key, rowLocation);
}

uint32_t HashTable::addItem(uint32_t bucketID, uint64_t key,
                            RowLocation rowLocation) {
  return buckets[bucketID]->addBucketItem(key, rowLocation);
}

uint32_t HashTable::deleteItem(uint32_t bucketID, uint64_t key) {
  return buckets[bucketID]->deleteBucketItem(key);
}

uint32_t HashTable::updateItem(uint32_t bucketID, uint64_t key,
                               RowLocation rowLocation) {
  return buckets[bucketID]->updateRowLocation(key, rowLocation);
}

uint64_t HashTable::getItemCount() {
  if (buckets.size() == 0) {
    return buckets.size();
  }
  uint64_t itemCount = 0;
  for (uint32_t i = 0; i < HASH_BUCKET_SIZE; i++) {
    itemCount += buckets[i]->getBucketSize();
  }
  return itemCount;
}

uint32_t HashTable::getAllBuckets(std::vector<HashBucket *> &buckets) {
  buckets = this->buckets;
  return SUCCESS;
}

uint32_t HashTable::refreshRowLocations(
    uint64_t blockID, uint64_t deleteOffset, uint64_t rowSize,
    std::map<uint64_t, RowLocation> &locationItems) {
  for (uint32_t i = 0; i < HASH_BUCKET_SIZE; i++) {
    std::map<uint64_t, RowLocation> bucketItems;
    buckets[i]->getAllBucketItems(bucketItems);
    std::map<uint64_t, RowLocation>::iterator iter = bucketItems.begin();
    while (iter != bucketItems.end()) {
      if ((iter->second).blockID == blockID &&
          (iter->second).rowOffset > deleteOffset) {
        (iter->second).rowOffset -= rowSize;
        locationItems.insert(
            std::pair<uint64_t, RowLocation>(iter->first, iter->second));
      }
    }
  }
  return SUCCESS;
}

uint32_t HashTable::clear() {
  std::vector<HashBucket *>::iterator iter = buckets.begin();
  while (iter != buckets.end()) {
    iter = buckets.erase(iter);
  }
  return SUCCESS;
}

}  // namespace LightTable
