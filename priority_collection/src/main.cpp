#include "test_runner.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <set>
#include <utility>
#include <vector>
#include <list>
#include <map>

using namespace std;

template <typename T>
class PriorityCollection {
public:
  using Id = int/* тип, используемый для идентификаторов */;

  // Добавить объект с нулевым приоритетом
  // с помощью перемещения и вернуть его идентификатор
  Id Add(T object) {
      entries.push_back({0, move(object)});
      Id id = entries.size() - 1;
      sorted_values.insert({0, id});
      return id;
}

  // Добавить все элементы диапазона [range_begin, range_end)
  // с помощью перемещения, записав выданные им идентификаторы
  // в диапазон [ids_begin, ...)
  template <typename ObjInputIt, typename IdOutputIt>
  void Add(ObjInputIt range_begin, ObjInputIt range_end,
           IdOutputIt ids_begin) {
               vector<Id> ids;
               for (auto it = range_begin; it != range_end; it = next(it)) {
                   *ids_begin++ = Add(move(*it));
            }
        }

  // Определить, принадлежит ли идентификатор какому-либо
  // хранящемуся в контейнере объекту
  bool IsValid(Id id) const {
      return (id >= 0 && id < entries.size() && entries[id].priority != -1);
}

  // Получить объект по идентификатору
  const T& Get(Id id) const {
      return entries[id].value;
}

  // Увеличить приоритет объекта на 1
  void Promote(Id id) {
      int current_priority = entries[id].priority;
      entries[id].priority++;
      sorted_values.erase(sorted_values.find({current_priority, id}));
      sorted_values.insert({++current_priority, id});
}

  // Получить объект с максимальным приоритетом и его приоритет
  pair<const T&, int> GetMax() const {
      Id max_id = prev(sorted_values.end())->second;
      return {entries[max_id].value, entries[max_id].priority};
}

  // Аналогично GetMax, но удаляет элемент из контейнера
  pair<T, int> PopMax() {
      const auto max_it = prev(sorted_values.end());
      int priority = max_it->first;
      Id id = max_it->second;
      sorted_values.erase(max_it);
      entries[id].priority = -1;
      return {move(entries[id].value), priority};
      
}

private:
    struct Entry{
      int priority = 0;
      T value;
    };
    vector<Entry> entries;
    set<pair<int, Id>> sorted_values;
};






class StringNonCopyable : public string {
public:
  using std::string::string;  // Позволяет использовать конструкторы строки
  StringNonCopyable(const StringNonCopyable&) = delete;
  StringNonCopyable(StringNonCopyable&&) = default;
  StringNonCopyable& operator=(const StringNonCopyable&) = delete;
  StringNonCopyable& operator=(StringNonCopyable&&) = default;
};

void TestNoCopy() {
  PriorityCollection<StringNonCopyable> strings;
  const auto white_id = strings.Add("white");
  const auto yellow_id = strings.Add("yellow");
  const auto red_id = strings.Add("red");

  strings.Promote(yellow_id);
  for (int i = 0; i < 2; ++i) {
    strings.Promote(red_id);
  }
  strings.Promote(yellow_id);
  {
    const auto item = strings.PopMax();
    ASSERT_EQUAL(item.first, "red");
    ASSERT_EQUAL(item.second, 2);
  }
  {
    const auto item = strings.PopMax();
    ASSERT_EQUAL(item.first, "yellow");
    ASSERT_EQUAL(item.second, 2);
  }
  {
    const auto item = strings.PopMax();
    ASSERT_EQUAL(item.first, "white");
    ASSERT_EQUAL(item.second, 0);
  }
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestNoCopy);
  return 0;
}
