#pragma once

template <typename T, typename Parameter>
class NamedType
{
  public:
    NamedType() = default;
    explicit constexpr NamedType(T value) : value(value){};
    explicit operator T() const { return value; }  // NOLINT
    bool operator==(const NamedType& other) const { return value == other.value; }
    bool operator!=(const NamedType& other) const { return value != other.value; }
    T get() const { return value; }

  private:
    T value;
};