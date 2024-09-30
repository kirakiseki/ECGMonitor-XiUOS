/**
 * @file Common.h
 * @brief parameter configuration
 * @version 0.1
 * @author SYS Lab
 * @date 2022.11.01
 */
#ifndef COMMON_H
#define COMMON_H

#include <rtconfig.h>
#include <time.h>

#include <chrono>
#include <queue>

namespace LightTable {

#define YCSB_TEST

extern int cacheReadNum;
extern int cacheWriteNum;

extern std::chrono::steady_clock::time_point start_time;
extern std::chrono::steady_clock::time_point end_time;
extern double diff_sdcard_read;
extern double diff_sdcard_write;

#define SYSTEM_MAX_CORES 0
#define SYSTEM_MAX_IO_QUEUES 8

#ifndef YCSB_TEST
#define NETWORK_MESSAGE_DATA_MAX_SIZE 4088
#define NETWORK_TRANSMIT_MAX_SIZE 4096
#else
#define NETWORK_MESSAGE_DATA_MAX_SIZE 51192
#define NETWORK_TRANSMIT_MAX_SIZE 51200
#endif

#define SDCARD_TEST_WRITE_BLOCKID 200
#define SDCARD_TEST_READ_BLOCKID 1000
#define SDCARD_TEST_NUM 10000

#define HASH_BUCKET_SIZE 50
#define CACHE_SIZE 500

#define SEGMENT_TYPE_SMALL_BITMAP_SIZE 32768
#define SEGMENT_TYPE_MEDIUM_BITMAP_SIZE 4096
#ifndef YCSB_TEST
#define SEGMENT_TYPE_BIG_BITMAP_SIZE 512
#else
#define SEGMENT_TYPE_BIG_BITMAP_SIZE 512
#endif

#define SEGMENT_TYPE_SMALL_START 64
#define SEGMENT_TYPE_MEDIUM_START 2097216
#define SEGMENT_TYPE_BIG_START 6291528

#define SEGMENT_TYPE_SMALL_CELL_SIZE 4096
#define SEGMENT_TYPE_MEDIUM_CELL_SIZE 65536
#ifndef YCSB_TEST
#define SEGMENT_TYPE_BIG_CELL_SIZE 2097152
#else
#define SEGMENT_TYPE_BIG_CELL_SIZE 65536000
#endif

#define PREFETCH_BLOCK_SERIALIZED_LENGTH 33

#define MAX_BRANCH_COUNT 4

#define PRELOAD_BLOCK_COUNT 10
#define PRELOAD_CHECK_INTERVAL_US 50

#define BLOCK_SIZE 512

#ifndef YCSB_TEST
#define BUFFER_SIZE 200
#else
#define BUFFER_SIZE 300000
#endif

#define CACHED_PAGE_COUNT 100

#define ROOTTABLE_TUPLE_SIZE sizeof(RootTable::TableTuple)

#define TABLE_ID_SIZE sizeof(uint64_t)

#define SYSTEM_TABLE_COUNT 1
#define ROOTTABLE_TABLE_ID 1
#define ROOTTABLE_SEGMENT_ID 1
#define ROOTTABLE_FIRST_BLOCK_ID 14680164 

#define TABLE1_META_BLOCKID 1000
#define TABLE1_META_LOG_BLOCKID 10000
#define TABLE1_INDEX_BLOCKID 2000
#define TABLE1_INDEX_LOG_BLOCKID 20000
#define TABLE1_DATA_LOG_BLOCKID 30000
#define META_ENTRY_NUM 8
#define META_ENTRY_SIZE 64
#define BUFFER_MAX 4096
#define COMMON_TABLE_START_ID 101
#define YCSB_TABLE_ID 100
#define TABLE_NAME_LENGTH 32
#define PRIMARY_KEY_LENGTH 128
#define FILE_PATH_LENGTH 1024
#define FILE_NAME_LENGTH 128
#define USER_NAME_LENGTH 32
#define COLUMN_NAME_LENGTH 128
#define COLUMN_TYPE_NAME_LENGTH 32

#define PREFETCH_BLOCK_SERIALIZED_LENGTH 33
#define ROWMAP_SERIALIZED_LENGTH 42
#define SCHEMA_ENTRY_SERIALIZED_LENGTH 181

#define MAX_META_LENGTH 2048

#define MAX_PATH_LENGTH 128
const char METAPATH[MAX_PATH_LENGTH] = "../../data";

struct timespec diff(struct timespec start, struct timespec end);

class Queue {
 public:
#ifdef IO_PROFILING
  static struct timespec accumulate_io_times();
  static struct timespec accumulate_io_submit_times();
#endif

#ifdef IO_PROFILING
  static std::queue<struct timespec> io_times;
  static std::queue<struct timespec> io_submit_times;
#endif
};

// A macro to disallow the copy constructor and operator= functions
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName &) = delete;     \
  TypeName &operator=(const TypeName &) = delete;
#endif

}  // namespace LightTable

#endif  // COMMON_H
