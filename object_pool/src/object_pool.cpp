#include "test_runner.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <queue>
#include <stdexcept>
#include <set>
using namespace std;

template <class T>
class ObjectPool {
public:
  T* Allocate() {
    if (!freed.empty()) {
      T* object = freed.front();
      freed.pop();
      vid.insert(object);
      return object;
    }
    else {
      T* object = new T();
      vid.insert(object);
      return object;
    }
  }
  T* TryAllocate() {
    if (freed.empty()) {
      return nullptr;
    }
    else {
      Allocate();
    }
  }

  void Deallocate(T* object) {
    auto it = vid.find(object);
    if (it != vid.end()) {
      T* object = *it;
      vid.erase(it);
      freed.push(object);
    }
    else {
      throw invalid_argument("");
    }
  }

  ~ObjectPool() {
    while (!freed.empty()) {
      T* object = freed.front();
      freed.pop();
      delete object;
    }
    for (auto object : vid) {
      delete object;
    }
  }

private:
  queue<T*> freed;
  set<T*> vid;
};

void TestObjectPool() {
  ObjectPool<string> pool;

  auto p1 = pool.Allocate();
  auto p2 = pool.Allocate();
  auto p3 = pool.Allocate();

  *p1 = "first";
  *p2 = "second";
  *p3 = "third";

  pool.Deallocate(p2);
  ASSERT_EQUAL(*pool.Allocate(), "second");

  pool.Deallocate(p3);
  pool.Deallocate(p1);
  ASSERT_EQUAL(*pool.Allocate(), "third");
  ASSERT_EQUAL(*pool.Allocate(), "first");

  pool.Deallocate(p1);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestObjectPool);
  return 0;
}
