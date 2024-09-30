/**
 * @file HashBucket.cc
 * @brief HashBucket
 * @version 0.1
 * @author SYS Lab
 * @date 2022.11.01
 */

#include "HashBucket.h"

namespace LightTable {

HashBucket::HashBucket() : bucketSize(0) {}

uint32_t HashBucket::addBucketItem(uint64_t key, RowLocation rowLocation) {
  std::pair<std::map<uint64_t, RowLocation>::iterator, bool> ret;
  // std::unique_lock<std::mutex> lck(mtx);
  // pthread_mutex_lock(&mtx);
  ret = bucketItems.insert(std::pair<uint64_t, RowLocation>(key, rowLocation));
  // lck.unlock();
  // pthread_mutex_unlock(&mtx);
  if (ret.second == false) {
    return ADD_BUCKET_ITEM_ERROR;
  }
  bucketSize++;
  return SUCCESS;
}

uint32_t HashBucket::deleteBucketItem(uint64_t key) {
  if (bucketItems.erase(key) == 1) {
    bucketSize--;
    return SUCCESS;
  }
  return BUCKET_ITEM_NOT_FOUND;
}

uint32_t HashBucket::updateRowLocation(uint64_t key, RowLocation rowLocation) {
  std::map<uint64_t, RowLocation>::iterator iter;
  iter = bucketItems.find(key);

  if (iter != bucketItems.end()) {
    (iter->second).blockID = rowLocation.blockID;
    (iter->second).rowOffset = rowLocation.rowOffset;
    return SUCCESS;
  }
  return BUCKET_ITEM_NOT_FOUND;
}

uint32_t HashBucket::getRowLocation(uint64_t key, RowLocation &rowLocation) {
  std::map<uint64_t, RowLocation>::iterator iter;
  iter = bucketItems.find(key);

  if (iter != bucketItems.end()) {
    rowLocation.blockID = (iter->second).blockID;
    rowLocation.rowOffset = (iter->second).rowOffset;
    return SUCCESS;
  }

  rowLocation.blockID = 0;
  rowLocation.rowOffset = 0;
  return BUCKET_ITEM_NOT_FOUND;
}

uint64_t HashBucket::getBucketSize() { return bucketSize; }

uint32_t HashBucket::setBucketSize(uint64_t bucketSize) {
  this->bucketSize = bucketSize;
  return SUCCESS;
}

uint32_t HashBucket::getAllBucketItems(
    std::map<uint64_t, RowLocation> &bucketItems) {
  bucketItems = this->bucketItems;
  return SUCCESS;
}

}  // namespace LightTable
