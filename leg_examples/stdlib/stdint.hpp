#ifndef LEG_STDLIB_STDINT_HPP_INCLUDED
#define LEG_STDLIB_STDINT_HPP_INCLUDED

#include "stddef.hpp"

namespace stdlib {

using Int32 = int;
using Word = Int32;
using Byte = unsigned char;
using IntPtr = Word;
static_assert(sizeof(Int32) == 4);
static_assert(sizeof(Byte) == 1);
using Size = unsigned int;
static_assert(sizeof(Size) == 4);

} // namespace stdlib

#define INTPTR(x) reinterpret_cast<::stdlib::IntPtr>((x))
#define BYTEPTR(x) reinterpret_cast<::stdlib::Byte *>((x))

using Int32 = stdlib::Int32;
using Word = stdlib::Word;
using Byte = stdlib::Byte;
using IntPtr = stdlib::IntPtr;
using Size = stdlib::Size;

#endif // LEG_STDLIB_STDINT_HPP_INCLUDED