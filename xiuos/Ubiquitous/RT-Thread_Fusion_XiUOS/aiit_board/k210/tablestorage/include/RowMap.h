/**
 * @file RowMap.h
 * @brief
 * @version 0.1
 * @author SYS Lab
 * @date 2022.11.01
 */

#ifndef ROWMAP_H
#define ROWMAP_H

#include <stdint.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <string>

#include "Common.h"
#include "Error.h"

namespace LightTable {

class RowMap {
 public:
  typedef struct rowMapEntry {
    uint64_t key;
    uint64_t blockID;
    uint64_t rowOffset;
  } RowMapEntry;

  RowMap();

  uint32_t appendEntry(uint64_t key, uint64_t blockID, uint64_t rowOffset);

  uint32_t deleteEntry(uint64_t key);

  uint32_t alterEntry(uint64_t key, uint64_t blockID, uint64_t rowOffset);

  uint32_t queryEntry(uint64_t key, RowMapEntry &rowMapEntry);

  uint32_t queryAllEntry(std::map<uint64_t, RowMapEntry> &allRowMapEntrys);

  uint64_t getRowCount();

 private:
  std::map<uint64_t, RowMapEntry> rowMapEntrys;

  DISALLOW_COPY_AND_ASSIGN(RowMap);

 public:
  friend class MetaHandle;
};

}  // namespace LightTable

#endif  // ROWMAP_H
