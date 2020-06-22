#pragma once
#include <cassert>
#include <algorithm>

namespace mcts {

template <typename T, size_t MAX_SIZE>
class MaxSizeVector
{
  public:
    MaxSizeVector() = default;
    MaxSizeVector(const MaxSizeVector& other) : data_(other.data_), count_(other.count_) {}
    MaxSizeVector(const std::vector<T>& other) { insert(begin(), other.begin(), other.end()); }
    MaxSizeVector(MaxSizeVector&& other) noexcept : data_(std::move(other.data_)), count_(std::move(other.count_)) {}

    MaxSizeVector& operator=(const MaxSizeVector& other)
    {
        data_ = other.data_;
        count_ = other.count_;
        return *this;
    }

    MaxSizeVector& operator=(MaxSizeVector&& other) noexcept
    {
        data_ = std::move(other.data_);
        count_ = std::move(other.count_);
        return *this;
    }

    [[nodiscard]] T* begin() { return data_.begin(); }
    [[nodiscard]] T* end() { return begin() + count_; }
    [[nodiscard]] const T* begin() const { return data_.begin(); }
    [[nodiscard]] const T* end() const { return begin() + count_; }

    void reserve(size_t newSize)
    {
        assert(newSize < MAX_SIZE);
        (void)newSize;
    }

    void clear() { count_ = 0; }

    template <class InputIterator>
    T* insert(T* position, InputIterator first, InputIterator last)
    {
        auto dist = std::distance(first, last);
        assert(size() + dist <= capacity());

        // first move all the elements to the end...
        std::copy(position, end(), position + dist);

        // now copy the input range into the empty space
        std::copy(first, last, position);

        count_ += std::distance(first, last);
        return position;
    }

    [[nodiscard]] bool empty() const { return count_ == 0; }
    [[nodiscard]] size_t size() const { return count_; }
    [[nodiscard]] size_t capacity() const { return MAX_SIZE; }

    void erase(T* elem)
    {
        assert(elem != end());

        T* next_elem = elem + 1;
        while (next_elem != end())
        {
            *elem = *next_elem;
            elem++;
            next_elem++;
        }
        count_--;
    }

    T& operator[](size_t idx)
    {
        assert(idx < count_);
        return data_[idx];
    }

    const T& operator[](size_t idx) const
    {
        assert(idx < count_);
        return data_[idx];
    }

    const T& back() const { return (*this)[count_ - 1]; }
    const T& front() const { return (*this)[0]; }

    void push_back(T&& elem)
    {
        assert(count_ < MAX_SIZE);
        data_[count_] = elem;
        count_++;
    }
    void push_back(const T& elem)
    {
        assert(count_ < MAX_SIZE);
        data_[count_] = elem;
        count_++;
    }

  private:
    std::array<T, MAX_SIZE> data_{};
    size_t count_ = 0;
};
}