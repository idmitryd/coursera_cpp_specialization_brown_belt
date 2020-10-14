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

  bool operator==(const Record& rec) const {
    return id == rec.id &&
           title == rec.title &&
           user == rec.user &&
           timestamp == rec.timestamp &&
           karma == rec.karma;
  }
};

// Реализуйте этот класс
class Database {
public:
  using Id = string;
  using Karma = int;
  using Timestamp = int;
  using User = string;

  bool Put(const Record& record) {
    if (!database.count(record.id)) {
      RecordHolder rh = {record,
                         karma_storage.insert({record.karma, record.id}),
                         timestamp_storage.insert({record.timestamp, record.id}),
                         user_storage.insert({record.user, record.id})};
      database[record.id] = rh;
      return true;
    } 
    else {
      return false;
    }
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
      const RecordHolder& rh = it->second;
      karma_storage.erase(rh.it_karma);
      timestamp_storage.erase(rh.it_timestamp);
      user_storage.erase(rh.it_user);
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
  struct RecordHolder {
    Record rec;
    multimap<Karma, Id>::iterator it_karma;
    multimap<Timestamp, Id>::iterator it_timestamp;
    multimap<User, Id>::iterator it_user;

    bool operator==(const RecordHolder& rh) const {
      return rec == rh.rec;
    }
  };

  unordered_map<Id, RecordHolder> database;
  multimap<Karma, Id> karma_storage;
  multimap<Timestamp, Id> timestamp_storage;
  multimap<User, Id> user_storage;

  template <typename Key, typename Callback>
  void ImplementCallback (const multimap<Key, Id>& mmap, Callback callback, 
                          const Key& k_low, const Key& k_high) const {
    auto it_start = mmap.lower_bound(k_low);
    auto it_end = mmap.upper_bound(k_high);
    for (auto it = it_start; it != it_end; ++it) {
      if (!callback(database.at(it->second).rec)) return;
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