#include <cstdint>
#include <iostream>
#include <map>
#include <unordered_map>
#include <string>
#include <queue>

using namespace std;

class HotelBooker {
private:
  struct Entry {
    int64_t time;
    string hotel_name;
    int client_id;
    int room_count;
  };
  queue<Entry> entries;
  unordered_map<string, int> occupied_rooms;
  unordered_map<string, unordered_map<int, int>> client_rooms;

public:
  void Book(int64_t time, const string& hotel_name, int client_id, int room_count) {
    entries.push({time, hotel_name, client_id, room_count});
    occupied_rooms[hotel_name] += room_count;
    client_rooms[hotel_name][client_id] += room_count;
    RemoveOldEntries(time);
  }

  int GetClientAmount(const string& hotel_name) const {
    if (client_rooms.count(hotel_name))
      return client_rooms.at(hotel_name).size();
    return 0;
  }

  int GetRoomAmount(const string& hotel_name) const {
    if (occupied_rooms.count(hotel_name))
      return occupied_rooms.at(hotel_name);
    return 0;
  }

private:
  void RemoveOldEntries(int64_t current_time) {
    while (!entries.empty() && entries.front().time <= current_time - 86400) {
      const string& hotel_name = entries.front().hotel_name;
      int client_id = entries.front().client_id;
      int room_count = entries.front().room_count;

      occupied_rooms[hotel_name] -= room_count;
      if (occupied_rooms[hotel_name] == 0)
        occupied_rooms.erase(hotel_name);

      client_rooms[hotel_name][client_id] -= room_count;
      if (client_rooms[hotel_name][client_id] == 0)
        client_rooms[hotel_name].erase(client_id);

      entries.pop();
    }
  }

};

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  HotelBooker hb;
  size_t q;
  cin >> q;
  for (size_t i = 0; i < q; ++i) {
    string event;
    cin >> event;
    if (event == "BOOK") {
      int64_t time;
      string hotel_name;
      int client_id, room_count;
      cin >> time >> hotel_name >> client_id >> room_count;
      hb.Book(time, move(hotel_name), client_id, room_count);
    }
    else if (event == "CLIENTS") {
      string hotel_name;
      cin >> hotel_name;
      cout << hb.GetClientAmount(move(hotel_name)) << '\n';
    }
    else if (event == "ROOMS") {
      string hotel_name;
      cin >> hotel_name;
      cout << hb.GetRoomAmount(move(hotel_name)) << '\n';
    }
    else {
      throw runtime_error("Unknown event was passed : " + event);
    }
  }
  return 0;
}
