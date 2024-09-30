/**
 * @file UndoLogEntry.h
 * @brief UndoLogEntry
 * @version 0.1
 * @author SYS Lab
 * @date 2022.11.01
 */

#ifndef UNDOLOGENTRY_H
#define UNDOLOGENTRY_H

#include <stdint.h>

#include "BufferItem.h"
#include "Common.h"
#include "Error.h"

namespace LightTable {

class UndoLogEntry : public BufferItem {
 public:
  UndoLogEntry(uint64_t tableID, const char *primaryKey, uint64_t keySize,
               char *tuple, uint64_t tupleSize, uint64_t blockID,
               uint64_t offset);

  static bool getIsExpired(UndoLogEntry *logEntry);
  void expireUndoLog();

  uint32_t getTuple(uint64_t tableID, const char *primaryKey, char *tuple,
                    uint64_t &blockID, uint64_t &offset);

  char *getPrimaryKey();

  ~UndoLogEntry();

 private:
  char *tuple;
  char *primaryKey;
  uint64_t blockID;
  uint64_t offset;
  uint64_t tupleSize;
  uint64_t keySize;
  bool isExpired;
  DISALLOW_COPY_AND_ASSIGN(UndoLogEntry);
};

}  // namespace LightTable

#endif  // UNDOLOGENTRY_H
