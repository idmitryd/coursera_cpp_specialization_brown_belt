#include "test_runner.h"

#include <iostream>
#include <map>
#include <string>
#include <unordered_map>

using namespace std;

struct Record {
  string id;
  string title;
  string user;
  int timestamp;
  int karma;
};

// Реализуйте этот класс
class Database {
public:
  using Id = string;
  using User = string;
  using Timestamp = int;
  using Karma = int;

  bool Put(const Record& record) {
    auto [it, is_interesting] = database.insert({record.id, Data {record, {}, {}, {}}});
    if (!is_interesting) {
      return false;
    }
    Data& data = it->second;
    const Record *rec_ptr = &data.rec;
    data.it_karma = karma_storage.insert({rec_ptr->karma, rec_ptr});
    data.it_timestamp = timestamp_storage.insert({rec_ptr->timestamp, rec_ptr});
    data.it_user = user_storage.insert({rec_ptr->user, rec_ptr});
    return true;
  }
  const Record* GetById(const string& id) const {
    if (database.count(id)) {
      return &database.at(id).rec;
    }
    else {
      return nullptr;
    }
  }

  bool Erase(const string& id) {
    auto it = database.find(id);
    if (it != database.end()) {
      const Data& data = it->second;
      karma_storage.erase(data.it_karma);
      timestamp_storage.erase(data.it_timestamp);
      user_storage.erase(data.it_user);
      database.erase(it);
      return true;
    }
    else {
      return false;
    }
  }

  template <typename Callback>
  void RangeByTimestamp(int low, int high, Callback callback) const {
    ImplementCallback(timestamp_storage, callback, low, high);
  }

  template <typename Callback>
  void RangeByKarma(int low, int high, Callback callback) const {
    ImplementCallback(karma_storage, callback, low, high);
  }

  template <typename Callback>
  void AllByUser(const string& user, Callback callback) const {
    ImplementCallback(user_storage, callback, user, user);
  }

private:
  struct Data {
    Record rec;
    multimap<Karma, const Record*>::iterator it_karma;
    multimap<Timestamp, const Record*>::iterator it_timestamp;
    multimap<User, const Record*>::iterator it_user;
  };

  unordered_map<Id, Data> database;
  multimap<Karma, const Record*> karma_storage;
  multimap<Timestamp, const Record*> timestamp_storage;
  multimap<User, const Record*> user_storage;

  template <typename Key, typename Callback>
  void ImplementCallback (const multimap<Key, const Record*>& mmap, const Callback& callback, 
                          const Key& k_low, const Key& k_high) const {
    auto it_start = mmap.lower_bound(k_low);
    auto it_end = mmap.upper_bound(k_high);
    for (auto it = it_start; it != it_end; ++it) {
      if (!callback(*it->second)) return;
    }
  }
};

void TestRangeBoundaries() {
  const int good_karma = 1000;
  const int bad_karma = -10;

  Database db;
  db.Put({"id1", "Hello there", "master", 1536107260, good_karma});
  db.Put({"id2", "O>>-<", "general2", 1536107260, bad_karma});

  int count = 0;
  db.RangeByKarma(bad_karma, good_karma, [&count](const Record&) {
    ++count;
    return true;
  });

  ASSERT_EQUAL(2, count);
}

void TestSameUser() {
  Database db;
  db.Put({"id1", "Don't sell", "master", 1536107260, 1000});
  db.Put({"id2", "Rethink life", "master", 1536107260, 2000});

  int count = 0;
  db.AllByUser("master", [&count](const Record&) {
    ++count;
    return true;
  });

  ASSERT_EQUAL(2, count);
}

void TestReplacement() {
  const string final_body = "Feeling sad";

  Database db;
  db.Put({"id", "Have a hand", "not-master", 1536107260, 10});
  db.Erase("id");
  db.Put({"id", final_body, "not-master", 1536107260, -10});

  auto record = db.GetById("id");
  ASSERT(record != nullptr);
  ASSERT_EQUAL(final_body, record->title);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestRangeBoundaries);
  RUN_TEST(tr, TestSameUser);
  RUN_TEST(tr, TestReplacement);
  return 0;
}