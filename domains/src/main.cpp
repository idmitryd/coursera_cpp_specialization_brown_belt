#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "test_runner.h"

using namespace std;

class ForbiddenDomains {
public:
  ForbiddenDomains(istream& in_stream) : forbidden_domains(ReadDomains(in_stream)) {
  }
private:
  vector<vector<string>> forbidden_domains;

  vector<vector<string>> ReadDomains(istream& in_stream);
};

vector<string> SplitBy(string_view sv, char by = '.') {
  if (sv.empty())
    return {};
  vector<string> res;
  size_t pos = 0;
  while (true) {
    size_t dot_pos = sv.find(by, pos);
    res.push_back(string(sv.substr(pos, dot_pos - pos)));
    if (dot_pos == string_view::npos)
      break;
    pos = dot_pos + 1;
  }
  return res;
}

vector<vector<string>> ReadDomains(istream& in_stream = cin) {
  size_t query_count;
  in_stream >> query_count;
  vector<vector<string>> domains(query_count);
  for (size_t i = 0; i < query_count; ++i) {
    string domain;
    in_stream >> domain;
    domains[i] = SplitBy(domain);
    reverse(domains[i].begin(), domains[i].end());
  }
  // reverse(domains.begin(), domains.end());
  return domains;
}

bool IsPrefix(const vector<string>& lhs, const vector<string>& rhs) {
  if (rhs.size() > lhs.size())
    return false;
  for (size_t i = 0; i < lhs.size(); ++i) {
    for (size_t j = 0; j < rhs.size(); ++j) {
      if (lhs[i+j] != rhs[j])
        return false;
    }
    return true;
  }
  return false;
}

bool IsDomainForbidden(const vector<string>& domain, const vector<vector<string>>& forbidden_domains) {
  auto it = upper_bound(forbidden_domains.begin(), forbidden_domains.end(), domain);
  while (it != forbidden_domains.begin() && (*prev(it))[0] == domain[0]) {
    it = prev(it);
    if (IsPrefix(domain, *it))
      return true;
  }
  return false;
  // if (auto it = upper_bound(forbidden_domains.begin(), forbidden_domains.end(), domain);
  //     it != forbidden_domains.begin() && !IsPrefix(domain, *prev(it))) {
  //   cout << domain << endl;
  //   cout << *prev(it) << endl;
  //   cout << IsPrefix(domain, *prev(it)) << endl;
  //   return true;
  // }
  // return false;
  // for (const auto& forbidden : forbidden_domains) {
  //   if (forbidden.size() > domain.size())
  //     return false;
  //   if (IsPrefix(domain, forbidden))
  //     return true;
  // }
  // return false;
}

void CheckDomains(const vector<vector<string>>& forbidden_domains,
                  const vector<vector<string>>& domains,
                  ostream& out = cout) {
  for (const auto& domain : domains) {
    bool isBad = IsDomainForbidden(domain, forbidden_domains);
    out << (isBad ? "Bad" : "Good") << '\n';
  }
}

void TestSplitByDotEmpty() {
  string s_domain = "";
  vector<string> v_domain = SplitBy(s_domain);
  ASSERT_EQUAL(v_domain.size(), 0u);
}

void TestSplitByDotSingle() {
  string s_domain = "google";
  vector<string> v_domain = SplitBy(s_domain);
  vector<string> res = {"google"};
  ASSERT_EQUAL(v_domain, res);
}

void TestSplitByDotMany() {
  string s_domain = "m.ya.com";
  vector<string> v_domain = SplitBy(s_domain);
  vector<string> res = {"m", "ya", "com"};
  ASSERT_EQUAL(v_domain, res);
}

void TestReadDomains() {
  istringstream input(
    "4\n"
    "m.ya.com\n"
    "ru\n"
    "very.big.domain.with.many.words\n"
    "simle.domain\n"
  );
  vector<vector<string>> v_domains = ReadDomains(input);
  vector<vector<string>> res = {
    { "m", "ya", "com" },
    { "ru" },
    { "very", "big", "domain", "with", "many", "words" },
    { "simle", "domain" }
  };
  for (auto& d : res)
    reverse(d.begin(), d.end());
  ASSERT_EQUAL(v_domains, res);
}

