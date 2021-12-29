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

pair<string, string> SplitBy(const string& what, const string& by) {
  size_t pos = what.find(by);
  if (by.size() < what.size() && pos < what.size() - by.size()) {
    return {what.substr(0, pos), what.substr(pos + by.size())};
  } else {
    return {what, {}};
  }
}

vector<string> SplitByDot(string_view sv) {
  if (sv.empty())
    return {};
  vector<string> res;
  size_t pos = 0;
  while (true) {
    size_t dot_pos = sv.find('.', pos);
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
    domains[i] = SplitByDot(domain);
  }
  return domains;
}

bool IsSubDomain(const vector<string>& lhs, const vector<string>& rhs) {
  if (rhs.size() > lhs.size())
    return false;
  for (int i = lhs.size() - 1; i >= 0; --i) {
    for (int j = rhs.size() - 1; j >= 0; --j) {
      if (lhs[i-(rhs.size() - 1 - j)] != rhs[j])
        return false;
    }
    return true;
  }
  return false;
}

bool IsDomainForbidden(const vector<string>& domain, const vector<vector<string>>& forbidden_domains) {
  for (const auto& forbidden : forbidden_domains) {
    if (forbidden.size() > domain.size())
      return false;
    if (IsSubDomain(domain, forbidden))
      return true;
  }
  return false;
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
  vector<string> v_domain = SplitByDot(s_domain);
  ASSERT_EQUAL(v_domain.size(), 0u);
}

void TestSplitByDotSingle() {
  string s_domain = "google";
  vector<string> v_domain = SplitByDot(s_domain);
  vector<string> res = {"google"};
  ASSERT_EQUAL(v_domain, res);
}

void TestSplitByDotMany() {
  string s_domain = "m.ya.com";
  vector<string> v_domain = SplitByDot(s_domain);
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
  ASSERT_EQUAL(v_domains, res);
}

void TestIsSubDomain() {
  ASSERT_EQUAL(IsSubDomain({"ya", "ru"}, {"ru"}), true);
  ASSERT_EQUAL(IsSubDomain({"ya", "ru"}, {"ya"}), false);
  ASSERT_EQUAL(IsSubDomain({"ya", "ru"}, {"com"}), false);
  ASSERT_EQUAL(IsSubDomain({"big", "words", "ya", "ru"}, {"com"}), false);
  ASSERT_EQUAL(IsSubDomain({"big", "words", "ya", "ru"}, {"big"}), false);
  ASSERT_EQUAL(IsSubDomain({"big", "words", "ya", "ru"}, {"big", "words"}), false);
  ASSERT_EQUAL(IsSubDomain({"big", "words", "ya", "ru"}, {"words", "ya"}), false);
  ASSERT_EQUAL(IsSubDomain({"big", "words", "ya", "ru"}, {"words", "ya", "ru"}), true);
  ASSERT_EQUAL(IsSubDomain({"big", "words", "ya", "ru"}, {"ya", "ru"}), true);
  ASSERT_EQUAL(IsSubDomain({"big", "words", "ya", "ru"}, {"ru"}), true);
  ASSERT_EQUAL(IsSubDomain({"big", "words", "ya", "ru"}, {"ig"}), false);
  ASSERT_EQUAL(IsSubDomain({"defabc", "ru"}, {"abc"}), false);
  ASSERT_EQUAL(IsSubDomain({"defabc", "ru"}, {"abc", "ru"}), false);
  ASSERT_EQUAL(IsSubDomain({"b", "com"}, {"a", "b", "com"}), false);

  ASSERT_EQUAL(IsSubDomain({"d", "b", "c"}, {"a", "b", "c"}), false);
  ASSERT_EQUAL(IsSubDomain({"d", "b", "c"}, {"b", "c"}), true);

  ASSERT_EQUAL(IsSubDomain({"ru", "com"}, {"ru"}), false);
}

void TestCheckDomains() {
  vector<vector<string>> forbidden_domains = {
    {"b", "c"},
    {"a", "b", "c"},
    // {},
  };
  vector<vector<string>> domains = {
    {"d", "b", "c"}
  };
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
  RUN_TEST(tr, TestIsSubDomain);
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
  //     "7\n"
  //     "ya.ru\n"
  //     "ya.com\n"
  //     "m.maps.me\n"
  //     "moscow.m.ya.ru\n"
  //     "maps.com\n"
  //     "maps.ru\n"
  //     "ya.ya\n"
  //     );
  istream& input = cin;
  // const vector<vector<string>> sorted_forbidden_domains = [&](){
  //   auto forbidden_domains = ReadDomains(input);
  //   // set<vector<string>> fd(forbidden_domains.begin(), forbidden_domains.end());
  //   // return vector<vector<string>>(fd.begin(), fd.end());
  //   sort(forbidden_domains.begin(), forbidden_domains.end(),
  //        [](const auto& lhs, const auto& rhs) {
  //          return lhs.size() < rhs.size();
  //        });
  //   return forbidden_domains;
  // }();
  vector<vector<string>> forbidden_domains = ReadDomains(input);
  vector<vector<string>> domains_to_test = ReadDomains(input);
  CheckDomains(forbidden_domains, domains_to_test);
  return 0;
}
