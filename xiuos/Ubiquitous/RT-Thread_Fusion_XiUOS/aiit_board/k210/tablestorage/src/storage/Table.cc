/**
 * @file Table.cc
 * @brief Table
 * @version 0.1
 * @author SYS Lab
 * @date 2022.11.01
 */

#include "Table.h"

namespace LightTable {

Table::Table(uint64_t tableID, const char *tableName, uint64_t firstBlockID,
             uint64_t segmentID, SegmentType segmentType,
             std::vector<Schema::SchemaEntry> schemaEntrys,
             bool usePrimaryKeyIndex)
    : prefetchBlockManager(firstBlockID, segmentID, segmentType),
      schema(schemaEntrys),
      rowMap(),
      tupleNum(0),
      t(time(0)),
      indexLocation{TABLE1_INDEX_BLOCKID, META_ENTRY_NUM},
      metaLogLocation{TABLE1_META_LOG_BLOCKID, META_ENTRY_NUM},
      indexLogLocation{TABLE1_INDEX_LOG_BLOCKID, META_ENTRY_NUM},
      dataLogLocation{TABLE1_DATA_LOG_BLOCKID, META_ENTRY_NUM} {
  this->tableID = tableID;
  memcpy(this->tableName, tableName, sizeof(tableName) + 1);
  this->usePrimaryKeyIndex = usePrimaryKeyIndex;
  std::map<uint64_t, Schema::SchemaEntry> allSchemaEntrys;
  schema.queryAllEntry(allSchemaEntrys);
}

Table::Table(uint64_t tableID, const char *tableName, uint64_t firstBlockID,
             uint64_t segmentID, SegmentType segmentType,
             bool usePrimaryKeyIndex)
    : prefetchBlockManager(firstBlockID, segmentID, segmentType),
      schema(),
      rowMap() {
  this->tableID = tableID;
  memcpy(this->tableName, tableName, sizeof(tableName) + 1);
  // this->buf = buf;
  // this->queryCache = queryCache;
  // this->blockPreload = blockPreload;
  this->usePrimaryKeyIndex = usePrimaryKeyIndex;
}

uint32_t Table::drop() {
  uint32_t ret = SUCCESS;

  ret = truncate();
  if (ret != SUCCESS) {
    return ret;
  }

  schema.drop();

  // std::map<uint64_t, Schema::SchemaEntry> allSchemaEntrys;
  // schema.queryAllEntry(allSchemaEntrys);
  // uint64_t columnCount = allSchemaEntrys.size();
  // uint64_t length = 10 + columnCount * (SCHEMA_ENTRY_SERIALIZED_LENGTH + 1);
  // char *schemaData = new char[length];
  // uint32_t size = 0;
  // MetaHandle::serialize(allSchemaEntrys, tableID, schemaData, size);
  // MetaBufferItem *schemaMetaItem = new MetaBufferItem(tableID, schemaData,
  // size); buf->append(schemaMetaItem); delete[] schemaData;

  return SUCCESS;
}

// uint32_t Table::truncate() {
//   uint32_t size = 0;
//   char *pbmData;
//   MetaBufferItem *pbmMeta;
//   char *hashTableData;
//   MetaBufferItem *hashTableMeta;
//   std::map<uint64_t, PrefetchBlockManager::PrefetchBlockManagerEntry> entrys;
//   prefetchBlockManager.getAllPrefetchBlockEntrys(entrys);
//   std::map<uint64_t, PrefetchBlockManager::PrefetchBlockManagerEntry>::iterator
//       pbmIter = entrys.begin();
//   while (pbmIter != entrys.end()) {
//     prefetchBlockManager.lockBlock(pbmIter->first);
//     prefetchBlockManager.setOffset(pbmIter->first, 0);
//     prefetchBlockManager.unlockBlock(pbmIter->first);
//     pbmData = new char[PREFETCH_BLOCK_SERIALIZED_LENGTH];
//     MetaHandle::serialize(pbmIter->second, tableID, pbmData, size);
//     pbmMeta = new MetaBufferItem(tableID, pbmData, size);
//     // buf->append(pbmMeta);
//     pbmIter++;
//   }

//   std::vector<HashBucket *> buckets;
//   hashTable.getAllBuckets(buckets);
//   for (uint32_t i = 0; i < buckets.size(); i++) {
//     std::map<uint64_t, RowLocation> bucketItems;
//     buckets[i]->getAllBucketItems(bucketItems);
//     std::map<uint64_t, RowLocation>::iterator itemIter;
//     itemIter = bucketItems.begin();
//     while (itemIter != bucketItems.end()) {
//       itemIter->second.blockID = 0;
//       hashTableData = new char[ROWMAP_SERIALIZED_LENGTH];
//       MetaHandle::serialize(
//           std::pair<uint64_t, RowLocation>(itemIter->first, itemIter->second),
//           tableID, hashTableData, size);
//       hashTableMeta = new MetaBufferItem(tableID, hashTableData, size);
//       // buf->append(hashTableMeta);
//       itemIter = bucketItems.erase(itemIter);
//       itemIter++;
//     }
//   }

//   return SUCCESS;
// }

uint32_t Table::parsePrimaryKey(const uint8_t *rowData, char *primaryKeyData) {
  if (strncmp(schema.getPrimaryKeySchema().type, "uint64_t", 8) == 0) {
    uint64_t primaryKeyI64 = 0;
    memcpy(&primaryKeyI64, rowData, schema.getPrimaryKeySchema().length);
    sprintf(primaryKeyData, "%ld", primaryKeyI64);
  } else if (strncmp(schema.getPrimaryKeySchema().type, "string", 6) == 0) {
    memcpy(primaryKeyData, rowData, schema.getPrimaryKeySchema().length);
  } else {
    return TYPE_INVALID;
  }
  return SUCCESS;
}

uint32_t Table::insertRow(const uint8_t *rowData) {
  uint32_t ret = SUCCESS;
  uint64_t blockID = 0;
  uint64_t blockOffset = 0;
  uint8_t blockData[BLOCK_SIZE];
  tupleNum++;
  t = time(0);

#ifndef YCSB_TEST
  blockID = prefetchBlockManager.blockAllocate(schema.getEntrySize());
  while (blockID == 0) {
    blockID = prefetchBlockManager.blockAllocate(schema.getEntrySize());
  }
  ret = prefetchBlockManager.getOffset(blockID, blockOffset);
  if (ret != SUCCESS) {
    // TODO allocate new seg
    return ret;
  }
#else
  blockID = prefetchBlockManager.getNextBlock();
#endif

  if (blockOffset != 0) {
    Driver::read(blockData, blockID);
  }

  memcpy(blockData + blockOffset, rowData, schema.getEntrySize());
#ifdef SDCARD_TEST
  start_time = std::chrono::steady_clock::now();
#endif
  Driver::write(blockData, blockID);
#ifdef SDCARD_TEST
  for (int i = 0; i < SDCARD_TEST_NUM; ++i)
    Driver::write(blockData, SDCARD_TEST_WRITE_BLOCKID + 100 * i);
  end_time = std::chrono::steady_clock::now();
  diff_sdcard_write = std::chrono::duration_cast<std::chrono::microseconds>(
                          end_time - start_time)
                          .count();
#endif

  char *primaryKeyData = new char[schema.getPrimaryKeySchema().length];
  ret = parsePrimaryKey(rowData, primaryKeyData);
  if (ret != SUCCESS) {
    delete[] primaryKeyData;
    prefetchBlockManager.unlockBlock(blockID);
    return ret;
  }

  uint64_t key = hashTable.getMapKey(primaryKeyData);
  uint32_t bucketID = hashTable.hashToBucket(key);
  RowLocation rowLocation = {blockID, blockOffset};
  ret = hashTable.addItem(bucketID, key, rowLocation);

  if (ret != SUCCESS) {
    delete[] primaryKeyData;
    prefetchBlockManager.unlockBlock(blockID);
    return ret;
  }

#ifdef TABLE_STORAGE_SECOND_INDEX
  ret = secondaryIndex.insert(primaryKeyData,
                              schema.getPrimaryKeySchema().length, rowLocation);
  if (ret != SUCCESS) {
    delete[] primaryKeyData;
    prefetchBlockManager.unlockBlock(blockID);
    return ret;
  }
#endif

#ifndef YCSB_TEST
  ret = prefetchBlockManager.advanceOffset(blockID, schema.getEntrySize());
  prefetchBlockManager.setBlockState(blockID, PREFETCH_BLOCK_STATUS_IDLE);
#endif

  delete[] primaryKeyData;

  return ret;
}

uint32_t Table::insertRow_logging(const uint8_t *rowData) {
  uint32_t ret = SUCCESS;
  uint64_t blockID = 0;
  uint64_t blockOffset = 0;

  tupleNum++;
  t = time(0);

  char *primaryKeyData = new char[schema.getPrimaryKeySchema().length];
  ret = parsePrimaryKey(rowData, primaryKeyData);
  if (ret != SUCCESS) {
    delete[] primaryKeyData;
    prefetchBlockManager.unlockBlock(blockID);
    return ret;
  }

  uint64_t key = hashTable.getMapKey(primaryKeyData);
  uint32_t bucketID = hashTable.hashToBucket(key);
  RowLocation rowLocation = {blockID, blockOffset};

  ret = hashTable.addItem(bucketID, key, rowLocation);
  if (ret != SUCCESS) {
    delete[] primaryKeyData;
    prefetchBlockManager.unlockBlock(blockID);
    return ret;
  }

  metaLogEntry metaEntry{.tableID = tableID, .tupleNum = tupleNum, .t = t};

  if (metaLogLocation.emptySlotNum != 0) {
    memcpy(metaLog + (META_ENTRY_SIZE *
                      (META_ENTRY_NUM - metaLogLocation.emptySlotNum)),
           &metaEntry, sizeof(metaLogEntry));
  } else {
    metaLogLocation.blockID++;
    metaLogLocation.emptySlotNum = META_ENTRY_NUM;
    memset(metaLog, 0, BLOCK_SIZE);
    memcpy(metaLog, &metaEntry, sizeof(metaLogEntry));
  }
  Driver::write(metaLog, metaLogLocation.blockID);
  metaLogLocation.emptySlotNum--;

  uint32_t size = 0;
  char *hashTableMetaData = new char[ROWMAP_SERIALIZED_LENGTH];
//   MetaHandle::serialize(std::pair<uint64_t, RowLocation>(key, rowLocation),
//                         tableID, hashTableMetaData, size);
//   MetaBufferItem *hashTableMeta =
//       new MetaBufferItem(tableID, hashTableMetaData, size);
//   if (indexLogLocation.emptySlotNum != 0) {
//     memcpy(indexLog + (META_ENTRY_SIZE *
//                        (META_ENTRY_NUM - indexLogLocation.emptySlotNum)),
//            hashTableMeta->metaItem,
//            META_ENTRY_SIZE > hashTableMeta->metaItemSize
//                ? hashTableMeta->metaItemSize
//                : META_ENTRY_SIZE);
//   } else {
//     indexLogLocation.blockID++;
//     indexLogLocation.emptySlotNum = META_ENTRY_NUM;
//     memset(indexLog, 0, BLOCK_SIZE);
//     memcpy(indexLog, hashTableMeta->metaItem,
//            META_ENTRY_SIZE > hashTableMeta->metaItemSize
//                ? hashTableMeta->metaItemSize
//                : META_ENTRY_SIZE);
//   }
  Driver::write(indexLog, indexLogLocation.blockID);
  indexLogLocation.emptySlotNum--;

  memcpy(metaData, &tupleNum, 8);
  memcpy(metaData + 8, &t, sizeof(std::time_t));
  Driver::write(metaData, TABLE1_META_BLOCKID);

//   if (indexLocation.emptySlotNum != 0) {
//     memcpy(indexData + (META_ENTRY_SIZE *
//                         (META_ENTRY_NUM - indexLocation.emptySlotNum)),
//            hashTableMeta->metaItem,
//            META_ENTRY_SIZE > hashTableMeta->metaItemSize
//                ? hashTableMeta->metaItemSize
//                : META_ENTRY_SIZE);
//   } else {
//     indexLocation.blockID++;
//     indexLocation.emptySlotNum = META_ENTRY_NUM;
//     memset(indexData, 0, BLOCK_SIZE);
//     memcpy(indexData, hashTableMeta->metaItem,
//            META_ENTRY_SIZE > hashTableMeta->metaItemSize
//                ? hashTableMeta->metaItemSize
//                : META_ENTRY_SIZE);
//   }
  Driver::write(indexData, indexLocation.blockID);
  indexLocation.emptySlotNum--;

  uint8_t blockData[BLOCK_SIZE];
  blockID = dataLogLocation.blockID;
  memcpy(blockData, rowData, schema.getEntrySize());
  Driver::write(blockData, blockID);
  dataLogLocation.blockID++;

  blockID = prefetchBlockManager.getNextBlock();
  if (blockOffset != 0) {
    Driver::read(blockData, blockID);
  }
  memcpy(blockData + blockOffset, rowData, schema.getEntrySize());
  Driver::write(blockData, blockID);

  delete[] hashTableMetaData;
//   delete hashTableMeta;
  delete[] primaryKeyData;

  return ret;
}

uint32_t Table::deleteRow(const char *primaryKey) {
  uint32_t ret = SUCCESS;

  RowLocation rowLocation;
  ret = locateRow(primaryKey, rowLocation);
  if (ret != SUCCESS) {
    return ret;
  }
  uint64_t deleteBlockID = rowLocation.blockID;
  uint64_t deleteRowOffset = rowLocation.rowOffset;

  while (prefetchBlockManager.lockBlock(rowLocation.blockID) != SUCCESS)
    ;
  uint64_t currentOffset = 0;
  ret = prefetchBlockManager.getOffset(rowLocation.blockID, currentOffset);
  uint64_t key = hashTable.getMapKey(primaryKey);

  if (rowLocation.rowOffset + schema.getEntrySize() != currentOffset) {
    ret = deleteRowData(key, rowLocation);
    if (ret != SUCCESS) {
      return ret;
    }
  }

  uint32_t size = 0;
  uint32_t bucketID = hashTable.hashToBucket(key);
  hashTable.deleteItem(bucketID, key);
  char *hashTableMetaData = new char[ROWMAP_SERIALIZED_LENGTH];
  rowLocation.blockID = 0;
  // MetaHandle::serialize(std::pair<uint64_t, RowLocation>(key, rowLocation),
  //                       tableID, hashTableMetaData, size);
  // MetaBufferItem *hashTableMeta= new MetaBufferItem(tableID,
  // hashTableMetaData,
  //                                                   size);
  // buf->append(hashTableMeta);

  if (deleteRowOffset + schema.getEntrySize() != currentOffset) {
    std::map<uint64_t, RowLocation> locationItems;
    hashTable.refreshRowLocations(deleteBlockID, deleteRowOffset,
                                  schema.getEntrySize(), locationItems);
    // std::map<uint64_t, RowLocation>::iterator iter = locationItems.begin();
    // while (iter != locationItems.end()) {
    //     MetaHandle::serialize(std::pair<uint64_t, RowLocation>(iter->first,
    //                           iter->second), tableID, hashTableMetaData,
    //                           size);
    //     MetaBufferItem *nextMeta= new MetaBufferItem(tableID,
    //     hashTableMetaData,
    //                                                  size);
    //     buf->append(nextMeta);
    // }
  }

  uint64_t newOffset = currentOffset - schema.getEntrySize();
  prefetchBlockManager.setOffset(deleteBlockID, newOffset);
  prefetchBlockManager.unlockBlock(deleteBlockID);
  PrefetchBlockManager::PrefetchBlockManagerEntry blockEntry;
  prefetchBlockManager.getPrefetchBlockEntry(deleteBlockID, blockEntry);
  // char *pbmData = new char[PREFETCH_BLOCK_SERIALIZED_LENGTH];
  // MetaHandle::serialize(blockEntry, tableID, pbmData, size);
  // MetaBufferItem *pbmMeta = new MetaBufferItem(tableID, pbmData, size);
  // buf->append(pbmMeta);

  return SUCCESS;
}

uint32_t Table::deleteRowData(uint64_t key, RowLocation rowLocation) {
  uint8_t blockData[BLOCK_SIZE];
  Driver::read(blockData, rowLocation.blockID);

  uint64_t start = rowLocation.rowOffset;
  uint64_t nextRow = start + schema.getEntrySize();
  memcpy(blockData + start, blockData + nextRow, BLOCK_SIZE - nextRow);

  Driver::write(blockData, rowLocation.blockID);

  return SUCCESS;
}

uint32_t Table::updateRow(const char *primaryKey, const uint8_t *rowData) {
  uint32_t ret = SUCCESS;

  RowLocation rowLocation;
  ret = locateRow(primaryKey, rowLocation);
  if (ret == BUCKET_ITEM_NOT_FOUND) {
    return insertRow(rowData);
  } else if (ret != SUCCESS) {
    return ret;
  }

  uint8_t blockData[BLOCK_SIZE];

  while (prefetchBlockManager.lockBlock(rowLocation.blockID) != SUCCESS)
    ;
  Driver::read(blockData, rowLocation.blockID);

  // char *undoTuple = new char[schema.getEntrySize()];
  // memcpy(undoTuple, blockData+rowLocation.rowOffset, schema.getEntrySize());
  // UndoLogEntry *logEntry = new UndoLogEntry(this->tableID, primaryKey,
  //                              schema.getPrimaryKeyLength(),
  //                              undoTuple, schema.getEntrySize(),
  //                              rowLocation.blockID, rowLocation.rowOffset);
  // buf->append(logEntry);

  memcpy(blockData + rowLocation.rowOffset, rowData, schema.getEntrySize());

  if (SUCCESS != Driver::write(blockData, rowLocation.blockID)) {
    // memcpy(blockData + rowLocation.rowOffset, undoTuple,
    //        schema.getEntrySize());
    // ret = UPDATE_TUPLE_ERROR;
  }

  // logEntry->expireUndoLog();
  prefetchBlockManager.unlockBlock(rowLocation.blockID);
  return ret;
}

uint32_t Table::locateRow(const char *primaryKey, RowLocation &rowLocation) {
  uint32_t ret = SUCCESS;

  uint64_t key = hashTable.getMapKey(primaryKey);
  uint32_t bucketID = hashTable.hashToBucket(key);

  ret = hashTable.getRowLocation(bucketID, key, rowLocation);
  return ret;
}

uint32_t Table::selectRow(const char *primaryKey, uint8_t *rowData) {
  RowLocation rowLocation;
  return selectRow(primaryKey, rowData, rowLocation);
}

uint32_t Table::selectRow(const char *primaryKey, uint8_t *rowData,
                          RowLocation &rowLocation) {
  uint32_t ret = SUCCESS;
  uint8_t blockData[BLOCK_SIZE];

  ret = locateRow(primaryKey, rowLocation);
  if (ret != SUCCESS) {
    return ret;
  }

#ifndef YCSB_TEST
  while (prefetchBlockManager.lockBlock(rowLocation.blockID) != SUCCESS)
    ;
#endif

#ifdef SDCARD_TEST
  start_time = std::chrono::steady_clock::now();
#endif
  Driver::read(blockData, rowLocation.blockID);
#ifdef SDCARD_TEST
  for (int i = 0; i < SDCARD_TEST_NUM; ++i)
    Driver::read(blockData, SDCARD_TEST_READ_BLOCKID + 100 * i);
  end_time = std::chrono::steady_clock::now();
  diff_sdcard_read = (std::chrono::duration_cast<std::chrono::microseconds>(
                          end_time - start_time)
                          .count());
#endif
#ifndef YCSB_TEST
  prefetchBlockManager.unlockBlock(rowLocation.blockID);
#endif

  ret = getRowFromBlock(primaryKey, rowData, blockData, rowLocation);

  return ret;
}

uint32_t Table::getRowFromBlock(const char *primaryKey, uint8_t *rowData,
                                const uint8_t *blockData,
                                const RowLocation rowLocation) {
  uint32_t ret = SUCCESS;

  char *primaryKeyData = new char[schema.getPrimaryKeySchema().length];
  if (rowLocation.rowOffset + schema.getEntrySize() > BLOCK_SIZE) {
    delete[] primaryKeyData;
    return ROWLOCATION_INVALID;
  }

  ret = parsePrimaryKey(blockData + rowLocation.rowOffset, primaryKeyData);
  if (ret != SUCCESS) {
    delete[] primaryKeyData;
    return ret;
  }

  if (strncmp(primaryKeyData, primaryKey,
              schema.getPrimaryKeySchema().length) != 0) {
    delete[] primaryKeyData;
    return ERROR_TABLE;
  }

  memcpy(rowData, blockData + rowLocation.rowOffset, schema.getEntrySize());

  delete[] primaryKeyData;
  return SUCCESS;
}

uint32_t Table::selectColumnItem(const char *primaryKey, const char *columnName,
                                 uint8_t *columnData) {
  uint32_t ret = SUCCESS;

  uint8_t *rowData = new uint8_t[schema.getEntrySize()];
  RowLocation rowLocation;
  ret = selectRow(primaryKey, rowData, rowLocation);
  if (ret != SUCCESS) {
    delete[] rowData;
    return ret;
  }

  uint64_t columnOffset = 0;
  uint64_t columnID = 0;
  uint32_t columnLength = 0;
  ret = getColumnOffset(columnName, columnID, columnLength, columnOffset);

  memcpy(columnData, rowData + columnOffset, columnLength);

  delete[] rowData;
  return SUCCESS;
}

uint32_t Table::updateColumnItem(const char *primaryKey, const char *columnName,
                                 const uint8_t *columnData) {
  uint32_t ret = SUCCESS;

  uint8_t *blockData = new uint8_t[BLOCK_SIZE];
  RowLocation rowLocation;
  ret = locateRow(primaryKey, rowLocation);
  if (ret != SUCCESS) {
    return ret;
  }

#ifndef YCSB_TEST
  while (prefetchBlockManager.lockBlock(rowLocation.blockID) != SUCCESS)
    ;
#endif
  Driver::read(blockData, rowLocation.blockID);

  // char *undoTuple = new char[schema.getEntrySize()];
  // memcpy(undoTuple, blockData+rowLocation.rowOffset, schema.getEntrySize());
  // UndoLogEntry *logEntry = new UndoLogEntry(this->tableID, primaryKey,
  //                              schema.getPrimaryKeyLength(),
  //                              undoTuple, schema.getEntrySize(),
  //                              rowLocation.blockID, rowLocation.rowOffset);
  // buf->append(logEntry);

  uint64_t columnOffset, columnID;
  uint32_t columnLength;
  ret = getColumnOffset(columnName, columnID, columnLength, columnOffset);

  uint64_t columnStart = rowLocation.rowOffset + columnOffset;
  memcpy(blockData + columnStart, columnData, columnLength);

  if (SUCCESS != Driver::write(blockData, rowLocation.blockID)) {
    // memcpy(blockData + rowLocation.rowOffset, undoTuple,
    //        schema.getEntrySize());
    // ret = UPDATE_TUPLE_ERROR;
  }

  // logEntry->expireUndoLog();
#ifndef YCSB_TEST
  prefetchBlockManager.unlockBlock(rowLocation.blockID);
#endif

  delete[] blockData;
  return SUCCESS;
}

uint32_t Table::updateColumnItem_logging(const char *primaryKey,
                                         const char *columnName,
                                         const uint8_t *columnData) {
  // TODO
  return SUCCESS;
}

uint32_t Table::getColumnOffset(const char *columnName, uint64_t &columnID,
                                uint32_t &columnLength, uint64_t &offset) {
  uint32_t ret = SUCCESS;
  offset = 0;
  columnID = 0;
  columnLength = 0;

  ret = getColumnID(columnName, columnID);
  if (ret != SUCCESS) {
    return ret;
  }

  std::map<uint64_t, Schema::SchemaEntry> schemaEntrys;
  ret = schema.queryAllEntry(schemaEntrys);
  if (ret != SUCCESS) {
    return ret;
  }

  columnLength = schemaEntrys[columnID].length;
  for (uint64_t i = 1; i < columnID; i++) {
    offset += schemaEntrys[i].length;
  }

  return SUCCESS;
}

uint64_t Table::getRowCount() { return hashTable.getItemCount(); }

uint64_t Table::getTableID() { return tableID; }

char *Table::getTableName() { return tableName; }

uint32_t Table::setTableID(uint64_t tableID) {
  this->tableID = tableID;
  return SUCCESS;
}

uint32_t Table::setTableName(const char *tableName) {
  memcpy(this->tableName, tableName, sizeof(tableName) + 1);
  return SUCCESS;
}

uint32_t Table::getColumnID(const char *columnName, uint64_t &columnID) {
  return schema.getColumnID(columnName, columnID);
}

Schema &Table::getSchema() { return schema; }

HashTable &Table::getHashTable() { return hashTable; }

}  // namespace LightTable
