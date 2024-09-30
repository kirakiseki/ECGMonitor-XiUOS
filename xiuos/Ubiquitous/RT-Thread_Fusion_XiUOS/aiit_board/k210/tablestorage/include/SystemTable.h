/**
 * @file SystemTable.h
 * @brief SystemTable
 * @version 0.1
 * @author SYS Lab
 * @date 2022.11.01
 */

#ifndef SYSTEMTABLE_H
#define SYSTEMTABLE_H

#include "Table.h"

namespace LightTable {

class SystemTable : public Table {
 public:
  SystemTable(uint64_t tableID, const char *tableName, uint64_t firstBlockID,
              uint64_t segmentID, SegmentType segmentType,
              std::vector<Schema::SchemaEntry> schemaEntrys,
              bool usePrimaryKeyIndex);

  SystemTable(uint64_t tableID, const char *tableName, uint64_t firstBlockID,
              uint64_t segmentID, SegmentType segmentType,
              bool usePrimaryKeyIndex);

 protected:
  DISALLOW_COPY_AND_ASSIGN(SystemTable);
};

}  // namespace LightTable

#endif  // SYSTEMTABLE_H
