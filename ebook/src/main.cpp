#include <iomanip>
#include <iostream>
#include <utility>
#include <vector>
#include <numeric>

#include "test_runner.h"

using namespace std;

class ReadingManager {
 public:
  ReadingManager()
      : users_on_page_(MAX_PAGE_COUNT_ + 1, 0),
        user_page_(MAX_USER_COUNT_ + 1, 0) {}

  void Read(int user_id, int page_count) {
    if (user_page_[user_id] != 0) {
      users_on_page_[user_page_[user_id]]--;
    }
    else {
      user_count++;
    }
    user_page_[user_id] = page_count;
    users_on_page_[page_count]++;
  }

  double Cheer(int user_id) const {
    int page = user_page_[user_id];
    if (page == 0)
      return 0;
    if (user_count == 1)
      return 1;
    return 1.0 * accumulate(users_on_page_.begin(), users_on_page_.begin() + page, 0) / (user_count - 1);
  }

 private:
  static const int MAX_USER_COUNT_ = 100'000;
  static const int MAX_PAGE_COUNT_ = 1000;

  // Number of users that corrently on this page
  vector<int> users_on_page_;
  // Number of page on which current user is
  vector<int> user_page_;
  int user_count = 0;

};

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  ReadingManager manager;

  int query_count;
  cin >> query_count;

  for (int query_id = 0; query_id < query_count; ++query_id) {
    string query_type;
    cin >> query_type;
    int user_id;
    cin >> user_id;

    if (query_type == "READ") {
      int page_count;
      cin >> page_count;
      manager.Read(user_id, page_count);
    } else if (query_type == "CHEER") {
      cout << setprecision(6) << manager.Cheer(user_id) << "\n";
    }
  }

  return 0;
}

// int main() {
//   // Для ускорения чтения данных отключается синхронизация
//   // cin и cout с stdio,
//   // а также выполняется отвязка cin от cout
//   ios::sync_with_stdio(false);
//   cin.tie(nullptr);

//   ReadingManager manager;
//   istringstream ss(
//       "12\nCHEER 5\nREAD 1 10\nCHEER 1\nREAD 2 5\nREAD 3 7\nCHEER 2\nCHEER 3\n"
//       "READ 3 10\nCHEER 3\nREAD 3 11\nCHEER 3\nCHEER 1");
//   ostringstream out;

//   int query_count;
//   ss >> query_count;

//   for (int query_id = 0; query_id < query_count; ++query_id) {
//     string query_type;
//     ss >> query_type;
//     int user_id;
//     ss >> user_id;

//     if (query_type == "READ") {
//       int page_count;
//       ss >> page_count;
//       manager.Read(user_id, page_count);
//     } else if (query_type == "CHEER") {
//       out << setprecision(6) << manager.Cheer(user_id) << "\n";
//     }
//   }
//   string res = "0\n1\n0\n0.5\n0.5\n1\n0.5\n";
//   ASSERT_EQUAL(out.str(), res);
//   cout << "Test passed" << endl;;
//   return 0;
// }