void TestIsPrefix() {
  ASSERT_EQUAL(IsPrefix({"ru", "ya"}, {"ru"}), true);
  ASSERT_EQUAL(IsPrefix({"ru", "ya"}, {"ya"}), false);
  ASSERT_EQUAL(IsPrefix({"ya", "ru"}, {"com"}), false);
  ASSERT_EQUAL(IsPrefix({"big", "words", "ya", "ru"}, {"com"}), false);
  ASSERT_EQUAL(IsPrefix({"big", "words", "ya", "ru"}, {"ru"}), false);
  ASSERT_EQUAL(IsPrefix({"big", "words", "ya", "ru"}, {"ru", "ya"}), false);
  ASSERT_EQUAL(IsPrefix({"big", "words", "ya", "ru"}, {"words", "ya"}), false);
  ASSERT_EQUAL(IsPrefix({"big", "words", "ya", "ru"}, {"big", "words", "ya"}), true);
  // ASSERT_EQUAL(IsPrefix({"big", "words", "ya", "ru"}, {"ya", "ru"}), true);
  // ASSERT_EQUAL(IsPrefix({"big", "words", "ya", "ru"}, {"ru"}), true);
  // ASSERT_EQUAL(IsPrefix({"big", "words", "ya", "ru"}, {"ig"}), false);
  // ASSERT_EQUAL(IsPrefix({"defabc", "ru"}, {"abc"}), false);
  // ASSERT_EQUAL(IsPrefix({"defabc", "ru"}, {"abc", "ru"}), false);
  // ASSERT_EQUAL(IsPrefix({"b", "com"}, {"a", "b", "com"}), false);

  // ASSERT_EQUAL(IsPrefix({"d", "b", "c"}, {"a", "b", "c"}), false);
  // ASSERT_EQUAL(IsPrefix({"d", "b", "c"}, {"b", "c"}), true);

  // ASSERT_EQUAL(IsPrefix({"ru", "com"}, {"ru"}), false);
}

void TestCheckDomains() {
  vector<vector<string>> forbidden_domains = {
    {"b", "c"},
    {"a", "b", "c"},
    // {},
  };
  for (auto& d : forbidden_domains)
    reverse(d.begin(), d.end());
  sort(forbidden_domains.begin(), forbidden_domains.end());
  vector<vector<string>> domains = {
    {"d", "b", "c"}
  };
  for (auto& d : domains)
    reverse(d.begin(), d.end());
  ostringstream out;
  CheckDomains(forbidden_domains, domains, out);
  ASSERT_EQUAL(out.str(), "Bad\n");
}

void RunTests() {
  TestRunner tr;
  RUN_TEST(tr, TestSplitByDotEmpty);
  RUN_TEST(tr, TestSplitByDotSingle);
  RUN_TEST(tr, TestSplitByDotMany);
  RUN_TEST(tr, TestReadDomains);
  RUN_TEST(tr, TestIsPrefix);
  RUN_TEST(tr, TestCheckDomains);
}

int main() {
  // RunTests();
  // istringstream input(
  //     "4\n"
  //     "ya.ru\n"
  //     "maps.me\n"
  //     "m.ya.ru\n"
  //     "com\n"
  //     "9\n"
  //     "ya.ru\n"
  //     "ya.com\n"
  //     "a.com\n"
  //     "z.com\n"
  //     "m.maps.me\n"
  //     "moscow.m.ya.ru\n"
  //     "maps.com\n"
  //     "maps.ru\n"
  //     "ya.ya\n"
  //     );
  istream& input = cin;
  const vector<vector<string>> forbidden_domains = [&](){
    auto forbidden_domains = ReadDomains(input);
    set<vector<string>> s(forbidden_domains.begin(), forbidden_domains.end());
    // sort(forbidden_domains.begin(), forbidden_domains.end());
    return vector<vector<string>>(s.begin(), s.end());
  }();
  vector<vector<string>> domains_to_test = ReadDomains(input);
  CheckDomains(forbidden_domains, domains_to_test);
  return 0;
}
