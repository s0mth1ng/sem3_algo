#include "FFT.h"

namespace FFT {
void ComplementToPowerOfTwo(std::vector<Complex> &data) {
  size_t n = 1;
  while (n < data.size()) {
    n <<= 1lu;
  }
  data.resize(n);
}

std::vector<Complex> GeneralTransform(std::vector<Complex> data) {
  if (data.size() < 2) {
    return data;
  }
  ComplementToPowerOfTwo(data);
  size_t sz = data.size();
  auto [evens, odds] = SplitByTwo(data);
  evens = GeneralTransform(evens);
  odds = GeneralTransform(odds);
  long double ang = 2 * M_PI / sz;
  for (size_t i = 0; i < sz; ++i) {
    Complex w(std::cos(ang * i), std::sin(ang * i));
    data[i] = evens[i % (sz / 2)] + w * odds[i % (sz / 2)];
  }
  return data;
}

std::pair<std::vector<Complex>, std::vector<Complex>>
SplitByTwo(const std::vector<Complex> &data) {
  std::vector<Complex> evens, odds;
  size_t size = data.size();
  evens.reserve(size / 2);
  odds.reserve(size / 2);
  for (size_t i = 0; i < data.size(); i += 2) {
    evens.push_back(data[i]);
    odds.push_back(data[i + 1]);
  }
  return {evens, odds};
}
} // namespace FFT
