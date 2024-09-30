/**
 * @file BufferItem.h
 * @brief BufferItem
 * @version 0.1
 * @author SYS Lab
 * @date 2022.11.01
 */

#ifndef BUFFERITEM_H
#define BUFFERITEM_H

#include <stdint.h>

#include "Common.h"

namespace LightTable {

class BufferItem {
 public:
  BufferItem(uint64_t tableID);

  virtual ~BufferItem();

  // virtual void flush() const = 0;

  uint64_t getTableID() { return tableID; }

 protected:
  uint64_t tableID;
  DISALLOW_COPY_AND_ASSIGN(BufferItem);
};

}  // namespace LightTable

#endif  // BUFFERITEM_H
