#ifndef LEG_STDLIB_ARRAYVIEW_HPP_INCLUDED
#define LEG_STDLIB_ARRAYVIEW_HPP_INCLUDED

#include "initializer_list.hpp"
#include "stdint.hpp"

namespace stdlib {

struct RawArrayViewJumpingIterator {
  RawArrayViewJumpingIterator(Byte *ptr, Size jump) noexcept
      : _ptr{ptr}, _jump{jump} {}

  RawArrayViewJumpingIterator &operator++() {
    _ptr += _jump;
    return *this;
  }

  RawArrayViewJumpingIterator operator++(int) {
    auto before = *this;
    ++(*this);
    return before;
  }

  RawArrayViewJumpingIterator &operator--() {
    _ptr -= _jump;
    return *this;
  }

  RawArrayViewJumpingIterator operator--(int) {
    auto before = *this;
    --(*this);
    return before;
  }

  bool operator==(const RawArrayViewJumpingIterator &other) const {
    return _ptr == other._ptr;
  }

  bool operator!=(const RawArrayViewJumpingIterator &other) const {
    return !(*this == other);
  }

  bool operator==(Byte *other) const { return _ptr == other; }

  bool operator!=(Byte *other) const { return !(*this == other); }

private:
  Byte *_ptr;
  Size _jump;
};

struct RawArrayView {
  RawArrayView(void *begin, void *end)
      : _begin{BYTEPTR(begin)}, _end{BYTEPTR(end)} {}

  RawArrayView(void *begin, Size size)
      : _begin{BYTEPTR(begin)}, _end{BYTEPTR(begin) + size} {}

  Byte *begin() const { return _begin; }
  Byte *end() const { return _end; }
  Size size() const { return static_cast<Size>(_end - _begin); }

private:
  Byte *_begin;
  Byte *_end;
};

struct JumpingRawArrayView {
  JumpingRawArrayView(const RawArrayView &view, const Size jump) noexcept
      : _view{&view}, _jump{jump} {}

  RawArrayViewJumpingIterator begin() const { return {_view->begin(), _jump}; }
  Byte *end() const { return _view->end(); }
  Size size() const { return _view->size() / _jump; }

private:
  const RawArrayView *_view;
  Size _jump;
};

template <typename T> struct ArrayView {
  constexpr ArrayView(T *begin, T *end) noexcept : _begin{begin}, _end{end} {}
  constexpr ArrayView(T *begin, Size size) noexcept
      : ArrayView{begin, begin + size} {}
  constexpr ArrayView(const std::initializer_list<T> &list)
      : ArrayView{std::begin(list), std::end(list)} {}

  T *begin() const { return _begin; }
  T *end() const { return _end; }
  Size size() const { return static_cast<Size>(_end - _begin); }
  const T &operator[](const Size i) const { return *(_begin + i); }
  T &operator[](const Size i) { return *(_begin + i); }

  JumpingRawArrayView raw() const { return {_begin, _end, sizeof(T)}; }
  RawArrayView bytes() const { return {_begin, _end}; }

private:
  T *_begin;
  T *_end;
};
} // namespace stdlib
#endif // LEG_STDLIB_ARRAYVIEW_HPP_INCLUDED