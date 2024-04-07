#include <stdlib/array_view.hpp>
#include <stdlib/stdint.hpp>
#include <stdlib/tuple.hpp>

namespace kernel {
using InterruptFunction = void (*)();
static_assert(sizeof(InterruptFunction) == 4);

enum class Interrupts : Int32 {
  Reset,

  TOTAL_INTERRUPTS,
};

constexpr Size TotalInterrupts =
    static_cast<Size>(Interrupts::TOTAL_INTERRUPTS);

constexpr Size InterruptTableSize = sizeof(InterruptFunction) * TotalInterrupts;

constexpr void *INTERRUPT_TABLE_ADDRESS = (void *)0x00000000;

class InterruptTable {
public:
  using Functions =
      stdlib::ArrayView<const stdlib::Pair<Interrupts, InterruptFunction>>;

  InterruptTable(volatile void *tableLocation = INTERRUPT_TABLE_ADDRESS,
                 const Functions &functions = {})
      : _table{reinterpret_cast<volatile InterruptFunction *>(tableLocation),
               InterruptTableSize} {
    for (const auto &[interrupt, function] : functions) {
      (*this)[interrupt] = function;
    }
  }

  InterruptTable(const Functions &functions)
      : InterruptTable{INTERRUPT_TABLE_ADDRESS, functions} {}

  InterruptFunction operator[](const Interrupts i) const {
    return _table[Size(i)];
  }

  volatile InterruptFunction &operator[](const Interrupts i) {
    return _table[Size(i)];
  }

  void raise(const Interrupts interrupt) {
    if (const auto handler = (*this)[interrupt]; handler != nullptr) {
      handler();
    }
  }

private:
  stdlib::ArrayView<volatile InterruptFunction> _table;
};

inline void raise_interrupt(const Interrupts interrupt) {
  InterruptTable().raise(interrupt);
}

} // namespace kernel