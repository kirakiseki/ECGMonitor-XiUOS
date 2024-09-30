/**
 * @file RootTable.h
 * @brief RootTable
 * @version 0.1
 * @author SYS Lab
 * @date 2022.11.01
 */

#ifndef ROOTTABLE_H
#define ROOTTABLE_H

#include <stddef.h>
#include <time.h>

#include <algorithm>
#include <iostream>
#include <map>

#include "Common.h"
#include "SystemTable.h"

namespace LightTable {

class Buffer;

class RootTable : public SystemTable {
 public:
  typedef struct tableTuple {
    uint64_t tableID;
    char tableName[TABLE_NAME_LENGTH];
    uint64_t firstBlockID;
    uint64_t segmentID;
    SegmentType segmentType;
    char user[USER_NAME_LENGTH];
    struct tm time;
  } TableTuple;

  RootTable(std::vector<Schema::SchemaEntry> schemaEntrys,
            bool usePrimaryKeyIndex)
      : SystemTable(ROOTTABLE_TABLE_ID, "RootTable", ROOTTABLE_FIRST_BLOCK_ID,
                    ROOTTABLE_SEGMENT_ID, SEGMENT_TYPE_BIG, schemaEntrys,
                    usePrimaryKeyIndex) {}

  RootTable(bool usePrimaryKeyIndex = false);

  uint32_t tableNameToTableID(const char *tableName, uint64_t &tableID);

  uint32_t deleteTableInfo(uint64_t tableID);

  uint32_t appendTableInfo(TableTuple tableTuple);

  uint32_t getTableInfo(uint64_t tableID, TableTuple &tableTupleResult);

  uint32_t modifyTableInfo(uint64_t tableID, TableTuple tableTuple);

  uint32_t getTableTuple(uint64_t tableID, TableTuple &tableTuple);

  uint32_t serializeRow(const TableTuple tableTuple, uint8_t *buf);

  uint32_t deserializeRow(const uint8_t *buf, TableTuple *tableTuple);

  uint64_t getTableCount();

 private:
  std::map<uint64_t, TableTuple> tableTupleMap;

  DISALLOW_COPY_AND_ASSIGN(RootTable);

 public:
  friend class MetaHandle;
};

}  // namespace LightTable

#endif  // ROOTTABLE_H
