#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <random>

#include "test_runner.h"

using namespace std;

enum class Gender {
  FEMALE,
  MALE
};

struct Person {
  int age;
  Gender gender;
  bool is_employed;
};

bool operator==(const Person& lhs, const Person& rhs) {
  return lhs.age == rhs.age
      && lhs.gender == rhs.gender
      && lhs.is_employed == rhs.is_employed;
}

ostream& operator<<(ostream& stream, const Person& person) {
  return stream << "Person(age=" << person.age
      << ", gender=" << static_cast<int>(person.gender)
      << ", is_employed=" << boolalpha << person.is_employed << ")";
}

struct AgeStats {
  int total;
  int females;
  int males;
  int employed_females;
  int unemployed_females;
  int employed_males;
  int unemployed_males;
};

template <typename InputIt>
int ComputeMedianAge(InputIt range_begin, InputIt range_end) {
  if (range_begin == range_end) {
    return 0;
  }
  vector<typename iterator_traits<InputIt>::value_type> range_copy(
      range_begin,
      range_end
  );
  auto middle = begin(range_copy) + range_copy.size() / 2;
  nth_element(
      begin(range_copy), middle, end(range_copy),
      [](const Person& lhs, const Person& rhs) {
        return lhs.age < rhs.age;
      }
  );
  return middle->age;
}

vector<Person> ReadPersons(istream& in_stream = cin) {
  int person_count;
  in_stream >> person_count;
  vector<Person> persons;
  persons.reserve(person_count);
  for (int i = 0; i < person_count; ++i) {
    int age, gender, is_employed;
    in_stream >> age >> gender >> is_employed;
    Person person{
        age,
        static_cast<Gender>(gender),
        is_employed == 1
    };
    persons.push_back(person);
  }
  return persons;
}

AgeStats ComputeStats(vector<Person> persons) {
  //                 persons
  //                |       |
  //          females        males
  //         |       |      |     |
  //      empl.  unempl. empl.   unempl.

  auto females_end = partition(
      begin(persons), end(persons),
      [](const Person& p) {
        return p.gender == Gender::FEMALE;
      }
  );
  auto employed_females_end = partition(
      begin(persons), females_end,
      [](const Person& p) {
        return p.is_employed;
      }
  );
  auto employed_males_end = partition(
      females_end, end(persons),
      [](const Person& p) {
        return p.is_employed;
      }
  );

  return {
       ComputeMedianAge(begin(persons), end(persons)),
       ComputeMedianAge(begin(persons), females_end),
       ComputeMedianAge(females_end, end(persons)),
       ComputeMedianAge(begin(persons),  employed_females_end),
       ComputeMedianAge(employed_females_end, females_end),
       ComputeMedianAge(females_end, employed_males_end),
       ComputeMedianAge(employed_males_end, end(persons))
  };
}

void PrintStats(const AgeStats& stats,
                ostream& out_stream = cout) {
  out_stream << "Median age = "
             << stats.total              << endl
             << "Median age for females = "
             << stats.females            << endl
             << "Median age for males = "
             << stats.males              << endl
             << "Median age for employed females = "
             << stats.employed_females   << endl
             << "Median age for unemployed females = "
             << stats.unemployed_females << endl
             << "Median age for employed males = "
             << stats.employed_males     << endl
             << "Median age for unemployed males = "
             << stats.unemployed_males   << endl;
}

void TestEqualOperator() {
  Person p1 = {30, Gender::MALE, true};
  Person p2 = {30, Gender::FEMALE, true};
  Person p3 = {30, Gender::MALE, false};
  Person p4 = {36, Gender::MALE, true};
  Person p5 = {25, Gender::FEMALE, true};

  ASSERT(p1==p1);
  ASSERT(!(p1==p2));
  ASSERT(!(p1==p3));
  ASSERT(!(p1==p4));
  ASSERT(!(p1==p5));
  ASSERT(!(p2==p5));
}

