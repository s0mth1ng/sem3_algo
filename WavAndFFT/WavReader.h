#pragma once

#include <cmath>
#include <cstdio>
#include <cstring>
#include <endian.h>
#include <iostream>
#include <vector>

namespace Wav {

static const uint16_t HEADER_SIZE = 44;

struct Header {
  std::string chunkId;
  uint32_t chunkSize;
  std::string format;
  std::string subchunk1Id;
  uint32_t subchunk1Size;
  uint16_t audioFormat;
  uint16_t numChannels;
  uint32_t sampleRate;
  uint32_t byteRate;
  uint16_t blockAlign;
  uint16_t bitsPerSample;
  std::string subchunk2Id;
  uint32_t subchunk2Size;
};

class File {
public:
  using Byte = uint8_t;
  struct Duration {
    uint32_t hours;
    uint8_t minutes;
    uint8_t seconds;
  };

  File() = default;

  void Load(const std::string &filePath);

  void Save(const std::string &filePath) const;

  friend std::ostream &operator<<(std::ostream &os, const File &file);

  std::vector<Byte> ExtractData() const;

  void UpdateData(const std::vector<Byte> &newData);

private:
  using DataIt = std::vector<Byte>::const_iterator;
  Header _header;
  std::vector<Byte> _data;
  Duration _duration{};

  Duration ComputeDuration() const;

  void LoadHeader(const std::vector<Byte> &fileData);

  template <typename T, uint8_t N> T FromBytesToInt(DataIt begin) {
    T res = 0;
    std::memcpy(&res, &(*begin), sizeof(T));
    return res;
  }
};
} // namespace Wav