/**
 * @file RowMap.cc
 * @brief RowMap
 * @version 0.1
 * @author SYS Lab
 * @date 2022.11.01
 */

#include "RowMap.h"

namespace LightTable {

RowMap::RowMap() {}

uint32_t RowMap::appendEntry(uint64_t key, uint64_t blockID,
                             uint64_t rowOffset) {
  std::pair<std::map<uint64_t, RowMapEntry>::iterator, bool> ret;
  RowMapEntry rowMapEntry = {key, blockID, rowOffset};

  ret = rowMapEntrys.insert(std::pair<uint64_t, RowMapEntry>(key, rowMapEntry));

  if (ret.second == false) {
    return ADD_ROW_MAP_ENTRY_ERROR;
  }

  return SUCCESS;
}

uint32_t RowMap::deleteEntry(uint64_t key) {
  if (rowMapEntrys.erase(key) == 1) {
    return SUCCESS;
  }
  return ROW_MAP_ENTRY_NOT_FOUND;
}

uint32_t RowMap::alterEntry(uint64_t key, uint64_t blockID,
                            uint64_t rowOffset) {
  std::map<uint64_t, RowMapEntry>::iterator iter;
  iter = rowMapEntrys.find(key);

  if (iter != rowMapEntrys.end()) {
    (iter->second).key = key;
    (iter->second).blockID = blockID;
    (iter->second).rowOffset = rowOffset;
    return SUCCESS;
  }

  return ROW_MAP_ENTRY_NOT_FOUND;
}

uint32_t RowMap::queryEntry(uint64_t key, RowMapEntry &rowMapEntry) {
  std::map<uint64_t, RowMapEntry>::iterator iter;
  iter = rowMapEntrys.find(key);

  if (iter != rowMapEntrys.end()) {
    rowMapEntry = iter->second;
    return SUCCESS;
  }

  rowMapEntry.key = key;
  rowMapEntry.blockID = 0;
  rowMapEntry.rowOffset = 0;
  return ROW_MAP_ENTRY_NOT_FOUND;
}

uint32_t RowMap::queryAllEntry(
    std::map<uint64_t, RowMapEntry> &allRowMapEntrys) {
  if (rowMapEntrys.empty() == false) {
    allRowMapEntrys = rowMapEntrys;
    return SUCCESS;
  }
  return ROW_MAP_ENTRY_NOT_FOUND;
}

uint64_t RowMap::getRowCount() { return rowMapEntrys.size(); }

}  // namespace LightTable
