#include <algorithm>
#include <iostream>
#include <vector>
#include <set>
#include <tuple>
#include <optional>
#include <string>
#include <numeric>

using namespace std;

template <typename Iterator>
class IteratorRange {
public:
  IteratorRange(Iterator begin, Iterator end)
    : first(begin)
    , last(end)
  {
  }

  Iterator begin() const {
    return first;
  }

  Iterator end() const {
    return last;
  }

private:
  Iterator first, last;
};

template <typename Collection>
auto Head(Collection& v, size_t top) {
  return IteratorRange{v.begin(), next(v.begin(), min(top, v.size()))};
}

struct Person {
  string name;
  int age, income;
  bool is_male;
};

ostream& operator<< (ostream& out, const Person& p) {
  string gender = p.is_male ? "M" : "W";
  out << "Name: " << p.name << ", age: " << p.age << ", income: " << p.income <<
       ", gender: " << gender;
  return out;
}

vector<Person> ReadPeople(istream& input) {
  int count;
  input >> count;

  vector<Person> result(count);
  for (Person& p : result) {
    char gender;
    input >> p.name >> p.age >> p.income >> gender;
    p.is_male = gender == 'M';
  }

  return result;
}

class DataBase {
private:
  vector<Person> byAge;
  vector<int> wealth;
  optional<string> most_popular_male_name;
  optional<string> most_popular_female_name;

public:
  explicit DataBase (vector<Person>& persons) {
    byAge = persons;
    sort(byAge.begin(), byAge.end(), 
      [] (const Person& lhs, const Person& rhs) {
        return lhs.age < rhs.age;
      }
    );
    vector<Person> byWealth = persons;
    sort(byWealth.begin(), byWealth.end(),
      [] (const Person& lhs, const Person& rhs) {
        return lhs.income > rhs.income;
      }
    );
    wealth.resize(byWealth.size());
    wealth[0] = byWealth[0].income;
    for (size_t i = 1; i < wealth.size(); ++i) {
      wealth[i] = wealth[i-1] + byWealth[i].income;
    }
    IteratorRange males = {persons.begin(),
      partition(persons.begin(), persons.end(),
        [](const Person& p) {
          return p.is_male;
        })};
    IteratorRange females = {males.end(), persons.end()};

    most_popular_male_name = MostPopularName(males);
    most_popular_female_name = MostPopularName(females);
  }

  int GetAge (int adult_age) const {
    auto it = lower_bound(byAge.begin(), byAge.end(), adult_age,
      [](const Person& lhs, int age) {
        return lhs.age < age;
      });

    return distance(it, byAge.end());
  }

  optional<string> GetMostPopularName(char gender) const {
    return gender == 'M' ? most_popular_male_name : most_popular_female_name;
  }

  int GetWealthy (size_t n) const {
    return n < wealth.size() ? wealth[n-1] : wealth.back();
  }

private:
  template <typename It>
  optional<string> MostPopularName(IteratorRange<It> range) const {
    if (range.begin() == range.end()) {
      return nullopt;
    }
    sort(range.begin(), range.end(), [](const Person& rhs, const Person& lhs) {
      return lhs.name > rhs.name;
    });
    const string* most_popular_name = &range.begin()->name;
    int count = 1;
    for (auto i = range.begin(); i != range.end(); ) {
      auto same_name_end = find_if_not(i, range.end(), [i](const Person& p) {
        return p.name == i->name;
      });
      const auto cur_name_count = distance(i, same_name_end);
      if (
        cur_name_count > count ||
        (cur_name_count == count && i->name < *most_popular_name)
      ) {
        count = cur_name_count;
        most_popular_name = &i->name;
      }
      i = same_name_end;
    }
    return *most_popular_name;
  }
};

  void TestSort() {
    vector<Person> p {
      {"John", 28, 1200, true},
      {"Liza", 22, 1300, false},
      {"Lizar", 22, 1300, true},
      {"David", 40, 10000, true}
    };
    DataBase db(p);
  }

int main() {
  // TestSort();
  vector<Person> people = ReadPeople(cin);
  const DataBase db(people);
  for (string command; cin >> command; ) {
    if (command == "AGE") {
      int adult_age;
      cin >> adult_age;
      cout << "There are " << db.GetAge(adult_age)
           << " adult people for maturity age " << adult_age << '\n';
    } else if (command == "WEALTHY") {
      int count;
      cin >> count;
      cout << "Top-" << count << " people have total income " << db.GetWealthy(count) << '\n';
    } else if (command == "POPULAR_NAME") {
      char gender;
      cin >> gender;
      optional<string> popular_name = db.GetMostPopularName(gender);
      if (!popular_name) {
        cout << "No people of gender " << gender << '\n';
      } else {
        cout << "Most popular name among people of gender " << gender << " is "
             << *popular_name << '\n';
      }
    }
  }
}
