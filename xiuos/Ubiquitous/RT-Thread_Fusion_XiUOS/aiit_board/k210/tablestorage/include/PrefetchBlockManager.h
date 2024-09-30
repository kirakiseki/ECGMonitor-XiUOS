/**
 * @file PrefetchBlockManager.h
 * @brief PrefetchBlockManager
 * @version 0.1
 * @author SYS Lab
 * @date 2022.11.01
 */

#ifndef PREFETCHBLOCKMANAGER_H
#define PREFETCHBLOCKMANAGER_H

#include <stdint.h>
#include <time.h>
#include <stdlib.h>
// #include <pthread.h>

#include <map>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <utility>
// #include <mutex>

#include "Error.h"
#include "Common.h"
#include "SegmentManager.h"

namespace LightTable
{

enum PrefetchBlockStatus
{
    PREFETCH_BLOCK_STATUS_IDLE = 0,
    PREFETCH_BLOCK_STATUS_BUSY,
};

class PrefetchBlockManager
{
    public:

        typedef struct prefetchBlockManagerEntry
        {
            uint64_t blockID;
            uint64_t currentOffset;
            bool isOccupied;
            bool isCached; 
            uint64_t pageID;
        }PrefetchBlockManagerEntry;

        PrefetchBlockManager(uint64_t firstBlockID, uint64_t segmentID,
                             SegmentType segmentType);
        
		~PrefetchBlockManager();

        uint64_t blockAllocate(uint64_t rowSize);

        uint64_t getNextBlock();
        
        uint32_t setBlockState(uint64_t blockID, bool isOccupied);

        uint32_t setOffset(uint64_t blockID, uint64_t currentOffset);

        uint32_t getOffset(uint64_t blockID, uint64_t &currentOffset);

        uint32_t advanceOffset(uint64_t blockID, uint64_t rowSize);

        uint32_t isOccupied(uint64_t blockID, bool &status);

        uint32_t getPrefetchBlockEntry(uint64_t blockID,
                                       PrefetchBlockManagerEntry &entry);

        uint32_t getAllPrefetchBlockEntrys(std::map<uint64_t,
                                           PrefetchBlockManagerEntry> &entrys);

        uint32_t lockBlock(uint64_t blockID);

        uint32_t unlockBlock(uint64_t blockID);

        uint32_t setIsCached(uint64_t blockID, bool isCached, uint64_t pageID);

        bool isCached(uint64_t blockID, uint64_t &pageID);

    private:
        std::map<uint64_t, PrefetchBlockManagerEntry> prefetchBlocks;

        const uint64_t segmentID;
        const SegmentType segmentType;
        const uint64_t firstBlockID;
        uint64_t blockCount;
        uint64_t currentID;

        DISALLOW_COPY_AND_ASSIGN(PrefetchBlockManager);

    public:
        friend class QueryCache;
        friend class MetaHandle;
};

} // namespace LightTable

#endif // PREFETCHBLOCKMANAGER_H
