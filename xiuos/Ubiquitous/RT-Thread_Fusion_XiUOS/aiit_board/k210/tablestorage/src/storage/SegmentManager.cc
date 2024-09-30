/**
 * @file SegmentManager.cc
 * @brief SegmentManager
 * @version 0.1
 * @author SYS Lab
 * @date 2022.11.01
 */

#include "SegmentManager.h"

namespace LightTable {

SegmentManager::SegmentManager() {
  int blockID, num = 0;

  smallBitmap = new uint8_t[SEGMENT_TYPE_SMALL_BITMAP_SIZE];
  mediumBitmap = new uint8_t[SEGMENT_TYPE_MEDIUM_BITMAP_SIZE];
  bigBitmap = new uint8_t[SEGMENT_TYPE_BIG_BITMAP_SIZE];

#ifdef YCSB_TEST
  blockID = SEGMENT_TYPE_BIG_START;
  while (num < SEGMENT_TYPE_BIG_BITMAP_SIZE) {
    Driver::read(bigBitmap + num, blockID);
    num += BLOCK_SIZE;
    blockID++;
  }

#else
  num = 0;
  blockID = SEGMENT_TYPE_SMALL_START;
  while (num < SEGMENT_TYPE_SMALL_BITMAP_SIZE) {
    Driver::read(smallBitmap + num, blockID);
    num += BLOCK_SIZE;
    blockID++;
  }

  num = 0;
  blockID = SEGMENT_TYPE_MEDIUM_START;
  while (num < SEGMENT_TYPE_MEDIUM_BITMAP_SIZE) {
    Driver::read(mediumBitmap + num, blockID);
    num += BLOCK_SIZE;
    blockID++;
  }

  num = 0;
  blockID = SEGMENT_TYPE_BIG_START;
  while (num < SEGMENT_TYPE_BIG_BITMAP_SIZE) {
    Driver::read(mediumBitmap + num, blockID);
    num += BLOCK_SIZE;
    blockID++;
  }
#endif
}

SegmentManager::~SegmentManager() {
  delete[] smallBitmap;
  delete[] mediumBitmap;
  delete[] bigBitmap;
}

uint32_t SegmentManager::setBitmap(SegmentType segmentType, uint64_t segmentID,
                                   SegmentStatus segmentStatus) {
  switch (segmentType) {
    case SEGMENT_TYPE_SMALL:
      return setBit(segmentID, smallBitmap, SEGMENT_TYPE_SMALL_START,
                    segmentStatus);

    case SEGMENT_TYPE_MEDIUM:
      return setBit(segmentID, mediumBitmap, SEGMENT_TYPE_MEDIUM_START,
                    segmentStatus);

    case SEGMENT_TYPE_BIG:
      return setBit(segmentID, bigBitmap, SEGMENT_TYPE_BIG_START,
                    segmentStatus);

    default:
      return SEGMENT_TYPE_ERROR;
  }
}

uint32_t SegmentManager::getIdleSegment(SegmentType segmentType,
                                        uint64_t &segmentID,
                                        uint64_t &firstBlockID) {
  uint32_t stateCode;

  switch (segmentType) {
    case SEGMENT_TYPE_SMALL:
      stateCode =
          findFirstIdle(smallBitmap, SEGMENT_TYPE_SMALL_BITMAP_SIZE, segmentID);

      if (stateCode != SUCCESS) return stateCode;

      firstBlockID = SEGMENT_TYPE_SMALL_START +
                     (SEGMENT_TYPE_SMALL_CELL_SIZE / BLOCK_SIZE) * segmentID;

      return setBit(segmentID, smallBitmap, SEGMENT_TYPE_SMALL_START,
                    SEGMENT_STATUS_BUSY);

    case SEGMENT_TYPE_MEDIUM:
      stateCode = findFirstIdle(mediumBitmap, SEGMENT_TYPE_MEDIUM_BITMAP_SIZE,
                                segmentID);

      if (stateCode != SUCCESS) return stateCode;

      firstBlockID = SEGMENT_TYPE_MEDIUM_START +
                     (SEGMENT_TYPE_MEDIUM_CELL_SIZE / BLOCK_SIZE) * segmentID;

      return setBit(segmentID, mediumBitmap, SEGMENT_TYPE_MEDIUM_START,
                    SEGMENT_STATUS_BUSY);

    case SEGMENT_TYPE_BIG:
      stateCode =
          findFirstIdle(bigBitmap, SEGMENT_TYPE_BIG_BITMAP_SIZE, segmentID);

      if (stateCode != SUCCESS) return stateCode;

      firstBlockID = SEGMENT_TYPE_BIG_START +
                     (SEGMENT_TYPE_BIG_CELL_SIZE / BLOCK_SIZE) * segmentID;

      return setBit(segmentID, bigBitmap, SEGMENT_TYPE_BIG_START,
                    SEGMENT_STATUS_BUSY);

    default:
      return SEGMENT_TYPE_ERROR;
  }
}

uint32_t SegmentManager::setBit(uint64_t segmentID, uint8_t *buf,
                                uint64_t segmentStart,
                                SegmentStatus segmentStatus) {
  uint32_t integer, remainder;
  uint8_t tmp = 1;

  integer = segmentID / 8;
  remainder = segmentID % 8;

  tmp = tmp << remainder;
  if (segmentStatus == SEGMENT_STATUS_BUSY) {
    buf[integer] |= tmp;
  } else if (segmentStatus == SEGMENT_STATUS_IDLE) {
    buf[integer] &= ~tmp;
  } else {
    return SEGMENT_STATUS_ERROR;
  }

  uint64_t num = integer / BLOCK_SIZE;
  uint64_t blockID = num + segmentStart;

  Driver::write(buf + num * BLOCK_SIZE, blockID);

  return SUCCESS;
}

uint32_t SegmentManager::findFirstIdle(uint8_t *buf, uint64_t segmentNum,
                                       uint64_t &segmentID) {
  uint64_t num, place;
  uint8_t res, tmp, isfound;

  for (isfound = num = 0; num < segmentNum; num++) {
    for (place = 0; place < 8; place++) {
      tmp = 0x01 << place;
      tmp = tmp & buf[num];
      res = (tmp >> place);
      if (res == 0) {
        isfound = 1;
        break;
      }
    }
    if (isfound == 1) break;
  }

  if (isfound == 0) {
    return SEGMENT_IS_EXHAUSTED;
  }
  segmentID = num * 8 + place;

  return SUCCESS;
}

}  // namespace LightTable
