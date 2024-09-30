/**
 * @file Common.cc
 * @brief Common
 * @version 0.1
 * @author SYS Lab
 * @date 2022.11.01
 */

#include "Common.h"

namespace LightTable {

int cacheReadNum = 0;
int cacheWriteNum = 0;

std::chrono::steady_clock::time_point start_time =
    std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point end_time =
    std::chrono::steady_clock::now();
double diff_sdcard_read = 0.0;
double diff_sdcard_write = 0.0;

struct timespec diff(struct timespec start, struct timespec end) {
  struct timespec temp;
  if ((end.tv_nsec - start.tv_nsec) < 0) {
    temp.tv_sec = end.tv_sec - start.tv_sec - 1;
    temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
  } else {
    temp.tv_sec = end.tv_sec - start.tv_sec;
    temp.tv_nsec = end.tv_nsec - start.tv_nsec;
  }
  return temp;
}

#ifdef IO_PROFILING
std::queue<struct timespec> Queue::io_times;
std::queue<struct timespec> Queue::io_submit_times;
#endif

#ifdef IO_PROFILING
struct timespec Queue::accumulate_io_times() {
  struct timespec sum;
  sum.tv_sec = 0;
  sum.tv_nsec = 0;

  while (!io_times.empty()) {
    sum.tv_sec += io_times.front().tv_sec;
    if (1000000000 <= sum.tv_nsec + io_times.front().tv_nsec) {
      sum.tv_sec++;
      sum.tv_nsec = sum.tv_nsec + io_times.front().tv_nsec - 1000000000;
    } else {
      sum.tv_nsec += io_times.front().tv_nsec;
    }
    io_times.pop();
  }

  return sum;
}

struct timespec Queue::accumulate_io_submit_times() {
  struct timespec sum;
  sum.tv_sec = 0;
  sum.tv_nsec = 0;

  while (!io_submit_times.empty()) {
    sum.tv_sec += io_submit_times.front().tv_sec;
    if (1000000000 <= sum.tv_nsec + io_submit_times.front().tv_nsec) {
      sum.tv_sec++;
      sum.tv_nsec = sum.tv_nsec + io_submit_times.front().tv_nsec - 1000000000;
    } else {
      sum.tv_nsec += io_submit_times.front().tv_nsec;
    }
    io_submit_times.pop();
  }

  return sum;
}
#endif

}  // namespace LightTable
