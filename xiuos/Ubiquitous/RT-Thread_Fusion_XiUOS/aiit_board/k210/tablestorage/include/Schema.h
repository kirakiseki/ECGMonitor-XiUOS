/**
 * @file Schema.h
 * @brief table schema
 * @version 0.1
 * @author SYS Lab
 * @date 2022.11.01
 */

#ifndef SCHEMA_H
#define SCHEMA_H

#include <stdint.h>
#include <string.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <string>

#include "Common.h"
#include "Error.h"

namespace LightTable {

class Schema {
 public:
  typedef struct schemaEntry {
    uint64_t columnID;
    char columnName[COLUMN_NAME_LENGTH];
    char type[COLUMN_TYPE_NAME_LENGTH];
    uint32_t length;
    bool isPrimaryKey;
    bool isEmpty;

    schemaEntry &operator=(schemaEntry &another) {
      this->columnID = another.columnID;
      memcpy(this->columnName, another.columnName, COLUMN_NAME_LENGTH);
      memcpy(this->type, another.type, COLUMN_TYPE_NAME_LENGTH);
      this->length = another.length;
      this->isPrimaryKey = another.isPrimaryKey;
      this->isEmpty = another.isEmpty;

      return *this;
    }

  } SchemaEntry;

  Schema();

  Schema(std::vector<SchemaEntry> schemaEntrys);

  uint32_t appendEntry(SchemaEntry &schemaEntry);

  uint32_t deleteEntry(uint64_t columnID);

  uint32_t alterEntry(uint64_t columnID, SchemaEntry schemaEntry);

  uint32_t getColumnID(const char *columnName, uint64_t &columnID);

  uint32_t queryEntry(uint64_t columnID, Schema::SchemaEntry &schemaEntry);

  uint32_t queryAllEntry(std::map<uint64_t, SchemaEntry> &allSchemaEntrys);

  uint64_t getEntrySize();

  void setEntrySize(uint64_t entrySize);

  uint32_t getPrimaryKeyLength();

  uint64_t getColumnCount();

  void setColumnCount(uint64_t columnCount);

  Schema::SchemaEntry &getPrimaryKeySchema();

  uint32_t drop();

 private:
  std::map<uint64_t, SchemaEntry> schemaEntrys;
  SchemaEntry primaryKeySchema;
  uint64_t columnCount;
  uint64_t entrySize;

  DISALLOW_COPY_AND_ASSIGN(Schema);

 public:
  friend class MetaHandle;
};

}  // namespace LightTable

#endif  // SCHEMA_H
