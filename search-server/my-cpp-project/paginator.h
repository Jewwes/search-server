#pragma once

#include <iostream>
#include <vector>
#include <numeric>
template <typename Iterator>
class IteratorRange {
public:
    IteratorRange(Iterator begin, Iterator end) : begin_(begin), end_(end), size_(distance(begin, end)) {

    }

    Iterator begin() const {
        return begin_;
    }

    Iterator end() const {
        return end_;
    }


    size_t size() const {
        return size_;
    }

private:
    Iterator begin_;
    Iterator end_;
    size_t size_;
};

template <typename Iterator>
class Paginator {
public:
    Paginator(Iterator begin, Iterator end, size_t page_size) {
        while (begin != end) {
            const size_t current_page_size = std::min(page_size, static_cast<size_t>(distance(begin, end)));
            const Iterator current_page_end = std::next(begin, current_page_size);
            pages_.emplace_back(begin, current_page_end);
            begin = current_page_end;
        }
    }

    auto begin() const {
        return pages_.begin();
    }

    auto end() const {
        return pages_.end();
    }


private:
    std::vector<IteratorRange<Iterator>> pages_;
};

template <typename Container>
auto Paginate(const Container& c, size_t page_size) {
    return Paginator(begin(c), end(c), page_size);
}

template <typename Iterator>
std::ostream& operator<<(std::ostream& os, const IteratorRange<Iterator>& range) {
    for (Iterator it = range.begin(); it != range.end(); ++it) {
        os << *it;
    }
    return os;
}