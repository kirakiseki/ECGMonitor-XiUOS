/**
 * @file RWLock.h
 * @brief RWLock
 * @version 0.1
 * @author SYS Lab
 * @date 2022.11.01
 */
#ifndef RWLOCK_H
#define RWLOCK_H

#include <stdint.h>
#include <stdio.h>

#include <atomic>

namespace LightTable {

using version_lock_t = uint8_t;
#define LOAD_ORDER std::memory_order_acquire
#define STORE_ORDER std::memory_order_release

static_assert(sizeof(version_lock_t) == 1, "Lock must be 1 byte.");
static constexpr version_lock_t CLIENT_BIT = 0b10000000;
static constexpr version_lock_t NO_CLIENT_BIT = 0b01111111;
static constexpr version_lock_t USED_BIT = 0b01000000;
static constexpr version_lock_t UNLOCKED_BIT = 0b11111110;

#define IS_LOCKED(lock) ((lock)&1)

class RWLock {
  std::atomic<version_lock_t> version_lock;

 public:
  bool lock(const bool blocking = true) {
    version_lock_t lock_value = version_lock.load(LOAD_ORDER);
    // Compare and swap until we are the thread to set the lock bit
    lock_value &= UNLOCKED_BIT;
    while (!version_lock.compare_exchange_weak(lock_value, lock_value + 1)) {
      lock_value &= UNLOCKED_BIT;
      if (!blocking) {
        return false;
      }
    }
    return true;
  }

  void unlock() {
    const version_lock_t current_version = version_lock.load(LOAD_ORDER);
    version_lock_t new_version = (current_version + 1) % USED_BIT;
    new_version |= USED_BIT;
    new_version |= (current_version & CLIENT_BIT);
    version_lock.store(new_version, STORE_ORDER);
  }
};
}  // namespace LightTable

#endif  // HASHTABLE_H