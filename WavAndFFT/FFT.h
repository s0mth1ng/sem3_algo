#pragma once

#include <algorithm>
#include <complex>
#include <vector>

namespace FFT {
using Complex = std::complex<long double>;

void ComplementToPowerOfTwo(std::vector<Complex> &data);
std::pair<std::vector<Complex>, std::vector<Complex>>
SplitByTwo(const std::vector<Complex> &data);

std::vector<Complex> GeneralTransform(std::vector<Complex> data);

template <typename T>
std::vector<Complex> Transform(const std::vector<T> &data,
                               bool isInversed = false) {
  std::vector<Complex> complexData(data.begin(), data.end());
  complexData = GeneralTransform(complexData);
  if (isInversed) {
    for (auto &value : complexData) {
      value /= complexData.size();
    }
    std::reverse(complexData.begin() + 1, complexData.end());
  }
  return complexData;
}

} // namespace FFT