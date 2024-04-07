#ifndef LEG_STDLIB_INITIALIZERLIST_HPP_INCLUDED
#define LEG_STDLIB_INITIALIZERLIST_HPP_INCLUDED

#include "stdint.hpp"

namespace std {

template <typename T> class initializer_list {
private:
  const T *m_first;
  const T *m_last;

public:
  using value_type = T;
  using reference = T &;
  using const_reference = const T &;
  using size_type = Size;
  using iterator = T *;
  using const_iterator = const T *;

  constexpr initializer_list() : m_first{nullptr}, m_last{nullptr} {}

  // Number of elements.
  constexpr size_type size() const noexcept { return m_last - m_first; }

  // First element.
  constexpr const T *begin() const noexcept { return m_first; }

  // One past the last element.
  constexpr const T *end() const noexcept { return m_last; }
};

template <typename T> constexpr T *begin(const std::initializer_list<T> &list) {
  return list.begin();
}

template <typename T> constexpr T *end(const std::initializer_list<T> &list) {
  return list.end();
}

} // namespace std

#endif // LEG_STDLIB_INITIALIZERLIST_HPP_INCLUDED