void TestInsertionOperator() {
  Person p1 = {30, Gender::MALE, true};
  Person p2 = {30, Gender::FEMALE, true};
  Person p3 = {30, Gender::MALE, false};
  Person p4 = {36, Gender::MALE, true};
  Person p5 = {25, Gender::FEMALE, false};
  ostringstream out;
  out << p1 << '\n' << p2 << '\n' << p3 << '\n' << p4 << '\n' << p5 << '\n';
  string res =
      "Person(age=30, gender=1, is_employed=true)\n"
      "Person(age=30, gender=0, is_employed=true)\n"
      "Person(age=30, gender=1, is_employed=false)\n"
      "Person(age=36, gender=1, is_employed=true)\n"
      "Person(age=25, gender=0, is_employed=false)\n";
  ASSERT(res == out.str());
}

void TestComputeMedianAge() {
  vector<Person> v;
  ASSERT_EQUAL(ComputeMedianAge(v.begin(), v.end()), 0);
  v = {
    {30, Gender::MALE, true},
    {27, Gender::FEMALE, true},
    {19, Gender::MALE, false},
    {64, Gender::MALE, true},
    {25, Gender::FEMALE, false},
  };
  ASSERT_EQUAL(ComputeMedianAge(v.begin() + 2, v.begin() + 2), 0);
  ASSERT_EQUAL(ComputeMedianAge(v.begin(), next(v.begin())), 30);
  ASSERT_EQUAL(ComputeMedianAge(v.begin(), v.end()), 27);
  v.push_back({80, Gender::FEMALE, true});
  ASSERT_EQUAL(ComputeMedianAge(v.begin(), v.end()), 30);
}

void TestReadPersons() {
  istringstream in(
      "5\n"
      "30 1 1\n"
      "27 0 1\n"
      "19 1 0\n"
      "64 1 1\n"
      "25 0 0\n"
      );
  vector<Person> res = {
    {30, Gender::MALE, true},
    {27, Gender::FEMALE, true},
    {19, Gender::MALE, false},
    {64, Gender::MALE, true},
    {25, Gender::FEMALE, false},
  };
  ASSERT_EQUAL(ReadPersons(in), res);
}

void TestComputeStats() {
  vector<Person> v = {
    {24, Gender::FEMALE, true},
    {30, Gender::FEMALE, true},
    {48, Gender::FEMALE, true},
    {55, Gender::FEMALE, true},

    {10, Gender::FEMALE, false},
    {12, Gender::FEMALE, false},
    {16, Gender::FEMALE, false},

    {33, Gender::MALE, true},
    {55, Gender::MALE, true},
    {58, Gender::MALE, true},

    {10, Gender::MALE, false},
    {13, Gender::MALE, false},
    {17, Gender::MALE, false},
    {22, Gender::MALE, false},
  };
  random_shuffle(v.begin(), v.end());
  AgeStats res = ComputeStats(v);
  ASSERT_EQUAL(res.total, 24);
  ASSERT_EQUAL(res.females, 24);
  ASSERT_EQUAL(res.males, 22);
  ASSERT_EQUAL(res.employed_females, 48);
  ASSERT_EQUAL(res.unemployed_females, 12);
  ASSERT_EQUAL(res.employed_males, 55);
  ASSERT_EQUAL(res.unemployed_males, 17);
}

void TestPrintStats() {
  AgeStats stats = {
      .total = 24,
      .females = 20,
      .males = 22,
      .employed_females = 48,
      .unemployed_females = 12,
      .employed_males = 55,
      .unemployed_males = 17,
  };
  ostringstream out;
  PrintStats(stats, out);
  string res =
    "Median age = 24\n"
    "Median age for females = 20\n"
    "Median age for males = 22\n"
    "Median age for employed females = 48\n"
    "Median age for unemployed females = 12\n"
    "Median age for employed males = 55\n"
    "Median age for unemployed males = 17\n";
  ASSERT_EQUAL(out.str(), res);
}

int main() {
  TestRunner tr;
  tr.RunTest(TestEqualOperator, "TestOperatorEqual");
  tr.RunTest(TestInsertionOperator, "TestInsertionOperator");
  tr.RunTest(TestComputeMedianAge, "TestComputeMedianAge");
  tr.RunTest(TestReadPersons, "TestReadPersons");
  tr.RunTest(TestComputeStats, "TestComputeStats");
  tr.RunTest(TestPrintStats, "TestPrintStats");
  return 0;
}
