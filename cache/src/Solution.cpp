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
      const auto it = cache_[book_name];
      order_.splice(order_.end(), order_, it);
      return order_.back();
    }

    unique_ptr<IBook> book = books_unpacker_->UnpackBook(book_name);
    size_t book_size = book->GetContent().size();
    if (book_size > settings_.max_memory) {
      order_.clear();
      cache_.clear();
      occupied_memory_ = 0;
      return move(book);
    }
    else {
      ReleaseSpaceForBook(book_size);
      return AddBook(move(book));
    }
  }

private:
  shared_ptr<IBooksUnpacker> books_unpacker_;
  const Settings settings_;
  list<BookPtr> order_;
  unordered_map<string, list<BookPtr>::iterator> cache_;
  size_t occupied_memory_ = 0;
  mutable mutex m;

  void ReleaseSpaceForBook(size_t demanded_size) {
    while (settings_.max_memory - occupied_memory_ < demanded_size) {
      occupied_memory_ -= order_.front()->GetContent().size();
      cache_.erase(order_.front()->GetName());
      order_.pop_front();
    }
  }

  BookPtr AddBook(unique_ptr<IBook> book) {
    occupied_memory_ += book->GetContent().size();
    string book_name = book->GetName();
    cache_[move(book_name)] = order_.insert(order_.end(), move(book));
    return order_.back();
  }
};

unique_ptr<ICache> MakeCache(shared_ptr<IBooksUnpacker> books_unpacker,
                             const ICache::Settings& settings) {
  return std::make_unique<LruCache>(books_unpacker, settings);
}
