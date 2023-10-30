#pragma once

#include <algorithm>
#include <vector>

using namespace std;

template <typename It>
class IteratorRange {
public:
    IteratorRange(It first, It last) : first(first), last(last) {
    }

    It begin() const {
        return first;
    }

    It end() const {
        return last;
    }

    size_t size() const {
        return last - first;
    }

private:
    It first, last;
};

template <typename Iterator>
class Paginator {
public:
    Paginator(Iterator begin, Iterator end, size_t page_size)
    {
        if (page_size == 0) {
            pages_.push_back(IteratorRange(begin, end));
        } else {
            for (Iterator it = begin; it < end; it += page_size) {
                if (it + page_size < end) {
                    pages_.push_back(IteratorRange(it, it + page_size));
                } else {
                    pages_.push_back(IteratorRange(it, end));
                }
            }
        }
    }

    size_t size() const
    {
        return pages_.size();
    }

    auto begin() const
    {
        return pages_.begin();
    }

    auto end() const
    {
        return pages_.end();
    }

private:
    vector<IteratorRange<Iterator>> pages_;
};

template <typename C>
auto Paginate(C& c, size_t page_size) {
    return Paginator(begin(c), end(c), page_size);
}

template <typename Container>
auto Head(Container& c, int top) {
//    return IteratorRange(begin(c), begin(c) + min<size_t>(max(top, 0), c.size()));
    return IteratorRange(begin(c), next(begin(c), min<size_t>(max(top, 0), c.size())));
}
