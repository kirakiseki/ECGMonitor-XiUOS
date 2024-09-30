/**
 * @file SegmentManager.h
 * @brief SegmentManager
 * @version 0.1
 * @author SYS Lab
 * @date 2022.11.01
 */

#ifndef SEGMENTMANAGER_H
#define SEGMENTMANAGER_H

#include <stdint.h>

#include "Common.h"
#include "Error.h"
#include "Driver.h"
#include "Common.h"

namespace LightTable {

enum SegmentType {
  SEGMENT_TYPE_SMALL = 101,
  SEGMENT_TYPE_MEDIUM,
  SEGMENT_TYPE_BIG,
};

enum SegmentStatus {
  SEGMENT_STATUS_IDLE = 0,
  SEGMENT_STATUS_BUSY,
};

class SegmentManager {
 public:
  SegmentManager();
  ~SegmentManager();

  uint32_t setBitmap(SegmentType segmentType, uint64_t segmentID,
                     SegmentStatus segmentStatus);

  uint32_t getIdleSegment(SegmentType segmentType, uint64_t &segmentID,
                          uint64_t &firstBlockID);

 private:
  uint32_t setBit(uint64_t segmentID, uint8_t *buf, uint64_t segmentStart,
                  SegmentStatus segmentStatus);

  uint32_t findFirstIdle(uint8_t *buf, uint64_t segmentNum,
                         uint64_t &segmentID);

  uint8_t *bigBitmap;
  uint8_t *mediumBitmap;
  uint8_t *smallBitmap;

  DISALLOW_COPY_AND_ASSIGN(SegmentManager);
};

}  // namespace LightTable

#endif  // SEGMENTMANAGER_H
