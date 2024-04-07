#include <kernel/interrupt_table.hpp>

template <Int32 Width, Int32 Height> struct Framebuffer {
  using Pixel = Int32;

  Pixel pixels[Width * Height];

  void clear();
};

template <Int32 Width, Int32 Height> void Framebuffer<Width, Height>::clear() {
  for (Int32 i = 0; i < Width; ++i) {
    for (Int32 j = 0; j < Height; ++j) {
      pixels[i * Height + j] = 0;
    }
  }
}

void _main() {
  Framebuffer<1024, 768> fb;
  fb.clear();
}

int main() {
  kernel::InterruptTable{{{kernel::Interrupts::Reset, _main}}};

  kernel::raise_interrupt(kernel::Interrupts::Reset);

  return 0;
}