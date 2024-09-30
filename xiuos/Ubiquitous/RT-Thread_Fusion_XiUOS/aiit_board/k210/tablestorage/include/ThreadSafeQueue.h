/**
 * @file ThreadSafeQueue.h
 * @brief ThreadSafeQueue
 * @version 0.1
 * @author SYS Lab
 * @date 2022.11.01
 */

#include <pthread.h>

#include <iostream>

#include "Common.h"

using namespace std;

namespace LightTable {

template <class object>
class ThreadSafeQueue {
 private:
  pthread_mutex_t m_lock;
  int m_front;
  int m_rear;
  object m_data[BUFFER_SIZE];

 public:
  ThreadSafeQueue() : m_front(0), m_rear(0) {
    pthread_mutex_init(&m_lock, NULL);
  }

  bool EnQueue(object data) {
    pthread_mutex_lock(&m_lock);
    if (isFull()) {
      //        cout<<"The queue is full!"<<endl;
      pthread_mutex_unlock(&m_lock);
      return false;
    }
    m_data[m_rear] = data;
    m_rear = (m_rear + 1) % BUFFER_SIZE;
    pthread_mutex_unlock(&m_lock);
    return true;
  }

  bool DeQueue(object& data) {
    pthread_mutex_lock(&m_lock);
    if (isEmpty()) {
      pthread_mutex_unlock(&m_lock);
      return false;
    }
    data = m_data[m_front];
    m_front = (m_front + 1) % BUFFER_SIZE;
    pthread_mutex_unlock(&m_lock);
    return true;
  }

  bool top(object& data) {
    pthread_mutex_lock(&m_lock);
    if (isEmpty()) {
      pthread_mutex_unlock(&m_lock);
      return false;
    }
    data = m_data[m_front];
    pthread_mutex_unlock(&m_lock);
    return true;
  }

  bool isFull() {
    if ((m_rear + 1) % BUFFER_SIZE == m_front) return true;
    return false;
  }

  bool isEmpty() {
    if (m_rear == m_front) return true;
    return false;
  }

  ~ThreadSafeQueue() { pthread_mutex_destroy(&m_lock); }
};

}  // namespace LightTable
