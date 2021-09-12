#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <numeric>
#include <unordered_map>

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

class DataBase {
public:
  explicit DataBase(istream& input)
    : people(ReadPeople(input)),
      sorted_ages(SortAges()),
      cumulative_income(CalculateCumulativeIncome()),
      m_popular_name(CalculatePopularName('M')),
      w_popular_name(CalculatePopularName('W'))
  {
  }

  ostream& GetPopularName(ostream& out, char gender) const {
    string name = gender == 'M' ? m_popular_name : w_popular_name;
    if (name.empty()) {
      return out << "No people of gender " << gender << '\n';
    }
    else {
      return out << "Most popular name among people of gender " << gender << " is " << name << '\n';
    }
  }

  ostream& GetWealthy(ostream& out, size_t n) const {
    int income = n <= cumulative_income.size() ? cumulative_income.at(n-1)
                                              : cumulative_income.back();
    return out << "Top-" << n << " people have total income " << income << '\n';
  }

  ostream& GetAge(ostream& out, size_t n) const {
    auto adult_begin = lower_bound(sorted_ages.begin(), sorted_ages.end(), n);
    size_t adults = distance(adult_begin, sorted_ages.end());
    return out << "There are " << adults << " adult people for maturity age " << n << '\n';
  }

private:
  const vector<Person> people;
  const vector<int> sorted_ages;
  const vector<int> cumulative_income;
  const string m_popular_name;
  const string w_popular_name;

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

  vector<int> SortAges() const {
    vector<int> res(people.size());
    for (size_t i = 0; i < res.size(); ++i) {
      res[i] = people[i].age;
    }
    sort(res.begin(), res.end());
    return res;
  }

  vector<int> CalculateCumulativeIncome() const {
    vector<int> res(people.size());
    for (size_t i = 0; i < res.size(); ++i) {
      res[i] = people[i].income;
    }
    sort(res.begin(), res.end(),
          [](int l, int r) {
            return l > r;
          }
        );
    for (size_t i = 1; i < res.size(); ++i) {
      res[i] += res[i-1];
    }
    return res;
  }

  string CalculatePopularName(char gender) const {
    unordered_map<string, size_t> name_counter;
    for (const Person& p : people) {
      if (p.is_male && gender == 'M') {
        name_counter[p.name]++;
      }
      else if (!p.is_male && gender=='W') {
        name_counter[p.name]++;
      }
    }
    vector<pair<string, size_t>> names(
        name_counter.begin(), name_counter.end() );
    if (names.empty()) {
      return "";
    }
    sort(names.begin(), names.end(),
        [](const auto& l, const auto& r) {
          if (l.second != r.second)
            return l.second > r.second;
          else {
            return l.first < r.first;
          }
        });
    return names[0].first;
  }

};

int main() {
  const DataBase db(cin);

  for (string command; cin >> command; ) {
    if (command == "AGE") {
      int adult_age;
      cin >> adult_age;
      db.GetAge(cout, adult_age);
    } else if (command == "WEALTHY") {
      int count;
      cin >> count;
      db.GetWealthy(cout, count);
    } else if (command == "POPULAR_NAME") {
      char gender;
      cin >> gender;
      db.GetPopularName(cout, gender);
    }
  }
}
