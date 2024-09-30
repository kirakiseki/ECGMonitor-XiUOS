/**
 * @file PrefetchBlockManager.cc
 * @brief PrefetchBlockManager
 * @version 0.1
 * @author SYS Lab
 * @date 2022.11.01
 */

#include "PrefetchBlockManager.h"

namespace LightTable
{

PrefetchBlockManager::PrefetchBlockManager(uint64_t firstBlockID,
                                            uint64_t segmentID,
                                            SegmentType segmentType)
    : segmentID(segmentID), segmentType(segmentType), firstBlockID(firstBlockID), currentID(0)
{
    uint32_t num = 0;

    switch (segmentType)
    {
    case SEGMENT_TYPE_SMALL:
        num = SEGMENT_TYPE_SMALL_CELL_SIZE / BLOCK_SIZE;
        break;
    case SEGMENT_TYPE_MEDIUM:
        num = SEGMENT_TYPE_MEDIUM_CELL_SIZE / BLOCK_SIZE;
        break;
    case SEGMENT_TYPE_BIG:
        num = SEGMENT_TYPE_BIG_CELL_SIZE / BLOCK_SIZE;
        break;
    default:
        break;
    }

    blockCount = num;

#ifndef YCSB_TEST
    while (num--)
    {
        PrefetchBlockManagerEntry *entry = new PrefetchBlockManagerEntry();
        entry->blockID = firstBlockID + num;
        entry->currentOffset = 0;
        entry->isOccupied = PREFETCH_BLOCK_STATUS_IDLE;
        prefetchBlocks.insert(std::pair<uint64_t, PrefetchBlockManagerEntry>(entry->blockID, *entry));
    }
#endif

}

PrefetchBlockManager::~PrefetchBlockManager()
{
    prefetchBlocks.clear();
}

uint64_t
PrefetchBlockManager::getNextBlock()
{
    uint32_t count = blockCount - currentID;
    if (count <= 0)
    {
        return 0;
    }
    currentID++;
    return currentID + firstBlockID - 1;
}

uint64_t
PrefetchBlockManager::blockAllocate(uint64_t rowSize)
{
    if (blockCount == 0)
    {
        return 0;
    }
    // struct timespec time;
    // clock_gettime(CLOCK_REALTIME, &time);
    srand((unsigned)time(NULL));
    std::map<uint64_t, PrefetchBlockManagerEntry>::iterator iter;
    uint32_t count = blockCount - currentID;

    while (count--)
    {
        uint64_t blockID = currentID + firstBlockID;
        iter = prefetchBlocks.find(blockID);

        if (!iter->second.isOccupied)
        {
            if (rowSize < (BLOCK_SIZE - iter->second.currentOffset))
            {
                iter->second.isOccupied = PREFETCH_BLOCK_STATUS_BUSY;
                return blockID;
            }
        }

        currentID++;
    }

    return 0;
}

uint32_t
PrefetchBlockManager::setBlockState(uint64_t blockID, bool isOccupied)
{
    std::map<uint64_t, PrefetchBlockManagerEntry>::iterator iter;
    iter = prefetchBlocks.find(blockID);

    if (iter != prefetchBlocks.end())
    {
        iter->second.isOccupied = isOccupied;
        return SUCCESS;
    }
    else
    {
        return PREFETCH_BLOCK_NOT_FOUND;
    }
}

uint32_t
PrefetchBlockManager::setOffset(uint64_t blockID, uint64_t currentOffset)
{
    std::map<uint64_t, PrefetchBlockManagerEntry>::iterator iter;
    iter = prefetchBlocks.find(blockID);

    if (iter != prefetchBlocks.end())
    {
        iter->second.currentOffset = currentOffset;
        return SUCCESS;
    }
    else
    {
        return PREFETCH_BLOCK_NOT_FOUND;
    }
}

uint32_t
PrefetchBlockManager::getOffset(uint64_t blockID, uint64_t &currentOffset)
{
    std::map<uint64_t, PrefetchBlockManagerEntry>::iterator iter;
    iter = prefetchBlocks.find(blockID);

    if (iter != prefetchBlocks.end())
    {
        currentOffset = iter->second.currentOffset;
        return SUCCESS;
    }
    else
    {
        return PREFETCH_BLOCK_NOT_FOUND;
    }
}

uint32_t
PrefetchBlockManager::advanceOffset(uint64_t blockID, uint64_t rowSize)
{
    std::map<uint64_t, PrefetchBlockManagerEntry>::iterator iter;
    iter = prefetchBlocks.find(blockID);

    if (iter != prefetchBlocks.end())
    {
        iter->second.currentOffset += rowSize;
        return SUCCESS;
    }
    else
    {
        return PREFETCH_BLOCK_NOT_FOUND;
    }
}

uint32_t
PrefetchBlockManager::isOccupied(uint64_t blockID, bool &status)
{
    std::map<uint64_t, PrefetchBlockManagerEntry>::iterator iter;
    iter = prefetchBlocks.find(blockID);

    if (iter != prefetchBlocks.end())
    {
        status = iter->second.isOccupied;
        return SUCCESS;
    }
    else
    {
        return PREFETCH_BLOCK_NOT_FOUND;
    }
}

uint32_t
PrefetchBlockManager::getPrefetchBlockEntry(uint64_t blockID,
                                            PrefetchBlockManagerEntry &entry)
{
    entry = prefetchBlocks[blockID];
    return SUCCESS;
}

uint32_t
PrefetchBlockManager::getAllPrefetchBlockEntrys(std::map<uint64_t,
                                                            PrefetchBlockManagerEntry> &entrys)
{
    entrys = prefetchBlocks;
    return SUCCESS;
}

uint32_t
PrefetchBlockManager::lockBlock(uint64_t blockID)
{
    std::map<uint64_t, PrefetchBlockManagerEntry>::iterator iter;
    iter = prefetchBlocks.find(blockID);
    if (iter != prefetchBlocks.end())
    {
        if (iter->second.isOccupied == PREFETCH_BLOCK_STATUS_BUSY)
        {
            return BLOCK_OCCUPIED;
        }
        iter->second.isOccupied = PREFETCH_BLOCK_STATUS_BUSY;
        return SUCCESS;
    }
    return PREFETCH_BLOCK_NOT_FOUND;
}

uint32_t
PrefetchBlockManager::unlockBlock(uint64_t blockID)
{
    std::map<uint64_t, PrefetchBlockManagerEntry>::iterator iter;
    iter = prefetchBlocks.find(blockID);
    if (iter != prefetchBlocks.end())
    {
        if (iter->second.isOccupied == PREFETCH_BLOCK_STATUS_BUSY)
        {
            iter->second.isOccupied = PREFETCH_BLOCK_STATUS_IDLE;
        }
        return SUCCESS;
    }
    return PREFETCH_BLOCK_NOT_FOUND;
}

bool
PrefetchBlockManager::isCached(uint64_t blockID, uint64_t &pageID)
{
    std::map<uint64_t, PrefetchBlockManagerEntry>::iterator iter;
    iter = prefetchBlocks.find(blockID);

    if (iter != prefetchBlocks.end() && iter->second.isCached)
    {
        pageID = iter->second.pageID;
        return true;
    }
    return false;
}

uint32_t
PrefetchBlockManager::setIsCached(uint64_t blockID, bool isCached, uint64_t pageID)
{
    std::map<uint64_t, PrefetchBlockManagerEntry>::iterator iter;
    iter = prefetchBlocks.find(blockID);

    if (iter != prefetchBlocks.end())
    {
        iter->second.isCached = isCached;
        if (isCached)
        {
            iter->second.pageID = pageID;
        }
        return SUCCESS;
    }
    return PREFETCH_BLOCK_NOT_FOUND;
}

} // namespace LightTable
