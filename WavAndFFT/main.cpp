#include "FFT.h"
#include "WavReader.h"

#include <cmath>

int main() {
  const double PORTION = 0.8;

  Wav::File file;
  file.Load("../samples/speech.wav");
  std::cout << "File loaded! Its info:\n" << file << std::endl;

  auto data = file.ExtractData();
  auto transformed = FFT::Transform(data);

  size_t fillingStart = transformed.size() * (1. - PORTION);
  size_t fillingEnd = transformed.size();
  std::fill(transformed.begin() + fillingStart,
            transformed.begin() + fillingEnd, 0);
  auto result = FFT::Transform(transformed, true);

  std::vector<Wav::File::Byte> newData;
  newData.reserve(data.size());
  for (size_t i = 0; i < data.size(); ++i) {
    newData.push_back(std::floor(result[i].real() + 0.5));
  }

  file.UpdateData(newData);
  file.Save("../samples/copy.wav");
  std::cout << "Transformed file saved!\n";
}