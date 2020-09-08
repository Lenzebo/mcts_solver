#pragma once
#include <cassert>
#include <algorithm>

namespace mcts {

template <typename T, size_t MAX_SIZE>
class MaxSizeVector
{
  public:
    MaxSizeVector() = default;
    constexpr MaxSizeVector(const MaxSizeVector& other) : data_(other.data_), count_(other.count_) {}
    MaxSizeVector(const std::vector<T>& other) { insert(begin(), other.begin(), other.end()); }
    constexpr MaxSizeVector(MaxSizeVector&& other) noexcept
        : data_(std::move(other.data_)), count_(std::move(other.count_))
    {
    }

    constexpr MaxSizeVector& operator=(const MaxSizeVector& other)
    {
        data_ = other.data_;
        count_ = other.count_;
        return *this;
    }

    constexpr MaxSizeVector& operator=(MaxSizeVector&& other) noexcept
    {
        data_ = std::move(other.data_);
        count_ = std::move(other.count_);
        return *this;
    }

    [[nodiscard]] constexpr T* begin() { return data_.begin(); }
    [[nodiscard]] constexpr T* end() { return begin() + count_; }
    [[nodiscard]] constexpr const T* begin() const { return data_.begin(); }
    [[nodiscard]] constexpr const T* end() const { return begin() + count_; }

    constexpr void reserve(size_t newSize)
    {
        assert(newSize < MAX_SIZE);
        (void)newSize;
    }

    constexpr void clear() { count_ = 0; }

    template <class InputIterator>
    constexpr T* insert(T* position, InputIterator first, InputIterator last)
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

    [[nodiscard]] constexpr bool empty() const { return count_ == 0; }
    [[nodiscard]] constexpr size_t size() const { return count_; }
    [[nodiscard]] constexpr size_t capacity() const { return MAX_SIZE; }

    constexpr void erase(T* elem)
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

    constexpr T& operator[](size_t idx)
    {
        assert(idx < count_);
        return data_[idx];
    }

    constexpr const T& operator[](size_t idx) const
    {
        assert(idx < count_);
        return data_[idx];
    }

    constexpr const T& back() const { return (*this)[count_ - 1]; }
    constexpr const T& front() const { return (*this)[0]; }

    constexpr void push_back(T&& elem)
    {
        assert(count_ < MAX_SIZE);
        data_[count_] = elem;
        count_++;
    }
    constexpr void push_back(const T& elem)
    {
        assert(count_ < MAX_SIZE);
        data_[count_] = elem;
        count_++;
    }

  private:
    std::array<T, MAX_SIZE> data_{};
    size_t count_ = 0;
};
}  // namespace mcts