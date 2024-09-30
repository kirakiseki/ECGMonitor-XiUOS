/**
 * @file TableStorage.cc
 * @brief TableStorage
 * @version 0.1
 * @author SYS Lab
 * @date 2022.11.01
 */

#include "TableStorage.h"

// #include "ThreadPool.h"
// #include "Buffer.h"

// static struct timespec time1, time2, time3;
// static uint32_t coreNum = 0;

namespace LightTable {

TableStorage::TableStorage(const char *ipAddress, const uint32_t portNum,
                           const char *path)
    :  // metaHandle(path),
      LightTableIPAddress(ipAddress),
      LightTablePortNum(portNum) {

  segmentManager = new SegmentManager();
  rootTable = new RootTable();

  std::cout << "INIT TableStorage SUCCESS" << "\n";
  // threadPool = ThreadPool::getInstance(SYSTEM_MAX_CORES);
  // buf        = Buffer::getInstance();
  // queryCache = QueryCache::getInstance(&tableMap);
  // blockPreload = BlockPreload::getInstance();
  // blockPreload->setParameters(queryCache);
  // buf->setParameters(&metaHandle, rootTable);
}

uint32_t TableStorage::createTable(const char *tableName,
                                   SegmentType segmentType,
                                   std::vector<Schema::SchemaEntry> &schema) {
  uint64_t segmentID = 0;
  uint64_t firstBlockID = 0;
  uint64_t tableID =
      tableMap.size() + COMMON_TABLE_START_ID - SYSTEM_TABLE_COUNT;
  uint32_t err =
      segmentManager->getIdleSegment(segmentType, segmentID, firstBlockID);
  if (err != SUCCESS) {
    return err;
  }

  CommonTable *commonTable = new CommonTable(tableID, tableName, firstBlockID,
                                             segmentID, segmentType, schema);
  std::pair<std::map<uint64_t, Table *>::iterator, bool> ret;
  ret = tableMap.insert(std::pair<uint64_t, Table *>(tableID, commonTable));
  if (false == ret.second) {
    return ERROR_TABLE;
  }
  time_t timeNow = time(NULL);
  struct tm *now = localtime(&timeNow);
  RootTable::TableTuple tuple;
  tuple.tableID = tableID;
  memcpy(tuple.tableName, tableName, TABLE_NAME_LENGTH);
  tuple.firstBlockID = firstBlockID;
  tuple.segmentType = segmentType;
  tuple.segmentID = segmentID;
  memcpy(tuple.user, "user", 5);
  tuple.time = *now;
  err = rootTable->appendTableInfo(tuple);

  if (err == SUCCESS) std::cout << "create table: " << tableName << std::endl;
  return err;
}

uint32_t TableStorage::dropTable(const char *tableName) {
  uint64_t tableID = 0;
  uint32_t result = rootTable->tableNameToTableID(tableName, tableID);
  if (SUCCESS != result) return result;

  std::map<uint64_t, Table *>::iterator iter = tableMap.find(tableID);
  Table *commonTable = iter->second;
  commonTable->drop();
  tableMap.erase(tableID);
  result = rootTable->deleteTableInfo(tableID);

  if (result == SUCCESS) std::cout << "drop table: " << tableName << std::endl;
  return result;
}

uint32_t TableStorage::insertTuple(const char *tableName, const uint8_t *row) {
  uint64_t tableID = 0;
  uint32_t result = 0;

#ifndef SDCARD_TEST
  result = rootTable->tableNameToTableID(tableName, tableID);
  if (SUCCESS != result) {
    return result;
  }
#else
  tableID = YCSB_TABLE_ID;
#endif

  std::map<uint64_t, Table *>::iterator iter = tableMap.find(tableID);
  Table *commonTable = iter->second;

#ifndef TABLE_STORAGE_LOGGING
  result = commonTable->insertRow(row);
#else
  result = commonTable->insertRow_logging(row);
#endif

  return result;
}

uint32_t TableStorage::deleteTuple(const char *tableName,
                                   const char *primaryKey) {
  uint64_t tableID = 0;
  uint32_t result = rootTable->tableNameToTableID(tableName, tableID);
  if (SUCCESS != result) return result;

  std::map<uint64_t, Table *>::iterator iter = tableMap.find(tableID);
  Table *commonTable = iter->second;
  result = commonTable->deleteRow(primaryKey);

  if (result == SUCCESS)
    std::cout << "delete a tuple in " << tableName << " success!" << std::endl;
  return result;
}

uint32_t TableStorage::updateTuple(const char *tableName,
                                   const char *primaryKey,
                                   const char *columnName,
                                   const uint8_t *value) {
  uint64_t tableID = 0;
  uint32_t result = rootTable->tableNameToTableID(tableName, tableID);
  if (SUCCESS != result) return result;

  std::map<uint64_t, Table *>::iterator iter = tableMap.find(tableID);
  Table *commonTable = iter->second;

#ifndef TABLE_STORAGE_LOGGING
  result = commonTable->updateColumnItem(primaryKey, columnName, value);
#else
  result = commonTable->updateColumnItem_logging(primaryKey, columnName, value);
#endif

  return result;
}

uint32_t TableStorage::selectTuple(const char *tableName,
                                   const char *primaryKey, uint8_t *row) {
  uint64_t tableID = 0;
  uint32_t result = 0;

#ifndef SDCARD_TEST
  result = rootTable->tableNameToTableID(tableName, tableID);
  if (SUCCESS != result) return result;
#else
  tableID = YCSB_TABLE_ID;
#endif

  std::map<uint64_t, Table *>::iterator iter = tableMap.find(tableID);
  Table *commonTable = iter->second;
  result = commonTable->selectRow(primaryKey, row);

  return result;
}

uint32_t TableStorage::initTableStorage() {
  return SUCCESS;
}

uint32_t TableStorage::closeTableStorage() {
  return SUCCESS;
}

TableStorage::~TableStorage() {
  for (auto ite = tableMap.begin(); ite != tableMap.end(); ++ite)
    delete (ite->second);
  tableMap.clear();
  delete (rootTable);
  delete (segmentManager);
  std::cout << "close TableStorage success!\n";
}

}  // namespace LightTable
