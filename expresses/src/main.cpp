#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <set>
#include <string>
#include "test_runner.h"

using namespace std;

class RouteManager {
public:
  void AddRoute(int start, int finish) {
    reachable_lists_[start].insert(finish);
    reachable_lists_[finish].insert(start);
  }
  int FindNearestFinish(int start, int finish) const {
    int res = abs(finish - start);
    if (reachable_lists_.count(start) == 0) {
      return res;
    }
    const set<int>& stops = reachable_lists_.at(start);
    auto it_after_or_equal = stops.lower_bound(finish);
    if (it_after_or_equal != stops.end() && finish == (*it_after_or_equal)) {
      return 0;
    }
    res = it_after_or_equal == stops.end()
              ? res
              : min(res, abs(finish - *it_after_or_equal));
    res = it_after_or_equal == stops.begin()
              ? res
              : min(res, abs(finish - *prev(it_after_or_equal)));
    return res;
  }
private:
  unordered_map<int, set<int>> reachable_lists_;
};

// int main() {
//   RouteManager routes;
//   istringstream ss(
//       "7\n"
//       "ADD -2 5\n"
//       "ADD 10 4\n"
//       "ADD 5 8\n"
//       "GO 4 10\n"
//       "GO 4 -2\n"
//       "GO 5 0\n"
//       "GO 5 100\n");
//   ostringstream out;

//   int query_count;
//   ss >> query_count;

//   for (int query_id = 0; query_id < query_count; ++query_id) {
//     string query_type;
//     ss >> query_type;
//     int start, finish;
//     ss >> start >> finish;
//     if (query_type == "ADD") {
//       routes.AddRoute(start, finish);
//     } else if (query_type == "GO") {
//       out << routes.FindNearestFinish(start, finish) << "\n";
//     }
//   }

//   ASSERT_EQUAL(out.str(), "0\n6\n2\n92\n");
//   cout << "Test passed!" << endl;
//   return 0;
// }

int main() {
  RouteManager routes;

  int query_count;
  cin >> query_count;

  for (int query_id = 0; query_id < query_count; ++query_id) {
    string query_type;
    cin >> query_type;
    int start, finish;
    cin >> start >> finish;
    if (query_type == "ADD") {
      routes.AddRoute(start, finish);
    } else if (query_type == "GO") {
      cout << routes.FindNearestFinish(start, finish) << "\n";
    }
  }

  return 0;
}
