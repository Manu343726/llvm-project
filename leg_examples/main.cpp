static_assert(sizeof(int) == 4);
static_assert(sizeof(void *) == 4);
using Int32 = int;

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

int main() {
  Framebuffer<1024, 768> fb;

  fb.clear();

  return 0;
}