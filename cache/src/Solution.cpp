#include <algorithm>
#include <exception>
#include <iostream>
#include <list>
#include <mutex>
#include <unordered_map>

#include "Common.h"

using namespace std;

class LruCache : public ICache {
public:
  LruCache(shared_ptr<IBooksUnpacker> books_unpacker, const Settings& settings)
      : books_unpacker_(books_unpacker), settings_(settings) {}

  BookPtr GetBook(const string& book_name) override {
    lock_guard<mutex> lg(m);
    if (cache_.count(book_name)) {
      UpdateBookOrder(book_name);
      return cache_[book_name];
    }

    unique_ptr<IBook> book = books_unpacker_->UnpackBook(book_name);
    size_t book_size = book->GetContent().size();
    if (book_size <= settings_.max_memory) {
      ReleaseSpaceForBook(book_size);
      return AddBook(move(book));
    }
    else {
      order_.clear();
      cache_.clear();
      occupied_memory_ = 0;
      return move(book);
    }
  }

private:
  shared_ptr<IBooksUnpacker> books_unpacker_;
  const Settings settings_;
  unordered_map<string, BookPtr> cache_;
  list<string> order_;
  size_t occupied_memory_ = 0;
  mutable mutex m;

  void ReleaseSpaceForBook(size_t book_size) {
    while (!cache_.empty() && settings_.max_memory - occupied_memory_ <= book_size) {
      string cur_book = *order_.begin();
      occupied_memory_ -= cache_[cur_book]->GetContent().size();
      order_.pop_front();
      cache_.erase(cur_book);
    }
  }

  void UpdateBookOrder(const string& book_name) {
    const auto it = find(order_.begin(), order_.end(), book_name);
    if (it == order_.end())
      throw runtime_error("Book name exists in cache but has no rank");
    order_.push_back(*it);
    order_.erase(it);
  }

  BookPtr AddBook(unique_ptr<IBook> book) {
    string book_name = book->GetName();
    size_t book_size = book->GetContent().size();
    order_.push_back(book_name);
    cache_[book_name] = move(book);
    occupied_memory_ += book_size;
    return cache_[book_name];
  }
};

unique_ptr<ICache> MakeCache(shared_ptr<IBooksUnpacker> books_unpacker,
                             const ICache::Settings& settings) {
  return std::make_unique<LruCache>(books_unpacker, settings);
}
