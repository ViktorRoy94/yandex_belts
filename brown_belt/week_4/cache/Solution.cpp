#include "Common.h"
#include <deque>
#include <algorithm>
#include <mutex>

using namespace std;

class LruCache : public ICache {
public:
    LruCache(shared_ptr<IBooksUnpacker> books_unpacker, const Settings& settings)
        : books_unpacker_(move(books_unpacker))
        , settings_(settings)
    {
    }

    BookPtr GetBook(const string& book_name) override {
        lock_guard g(m);
        auto it = find_if(sorted_books_.begin(),
                          sorted_books_.end(),
                          [book_name](BookPtr b){
                              return b->GetName() == book_name;
                          }
        );

        BookPtr book;
        if (it != sorted_books_.end()) {
            book = *it;
            books_in_cache_size_ -= book->GetContent().size();
            sorted_books_.erase(it);
        } else {
            book = books_unpacker_->UnpackBook(book_name);
        }
        books_in_cache_size_ += book->GetContent().size();
        sorted_books_.push_back(move(book));
        DeleteLowRankBooks();
        return sorted_books_.back();
    }

private:
    deque<BookPtr> sorted_books_;
    size_t books_in_cache_size_ = 0;
    shared_ptr<IBooksUnpacker> books_unpacker_;
    Settings settings_;

    mutex m;

    void DeleteLowRankBooks() {
        while (books_in_cache_size_ > settings_.max_memory && !sorted_books_.empty()) {
            books_in_cache_size_ -= sorted_books_.front()->GetContent().size();
            sorted_books_.pop_front();
        }
    }
};


unique_ptr<ICache> MakeCache(
        shared_ptr<IBooksUnpacker> books_unpacker,
        const ICache::Settings& settings) {
    return make_unique<LruCache>(move(books_unpacker), settings);
}
