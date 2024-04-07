#ifndef LEG_STDLIB_TUPLE_HPP_INCLUDED
#define LEG_STDLIB_TUPLE_HPP_INCLUDED

namespace stdlib {

template <typename First, typename Second> struct Pair {
  First first;
  Second second;
};

} // namespace stdlib

#endif // LEG_STDLIB_TUPLE_HPP_INCLUDED