/**
 * @file Schema.cc
 * @brief table schema
 * @version 0.1
 * @author SYS Lab
 * @date 2022.11.01
 */

#include "Schema.h"

namespace LightTable {

Schema::Schema() {
  primaryKeySchema.columnID = 1;
  primaryKeySchema.isPrimaryKey = true;
  columnCount = 0;
  entrySize = 0;
}

Schema::Schema(std::vector<SchemaEntry> schemaEntrys) {
  columnCount = 0;
  entrySize = 0;
  primaryKeySchema.columnID = 1;
  primaryKeySchema.isPrimaryKey = true;
  std::vector<SchemaEntry>::iterator iter = schemaEntrys.begin();
  while (iter != schemaEntrys.end()) {
    appendEntry(*iter);
    iter++;
  }
}

uint32_t Schema::appendEntry(SchemaEntry &schemaEntry) {
  if (schemaEntry.columnID != columnCount + 1) {
    return COLUMN_ID_NOT_CONTINUOUS;
  }
  std::pair<std::map<uint64_t, SchemaEntry>::iterator, bool> ret;
  ret = schemaEntrys.insert(std::pair<uint64_t, Schema::SchemaEntry>(
      schemaEntry.columnID, schemaEntry));

  if (ret.second == false) {
    return ADD_SCHEMA_ENTRY_ERROR;
  }

  if (schemaEntry.isPrimaryKey) {
    primaryKeySchema = schemaEntry;
  }

  columnCount++;
  entrySize = entrySize + schemaEntry.length;
  return SUCCESS;
}

uint32_t Schema::deleteEntry(uint64_t columnID) {
  if (columnID == primaryKeySchema.columnID) {
    return DELETE_PARIMARY_KEY_COLUMN;
  }

  uint64_t newColumnID = columnID;
  SchemaEntry schemaEntry;

  std::map<uint64_t, SchemaEntry>::iterator iter;
  iter = schemaEntrys.find(columnID);
  if (iter != schemaEntrys.end()) {
    schemaEntry = iter->second;
    schemaEntrys.erase(iter);
    entrySize = entrySize - schemaEntry.length;
    columnCount--;

    if (columnCount > 0) {
      iter = schemaEntrys.find(newColumnID + 1);

      while (iter != schemaEntrys.end()) {
        schemaEntry = iter->second;
        schemaEntry.columnID = newColumnID;

        schemaEntrys.insert(
            std::pair<uint64_t, Schema::SchemaEntry>(newColumnID, schemaEntry));

        schemaEntrys.erase(iter);
        newColumnID++;
        iter = schemaEntrys.find(newColumnID + 1);
      }
    }
    return SUCCESS;
  }
  return SCHEMA_ENTRY_NOT_FOUND;
}

uint32_t Schema::alterEntry(uint64_t columnID, SchemaEntry schemaEntry) {
  if (schemaEntry.columnID != columnID) {
    return COLUMN_ID_NOT_CONTINUOUS;
  }

  std::map<uint64_t, SchemaEntry>::iterator iter;
  iter = schemaEntrys.find(columnID);

  entrySize = entrySize - (iter->second).length;

  if (iter != schemaEntrys.end()) {
    iter->second = schemaEntry;
    primaryKeySchema = schemaEntry;
    entrySize = entrySize + (iter->second).length;
    return SUCCESS;
  }
  return SCHEMA_ENTRY_NOT_FOUND;
}

uint32_t Schema::getColumnID(const char *columnName, uint64_t &columnID) {
  std::map<uint64_t, SchemaEntry>::iterator iter = schemaEntrys.begin();
  while (iter != schemaEntrys.end()) {
    if (strncmp(iter->second.columnName, columnName, iter->second.length) ==
        0) {
      columnID = (iter->second).columnID;
      return SUCCESS;
    }
    iter++;
  }
  return COLUMN_ITEM_NOT_FOUND;
}

uint32_t Schema::queryEntry(uint64_t columnID,
                            Schema::SchemaEntry &schemaEntry) {
  std::map<uint64_t, SchemaEntry>::iterator iter;
  iter = schemaEntrys.find(columnID);

  if (iter != schemaEntrys.end()) {
    schemaEntry = iter->second;
    return SUCCESS;
  }

  schemaEntry.columnID = 0;
  return SCHEMA_ENTRY_NOT_FOUND;
}

uint32_t Schema::queryAllEntry(
    std::map<uint64_t, SchemaEntry> &allSchemaEntrys) {
  allSchemaEntrys = schemaEntrys;
  if (schemaEntrys.empty() == false) {
    return SUCCESS;
  }
  return SCHEMA_ENTRY_NOT_FOUND;
}

uint64_t Schema::getEntrySize() { return entrySize; }

void Schema::setEntrySize(uint64_t entrySize) { this->entrySize = entrySize; }

uint64_t Schema::getColumnCount() { return columnCount; }

void Schema::setColumnCount(uint64_t columnCount) {
  this->columnCount = columnCount;
}

uint32_t Schema::getPrimaryKeyLength() { return primaryKeySchema.length; }

Schema::SchemaEntry &Schema::getPrimaryKeySchema() { return primaryKeySchema; }

uint32_t Schema::drop() {
  schemaEntrys.clear();
  primaryKeySchema.length = 0;
  columnCount = 0;
  entrySize = 0;
  return SUCCESS;
}

}  // namespace LightTable
