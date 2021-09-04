#include "test_runner.h"

#include <iostream>
#include <map>
#include <string>
#include <list>
#include <unordered_map>
#include <unordered_set>

using namespace std;

struct Record {
  string id;
  string title;
  string user;
  int timestamp;
  int karma;
};

struct RecordHasher {
  size_t operator() (const Record& r) const {
    hash<string> str_hash;
    hash<int> int_hash;
    int coef = 5003081;
    return
      coef * coef * coef * coef * str_hash(r.id) +
             coef * coef * coef * str_hash(r.title) +
                    coef * coef * str_hash(r.user) +
                           coef * int_hash(r.timestamp) +
                                  int_hash(r.karma);
  }
};

bool operator== (const Record& l, const Record& r) {
  return
    l.id == r.id &&
    l.title == r.title &&
    l.user == r.user &&
    l.timestamp == r.timestamp &&
    l.karma == r.karma;
}

bool operator< (const Record& l, const Record& r) {
  return
    tie(l.id, l.title, l.user, l.timestamp, l.karma) <
    tie(r.id, r.title, r.user, r.timestamp, r.karma);
}

// Реализуйте этот класс
class Database {
public:
  using Id = string;

  bool Put(const Record& record) {
    if (inDatabase(record.id)) {
      return false;
    }
    else {
      db[record.id] = {record};
      Entry& e = db.at(record.id);

      list<const Record*>& by_user_list = by_user[record.user];
      by_user_list.push_back(&e.rec);
      e.it_by_user = prev(by_user_list.end());

      list<const Record*>& by_timestamp_list = by_timestamp[record.timestamp];
      by_timestamp_list.push_back(&e.rec);
      e.it_by_timestamp = prev(by_timestamp_list.end());

      list<const Record*>& by_karma_list = by_karma[record.karma];
      by_karma_list.push_back(&e.rec);
      e.it_by_karma = prev(by_karma_list.end());

      return true;
    }

  }
  const Record* GetById(const string& id) const {
    if (inDatabase(id)) {
      return &db.at(id).rec;
    }
    else {
      return nullptr;
    }
  }

  bool Erase(const string& id) {
    if (inDatabase(id)) {
      const Entry& e = db.at(id);
      by_user[e.rec.user].erase(e.it_by_user);
      by_timestamp[e.rec.timestamp].erase(e.it_by_timestamp);
      by_karma[e.rec.karma].erase(e.it_by_karma);
      db.erase(id);
      return true;
    }
    else {
      return false;
    }
  }

  template <typename Callback>
  void RangeByTimestamp(int low, int high, Callback callback) const {
    auto begin = by_timestamp.lower_bound(low);
    auto end = by_timestamp.upper_bound(high);
    for (auto it = begin; it != end; ++it) {
      for (const Record* rec : it->second) {
        if(!callback(*rec)) return;
      }
    }
  }

  template <typename Callback>
  void RangeByKarma(int low, int high, Callback callback) const {
    auto begin = by_karma.lower_bound(low);
    auto end = by_karma.upper_bound(high);
    for (auto it = begin; it != end; ++it) {
      for (const Record* rec : it->second) {
        if (!callback(*rec)) return;
      }
    }
  }

  template <typename Callback>
  void AllByUser(const string& user, Callback callback) const {
    if (by_user.count(user)) {
      const list<const Record*>& by_user_list = by_user.at(user);
      for (const Record* rec : by_user_list) {
        if (!callback(*rec)) return;
      }
    }
  }
private:
  struct Entry {
    Record rec;
    list<const Record*>::iterator it_by_user;
    list<const Record*>::iterator it_by_timestamp;
    list<const Record*>::iterator it_by_karma;
  };

  unordered_map<Id, Entry> db;
  unordered_map<string, list<const Record*>> by_user;
  map<int, list<const Record*>> by_timestamp;
  map<int, list<const Record*>> by_karma;

  bool inDatabase(const Id& id) const {
    return db.count(id);
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
