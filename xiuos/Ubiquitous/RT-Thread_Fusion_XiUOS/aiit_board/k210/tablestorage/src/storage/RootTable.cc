/**
 * @file RootTable.cc
 * @brief RootTable
 * @version 0.1
 * @author SYS Lab
 * @date 2022.11.01
 */

#include "RootTable.h"

namespace LightTable {

RootTable::RootTable(bool usePrimaryKeyIndex)
    : SystemTable(ROOTTABLE_TABLE_ID, "RootTable", ROOTTABLE_FIRST_BLOCK_ID,
                  ROOTTABLE_SEGMENT_ID, SEGMENT_TYPE_SMALL,
                  usePrimaryKeyIndex) {
  Schema::SchemaEntry entry1 = {1, "tableID", "uint64_t", 8, true, false};
  schema.appendEntry(entry1);
  Schema::SchemaEntry entry2 = {2,     "tableName", "string", TABLE_NAME_LENGTH,
                                false, false};
  schema.appendEntry(entry2);
  Schema::SchemaEntry entry3 = {3, "firstBlockID", "uint64_t", 8, false, false};
  schema.appendEntry(entry3);
  Schema::SchemaEntry entry4 = {4, "segmentID", "uint64_t", 8, false, false};
  schema.appendEntry(entry4);
  Schema::SchemaEntry entry5 = {5, "segmentType", "uint32_t", 4, false, false};
  schema.appendEntry(entry5);
  Schema::SchemaEntry entry6 = {6,     "user", "string", USER_NAME_LENGTH,
                                false, true};
  schema.appendEntry(entry6);
  Schema::SchemaEntry entry7 = {7, "time", "time", 60, false, false};
  schema.appendEntry(entry7);
}

uint32_t RootTable::tableNameToTableID(const char *tableName,
                                       uint64_t &tableID) {
  std::map<uint64_t, TableTuple>::iterator iter = tableTupleMap.begin();

  while (iter != tableTupleMap.end()) {
    if (0 == strncmp(tableName, iter->second.tableName, TABLE_NAME_LENGTH)) {
      tableID = iter->second.tableID;
      return SUCCESS;
    }
    iter++;
  }
  return TABLE_NOT_FOUND;
}

uint32_t RootTable::deleteTableInfo(uint64_t tableID) {
  uint32_t ret = SUCCESS;

  RowLocation rowLocation;
  uint32_t bucketID = hashTable.hashToBucket(tableID);
  ret = hashTable.getRowLocation(bucketID, tableID, rowLocation);

  uint64_t deleteBlockID = rowLocation.blockID;
  uint64_t deleteRowOffset = rowLocation.rowOffset;
  char *hashTableMetaData = new char[ROWMAP_SERIALIZED_LENGTH];
  uint32_t size = 0;

  while (prefetchBlockManager.lockBlock(rowLocation.blockID) != SUCCESS)
    ;
  uint64_t currentOffset = 0;
  ret = prefetchBlockManager.getOffset(rowLocation.blockID, currentOffset);
  if (ret != SUCCESS) {
    return ret;
  }

  if (rowLocation.rowOffset + schema.getEntrySize() != currentOffset) {
    // RowBufferItem::Tuple item;
    // uint8_t str[ROOTTABLE_TUPLE_SIZE];
    // memcpy(str, &tableTupleMap[tableID], sizeof(tableTupleMap[tableID]));
    // item.blockID = rowLocation.blockID;
    // item.rowOffset = rowLocation.rowOffset;
    // item.isDelete = true;
    // RowBufferItem *rowBufferItem = new RowBufferItem(item);
    // buf->append(rowBufferItem);

    std::map<uint64_t, RowLocation> locationItems;
    hashTable.refreshRowLocations(deleteBlockID, deleteRowOffset,
                                  schema.getEntrySize(), locationItems);
    // std::map<uint64_t, RowLocation>::iterator iter = locationItems.begin();
    // while (iter != locationItems.end()) {
    // MetaHandle::serialize(std::pair<uint64_t, RowLocation>(iter->first,
    //                       iter->second), tableID, hashTableMetaData, size);
    // MetaBufferItem *nextMeta= new MetaBufferItem(tableID, hashTableMetaData,
    // size); buf->append(nextMeta);
    // }
  }

  hashTable.deleteItem(bucketID, tableID);
  rowLocation.blockID = 0;
  // MetaHandle::serialize(std::pair<uint64_t, RowLocation>(tableID,
  // rowLocation),
  //                       ROOTTABLE_TABLE_ID, hashTableMetaData, size);
  // MetaBufferItem *hashTableMeta= new MetaBufferItem(ROOTTABLE_TABLE_ID,
  //                                                   hashTableMetaData, size);
  // buf->append(hashTableMeta);

  uint64_t newOffset = currentOffset - schema.getEntrySize();
  prefetchBlockManager.setOffset(deleteBlockID, newOffset);

  PrefetchBlockManager::PrefetchBlockManagerEntry blockEntry;
  prefetchBlockManager.unlockBlock(deleteBlockID);
  prefetchBlockManager.getPrefetchBlockEntry(deleteBlockID, blockEntry);
  // char *pbmData = new char[PREFETCH_BLOCK_SERIALIZED_LENGTH];
  // MetaHandle::serialize(blockEntry, ROOTTABLE_TABLE_ID, pbmData, size);
  // MetaBufferItem *pbmMeta = new MetaBufferItem(ROOTTABLE_TABLE_ID, pbmData,
  // size); buf->append(pbmMeta);

  if (tableTupleMap.erase(tableID) == 1) {
    return SUCCESS;
  }
  return ERROR_TABLE;
}

uint32_t RootTable::appendTableInfo(TableTuple tableTuple) {
  uint32_t err = SUCCESS;

  tableTupleMap.insert(
      std::pair<uint64_t, TableTuple>(tableTuple.tableID, tableTuple));

  uint64_t blockID = 0;
  uint64_t blockOffset = 0;
  blockID = prefetchBlockManager.blockAllocate(schema.getEntrySize());
  err = prefetchBlockManager.getOffset(blockID, blockOffset);

  if (err != SUCCESS) {
    // TODO allocate new seg
    return GET_AVAILABLE_BLOCK_ERROR;
  }

  // RowBufferItem::Tuple item;
  // uint8_t str[ROOTTABLE_TUPLE_SIZE];
  // memcpy(str, &tableTuple, sizeof(tableTuple));
  // item.row = str;
  // item.blockID = blockID;
  // item.rowOffset = blockOffset;
  // item.isDelete = false;
  // RowBufferItem *rowBufferItem = new RowBufferItem(item);
  // buf->append(rowBufferItem);

  uint32_t bucketID = hashTable.hashToBucket(tableTuple.tableID);
  RowLocation rowLocation = {blockID, blockOffset};
  err = hashTable.addItem(bucketID, tableTuple.tableID, rowLocation);

  if (err != SUCCESS) {
    return err;
  }

  // uint32_t size = 0;
  // char *hashTableMetaData = new char[ROWMAP_SERIALIZED_LENGTH];
  // MetaHandle::serialize(std::pair<uint64_t, RowLocation>(tableTuple.tableID,
  //                       rowLocation), tableID, hashTableMetaData, size);
  // MetaBufferItem *hashTableMeta = new MetaBufferItem(tableID,
  // hashTableMetaData,
  //                                                    size);
  // buf->append(hashTableMeta);

  err = prefetchBlockManager.advanceOffset(blockID, schema.getEntrySize());
  prefetchBlockManager.setBlockState(blockID, PREFETCH_BLOCK_STATUS_IDLE);

  // PrefetchBlockManager::PrefetchBlockManagerEntry blockEntry;
  // prefetchBlockManager.getPrefetchBlockEntry(blockID, blockEntry);
  // char *pbmData = new char[PREFETCH_BLOCK_SERIALIZED_LENGTH];
  // MetaHandle::serialize(blockEntry, tableID, pbmData, size);
  // MetaBufferItem *pbmMeta = new MetaBufferItem(tableID, pbmData, size);
  // buf->append(pbmMeta);

  return SUCCESS;
}

uint32_t RootTable::getTableInfo(uint64_t tableID,
                                 TableTuple &tableTupleResult) {
  std::map<uint64_t, TableTuple>::iterator iter;
  iter = tableTupleMap.find(tableID);

  if (iter != tableTupleMap.end()) {
    tableTupleResult = iter->second;
    return SUCCESS;
  }
  tableTupleResult.tableID = 0;
  return TABLE_TUPLE_NOT_FOUND;
}

uint32_t RootTable::modifyTableInfo(uint64_t tableID, TableTuple tableTuple) {
  uint32_t ret = SUCCESS;
  std::map<uint64_t, TableTuple>::iterator iter;
  iter = tableTupleMap.find(tableID);

  if (iter != tableTupleMap.end()) {
    iter->second = tableTuple;

    uint32_t bucketID = hashTable.hashToBucket(tableID);
    RowLocation rowLocation;
    ret = hashTable.getRowLocation(bucketID, tableID, rowLocation);
    if (ret != SUCCESS) {
      return ret;
    }
    // RowBufferItem::Tuple item;
    // uint8_t str[ROOTTABLE_TUPLE_SIZE];
    // memcpy(str, &tableTuple, sizeof(tableTuple));
    // item.row = str;
    // item.blockID = rowLocation.blockID;
    // item.rowOffset = rowLocation.rowOffset;
    // item.isDelete = false;
    // RowBufferItem *rowBufferItem = new RowBufferItem(item);
    // buf->append(rowBufferItem);
    
    return SUCCESS;
  }
  return TABLE_TUPLE_NOT_FOUND;
}

uint32_t RootTable::getTableTuple(uint64_t tableID, TableTuple &tableTuple) {
  std::map<uint64_t, TableTuple>::iterator iter;
  iter = tableTupleMap.find(tableID);
  if (iter != tableTupleMap.end()) {
    tableTuple = iter->second;
    return SUCCESS;
  }
  return TABLE_TUPLE_NOT_FOUND;
}

uint32_t RootTable::serializeRow(const TableTuple tableTuple, uint8_t *buf) {
  memcpy(buf, &tableTuple, ROOTTABLE_TUPLE_SIZE);

  return SUCCESS;
}

uint32_t RootTable::deserializeRow(const uint8_t *buf,
                                   TableTuple *tableTupleResult) {
  if (buf != NULL) {
    memcpy(tableTupleResult, buf, ROOTTABLE_TUPLE_SIZE);
    if (tableTupleResult->tableID != 0) {
      return SUCCESS;
    }
  }
  return BUFFER_EMPTY;
}

uint64_t RootTable::getTableCount() { return tableTupleMap.size(); }

}  // namespace LightTable
