/**
 * @file CommonTable.h
 * @brief CommonTable
 * @version 0.1
 * @author SYS Lab
 * @date 2022.11.01
 */

#ifndef COMMONTABLE_H
#define COMMONTABLE_H

#include "Table.h"

namespace LightTable {

class CommonTable : public Table {
 public:
  CommonTable(uint64_t tableID, const char *tableName, uint64_t firstBlockID,
              uint64_t segmentID, SegmentType segmentType,
              std::vector<Schema::SchemaEntry> schemaEntrys,
              bool usePrimaryKeyIndex = false);

  CommonTable(uint64_t tableID, const char *tableName, uint64_t firstBlockID,
              uint64_t segmentID, SegmentType segmentType,
              bool usePrimaryKeyIndex = false);

  friend class MetaHandle;

 protected:
  DISALLOW_COPY_AND_ASSIGN(CommonTable);
};

}  // namespace LightTable

#endif  // COMMONTABLE_H
