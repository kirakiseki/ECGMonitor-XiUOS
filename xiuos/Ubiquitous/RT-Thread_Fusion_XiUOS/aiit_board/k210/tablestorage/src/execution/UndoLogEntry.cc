/**
 * @file UndoLogEntry.cc
 * @brief UndoLogEntry
 * @version 0.1
 * @author SYS Lab
 * @date 2022.11.01
 */

#include "UndoLogEntry.h"

#include <string.h>

namespace LightTable {

UndoLogEntry::UndoLogEntry(uint64_t tableID, const char *primaryKey,
                           uint64_t keySize, char *tuple, uint64_t tupleSize,
                           uint64_t blockID, uint64_t offset)
    : BufferItem(tableID),
      blockID(blockID),
      offset(offset),
      tupleSize(tupleSize),
      keySize(keySize),
      isExpired(false) {
  memcpy(this->tuple, tuple, tupleSize);
  memcpy(this->primaryKey, primaryKey, keySize);
}

UndoLogEntry::~UndoLogEntry() {
  delete[] primaryKey;
  delete[] tuple;
}

uint32_t UndoLogEntry::getTuple(uint64_t tableID, const char *primaryKey,
                                char *tuple, uint64_t &blockID,
                                uint64_t &offset) {
  if (this->tableID != tableID || strcmp(this->primaryKey, primaryKey) != 0) {
    return ERROR_TABLE;
  }

  memcpy(tuple, this->tuple, tupleSize);
  blockID = this->blockID;
  offset = this->offset;
  return SUCCESS;
}

void UndoLogEntry::expireUndoLog() { isExpired = true; }

char *UndoLogEntry::getPrimaryKey() { return this->primaryKey; }

bool UndoLogEntry::getIsExpired(UndoLogEntry *logEntry) {
  return logEntry->isExpired;
}

}  // namespace LightTable
