/**
 * @file Table.h
 * @brief Table
 * @version 0.1
 * @author SYS Lab
 * @date 2022.11.01
 */

#ifndef TABLE_H
#define TABLE_H

#include <stdint.h>
#include <string.h>
#include <stddef.h>

#include <algorithm>
#include <ctime>
#include <iostream>
#include <queue>
#include <string>
#include <vector>

#include "Common.h"
#include "HashTable.h"
#include "RowMap.h"
#include "Schema.h"
#include "Driver.h"
#include "UndoLogEntry.h"
#include "SegmentManager.h"
#include "PrefetchBlockManager.h"

namespace LightTable {
#ifdef TABLE_STORAGE_SECOND_INDEX
  // TODO: second index
#endif

class Buffer;

class Table {
  typedef struct currentLocation {
    uint64_t blockID;
    int32_t emptySlotNum;
  } currentLocation;

  typedef struct metaLogEntry {
    uint64_t tableID;
    uint64_t tupleNum;
    std::time_t t;
  } metaLogEntry;

 public:
  Table(uint64_t tableID, const char *tableName, uint64_t firstBlockID,
        uint64_t segmentID, SegmentType segmentType,
        std::vector<Schema::SchemaEntry> schemaEntrys,
        bool usePrimaryKeyIndex = false);

  Table(uint64_t tableID, const char *tableName, uint64_t firstBlockID,
        uint64_t segmentID, SegmentType segmentType,
        bool usePrimaryKeyIndex = false);

  uint32_t drop();

  uint32_t truncate();

  uint32_t insertRow(const uint8_t *rowData);

  uint32_t insertRow_logging(const uint8_t *rowData);

  uint32_t deleteRow(const char *primaryKey);

  uint32_t updateRow(const char *primaryKey, const uint8_t *rowData);

  uint32_t updateColumnItem(const char *primaryKey, const char *columnName,
                            const uint8_t *columnData);

  uint32_t updateColumnItem_logging(const char *primaryKey,
                                    const char *columnName,
                                    const uint8_t *columnData);

  uint32_t selectRow(const char *primaryKey, uint8_t *rowData);

  uint32_t selectColumnItem(const char *primaryKey, const char *columnName,
                            uint8_t *columnData);

  uint64_t getRowCount();

  uint64_t getTableID();

  char *getTableName();

  uint32_t setTableID(uint64_t tableID);

  uint32_t setTableName(const char *tableName);

  Schema &getSchema();

  HashTable &getHashTable();

 protected:
  uint32_t getColumnID(const char *columnName, uint64_t &columnID);

  uint32_t parsePrimaryKey(const uint8_t *rowData, char *primaryKeyData);

  uint32_t locateRow(const char *primaryKey, RowLocation &rowLocation);

  uint32_t getRowFromBlock(const char *primaryKey, uint8_t *rowData,
                           const uint8_t *blockData,
                           const RowLocation rowLocation);

  uint32_t selectRow(const char *primaryKey, uint8_t *rowData,
                     RowLocation &rowLocation);

  uint32_t deleteRowData(uint64_t key, RowLocation rowLocation);

  uint32_t getColumnOffset(const char *columnName, uint64_t &columnID,
                           uint32_t &columnLength, uint64_t &offset);

  uint64_t tableID;
  char tableName[TABLE_NAME_LENGTH];
  Schema schema;
  RowMap rowMap;
  HashTable hashTable;
  bool usePrimaryKeyIndex;
  uint64_t tupleNum;
  std::time_t t;
  currentLocation indexLocation;
  currentLocation metaLogLocation;
  currentLocation indexLogLocation;
  currentLocation dataLogLocation;
  uint8_t indexData[BLOCK_SIZE];
  uint8_t metaData[BLOCK_SIZE];
  uint8_t metaLog[BLOCK_SIZE];
  uint8_t indexLog[BLOCK_SIZE];
  uint8_t dataLog[BLOCK_SIZE];
  PrefetchBlockManager prefetchBlockManager;

  DISALLOW_COPY_AND_ASSIGN(Table);

 public:
  friend class DaemonProcess;
  friend class MetaHandle;
  friend class QueryCache;
};

}  // namespace LightTable

#endif  // TABLE_H
