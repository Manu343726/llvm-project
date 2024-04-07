#ifndef LEG_STDLIB_STDDEF_HPP_INCLUDED
#define LEG_STDLIB_STDDEF_HPP_INCLUDED

#ifndef __leg__
#error "LEG target not enabled"
#endif

static_assert(sizeof(void *) == 4);

#endif // LEG_STDLIB_STDDEF_HPP_INCLUDED