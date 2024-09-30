/**
 * @file TableStorage.h
 * @brief TableStorage
 * @version 0.1
 * @author SYS Lab
 * @date 2022.11.01
 */

#ifndef DAEMONPROCESS_H
#define DAEMONPROCESS_H

#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <algorithm>
#include <map>
#include <iomanip>
#include <iostream>
#include <thread>

// #include <sys/types.h>
// #include <sys/socket.h>
// #include <arpa/inet.h>
// #include <netinet/in.h>
// #include <netinet/tcp.h>

#include "RootTable.h"
#include "SegmentManager.h"
#include "Common.h"
#include "CommonTable.h"
#include "Error.h"
#include "Driver.h"
#include "Table.h"

#define TABLE_DEBUG_ENABLE
#define PROJECT_NAME "TanleStorage"

#ifndef TABLE_PRINT
#define TABLE_PRINT(...) printf(__VA_ARGS__)
#endif
#define TABLE_LOG_PREFIX1()                                          \
  TABLE_PRINT("[\e[1;34m%s\033[0m] \e[1;31m%s\033[0m", PROJECT_NAME, \
              __FUNCTION__)
#define TABLE_LOG_PREFIX2() TABLE_PRINT(" ")
#define TABLE_LOG_PREFIX() \
  TABLE_LOG_PREFIX1();     \
  TABLE_LOG_PREFIX2()

#ifdef TABLE_DEBUG_ENABLE
#define TABLE_DEBUG(...)                                      \
  TABLE_LOG_PREFIX();                                         \
  TABLE_PRINT("(\e[1;32m%s:%d\033[0m) ", __FILE__, __LINE__); \
  TABLE_PRINT(__VA_ARGS__)
#else
#define TABLE_DEBUG(...)
#endif

namespace LightTable {

enum EventType {
  CREATE_TABLE_EVENT = 10001,
  INSERT_ROW_EVENT,
  DELETE_ROW_EVENT,
  SELECT_ROW_EVENT,
#ifdef YCSB_TEST
  SCAN_EVENT,
#endif
  UPDATE_ROW_EVENT,
#ifdef YCSB_TEST
  UPDATE_COLUMNS_EVENT,
#endif
  DROP_TABLE_EVENT,
  RESPOND_CLIENT_REQUEST,
  STOP_SERVICE_EVENT,
#ifdef YCSB_TEST
  UNKNOWN_EVENT,
#endif
};

typedef struct message {
  uint32_t size;
  EventType eventType;
  uint8_t data[NETWORK_MESSAGE_DATA_MAX_SIZE];
} Message;

class TableStorage {
 public:
  static TableStorage *getInstance(const char *ipAddress,
                                   const uint32_t portNum,
                                   const char *path = METAPATH) {
    static TableStorage *daemonProcess;

    // To determine whether the first call.
    if (daemonProcess == NULL) {
      daemonProcess = new TableStorage(ipAddress, portNum, path);
    }

    return daemonProcess;
  }

  uint32_t createTable(const char *tableName, SegmentType segmentType,
                       std::vector<Schema::SchemaEntry> &schema);

  uint32_t dropTable(const char *tableName);

  uint32_t insertTuple(const char *tableName, const uint8_t *row);

  uint32_t deleteTuple(const char *tableName, const char *primaryKey);

  uint32_t updateTuple(const char *tableName, const char *primaryKey,
                       const char *columnName, const uint8_t *value);

  uint32_t selectTuple(const char *tableName, const char *primaryKey,
                       uint8_t *row);

  uint32_t initTableStorage();
  uint32_t closeTableStorage();

 private:
  TableStorage(const char *ipAddress, const uint32_t portNum, const char *path);
  ~TableStorage();

  RootTable *rootTable;
  std::map<uint64_t, Table *> tableMap;
  SegmentManager *segmentManager;
  const char *LightTableIPAddress;
  const uint32_t LightTablePortNum;

  // int32_t socketfd;
  // MetaHandle metaHandle;
  // Buffer *buf;
  // QueryCache *queryCache;
  // BlockPreload *blockPreload;

  friend class Benchmark;
};

}  // namespace LightTable

#endif  // DAEMONPROCESS_H
