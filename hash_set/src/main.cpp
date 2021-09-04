#include "test_runner.h"
#include "profile.h"

#include <forward_list>
#include <iterator>
#include <algorithm>

using namespace std;

template <typename Type, typename Hasher>
class HashSet {
public:
  using BucketList = forward_list<Type>;

public:
  explicit HashSet(
      size_t num_buckets,
      const Hasher& hasher = {}
  ) : hasher_(hasher)
    , data_(num_buckets)
  {
  };

  void Add(const Type& value) {
    size_t val_ind = CalculateIndex(value);
    if (!Has(value)) {
      data_[val_ind].push_front(value);
    }
  }

  bool Has(const Type& value) const {
    size_t val_ind = CalculateIndex(value);
    return FindValueIterator(value) != data_[val_ind].end();
  }

  void Erase(const Type& value) {
    size_t val_ind = CalculateIndex(value);
    data_[val_ind].remove(value);
  }

  const BucketList& GetBucket(const Type& value) const {
    size_t val_ind = CalculateIndex(value);
    return data_[val_ind];
  }

private:
  Hasher hasher_;
  vector<BucketList> data_;

  size_t CalculateIndex (const Type& value) const {
    return hasher_(value) % data_.size();
  }

  typename BucketList::const_iterator FindValueIterator (Type value) const {
    size_t val_ind = CalculateIndex(value);
    return find(data_[val_ind].begin(), data_[val_ind].end(), value);
  }
};

struct IntHasher {
  size_t operator()(int value) const {
    // Это реальная хеш-функция из libc++, libstdc++.
    // Чтобы она работала хорошо, std::unordered_map
    // использует простые числа для числа бакетов
    return value;
  }
};

struct TestValue {
  int value;

  bool operator==(TestValue other) const {
    return value / 2 == other.value / 2;
  }
};

struct TestValueHasher {
  size_t operator()(TestValue value) const {
    return value.value / 2;
  }
};

void TestSmoke() {
  HashSet<int, IntHasher> hash_set(2);
  hash_set.Add(3);
  hash_set.Add(4);

  ASSERT(hash_set.Has(3));
  ASSERT(hash_set.Has(4));
  ASSERT(!hash_set.Has(5));

  hash_set.Erase(3);

  ASSERT(!hash_set.Has(3));
  ASSERT(hash_set.Has(4));
  ASSERT(!hash_set.Has(5));

  hash_set.Add(3);
  hash_set.Add(5);

  ASSERT(hash_set.Has(3));
  ASSERT(hash_set.Has(4));
  ASSERT(hash_set.Has(5));
}

void TestEmpty() {
  HashSet<int, IntHasher> hash_set(10);
  for (int value = 0; value < 10000; ++value) {
    ASSERT(!hash_set.Has(value));
  }
}

void TestIdempotency() {
  HashSet<int, IntHasher> hash_set(10);
  hash_set.Add(5);
  ASSERT(hash_set.Has(5));
  hash_set.Add(5);
  ASSERT(hash_set.Has(5));
  hash_set.Erase(5);
  ASSERT(!hash_set.Has(5));
  hash_set.Erase(5);
  ASSERT(!hash_set.Has(5));
}

void TestEquivalence() {
  HashSet<TestValue, TestValueHasher> hash_set(10);
  hash_set.Add(TestValue{2});
  hash_set.Add(TestValue{3});

  ASSERT(hash_set.Has(TestValue{2}));
  ASSERT(hash_set.Has(TestValue{3}));

  const auto& bucket = hash_set.GetBucket(TestValue{2});
  const auto& three_bucket = hash_set.GetBucket(TestValue{3});
  ASSERT_EQUAL(&bucket, &three_bucket);

  ASSERT_EQUAL(1, distance(begin(bucket), end(bucket)));
  ASSERT_EQUAL(2, bucket.front().value);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestSmoke);
  RUN_TEST(tr, TestEmpty);
  RUN_TEST(tr, TestIdempotency);
  RUN_TEST(tr, TestEquivalence);

  // HashSet<int, IntHasher> hash_set(100);
  // unordered_set<int> un_set;
  // LOG_DURATION("Add HashSet")
  // {
  //   for (int i = 0; i < 50000; i++) {
  //     hash_set.Add(i);
  //   }
  //   for (int i = 0; i < 50000; i+=123) {
  //     hash_set.Add(i);
  //   }
  // }
  // LOG_DURATION("Add un_set")
  // {
  //   for (int i = 0; i < 50000; i++) {
  //     un_set.insert(i);
  //   }
  //   for (int i = 0; i < 50000; i+=123) {
  //     un_set.insert(i);
  //   }
  // }
  return 0;
}