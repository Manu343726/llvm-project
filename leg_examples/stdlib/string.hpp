#ifndef LEG_STDLIB_STRING_HPP_INCLUDED
#define LEG_STDLIB_STRING_HPP_INCLUDED

#include "stdint.hpp"

namespace stdlib {

inline void *next_aligned(void *address, Size alignment) {
  return BYTEPTR(address) + (alignment - (INTPTR(address) % alignment));
}

inline void memcpy(void *src, void *dest, Size size) {
  auto *src_ = BYTEPTR(src);
  auto *dest_ = BYTEPTR(dest);
  auto *end = BYTEPTR(src) + size;
  while (src_ != end) {
    *(src_++) = *(dest_++);
  }
}

} // namespace stdlib

#endif // LEG_STDLIB_STRING_HPP_INCLUDED