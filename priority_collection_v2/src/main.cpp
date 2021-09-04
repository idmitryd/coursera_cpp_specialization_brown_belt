#include <iostream>
#include <utility>
#include <map>
#include <unordered_map>
#include <set>

#include <test_runner.h>

using namespace std;

template <typename T>
class PriorityCollection {
public:
  using Id = int /* тип, используемый для идентификаторов */;

  // Добавить объект с нулевым приоритетом
  // с помощью перемещения и вернуть его идентификатор
  Id Add(T object) {
    data_holder.push_back({0, move(object)});
    const Id id = data_holder.size() - 1;
    sorted_values.insert({0,id});
    return id;
  }

  // Добавить все элементы диапазона [range_begin, range_end)
  // с помощью перемещения, записав выданные им идентификаторы
  // в диапазон [ids_begin, ...)
  template <typename ObjInputIt, typename IdOutputIt>
  void Add(ObjInputIt range_begin, ObjInputIt range_end,
           IdOutputIt ids_begin) {
    for (auto it = range_begin; it != range_end; ++it) {
      *ids_begin = Add(move(*it));
      ids_begin++;
    }
  }

  // Определить, принадлежит ли идентификатор какому-либо
  // хранящемуся в контейнере объекту
  bool IsValid(Id id) const {
    return id >= 0 && id < data_holder.size() && data_holder[id].priority != -1;
  }

  // Получить объект по идентификатору
  const T& Get(Id id) const {
    return data_holder[id].value;
  }

  // Увеличить приоритет объекта на 1
  void Promote(Id id) {
    sorted_values.erase({data_holder[id].priority, id});
    sorted_values.insert({++data_holder[id].priority, id});
  }

  // Получить объект с максимальным приоритетом и его приоритет
  pair<const T&, int> GetMax() const {
    Id id = sorted_values.rbegin()->second;
    return {data_holder[id].value, data_holder[id].priority};
  }

  // Аналогично GetMax, но удаляет элемент из контейнера
  pair<T, int> PopMax() {
    pair<int, Id> val = sorted_values.extract(*sorted_values.rbegin()).value();
    data_holder[val.second].priority = -1;
    return {move(data_holder[val.second].value), val.first};
  }

private:
  struct Entry {
    int priority = 0;
    T value;
  };
  vector<Entry> data_holder;
  set<pair<int, Id>> sorted_values;
};

class StringNonCopyable : public string {
public:
  using string::string;  // Позволяет использовать конструкторы строки
  StringNonCopyable(const StringNonCopyable&) = delete;
  StringNonCopyable(StringNonCopyable&&) = default;
  StringNonCopyable& operator=(const StringNonCopyable&) = delete;
  StringNonCopyable& operator=(StringNonCopyable&&) = default;
};

void TestNoCopy() {
  PriorityCollection<StringNonCopyable> strings;
  strings.Add("white");
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
