#include "WavReader.h"

#include <fstream>
#include <iomanip>
#include <istream>
#include <iterator>

namespace Wav {

std::ostream &operator<<(std::ostream &os, const File::Duration &duration) {
  os << duration.hours << ":" << std::setw(2) << std::setfill('0')
     << static_cast<uint>(duration.minutes) << ":" << std::setw(2)
     << std::setfill('0') << static_cast<uint>(duration.seconds);
  return os;
}

std::ostream &operator<<(std::ostream &os, const File &file) {
  os << "Chunk id: \t\t" << file._header.chunkId << "\n";
  os << "Chunk size: \t\t" << file._header.chunkSize << " bytes\n";
  os << "File format: \t\t" << file._header.format << "\n";
  os << "Subchunk1 id: \t\t" << file._header.subchunk1Id << "\n";
  os << "Subchunk1 size: \t" << file._header.subchunk1Size << " bytes\n";
  os << "Audio format: \t\t" << file._header.audioFormat << "\n";
  os << "Number of channels: \t" << file._header.numChannels << "\n";
  os << "Sample rate: \t\t" << file._header.sampleRate << "\n";
  os << "Bits per sample: \t" << file._header.byteRate << "\n";
  os << "Subchunk2 id: \t\t" << file._header.subchunk2Id << "\n";
  os << "Subchunk2 size: \t" << file._header.subchunk2Size << " bytes\n";
  os << "Duration: \t\t" << file._duration;
  return os;
}

void File::LoadHeader(const std::vector<Byte> &fileData) {
  _header.chunkId = {fileData.begin(), fileData.begin() + 4};
  _header.chunkSize = FromBytesToInt<uint32_t, 4>(fileData.begin() + 4);
  _header.format = {fileData.begin() + 8, fileData.begin() + 12};
  _header.subchunk1Id = {fileData.begin() + 12, fileData.begin() + 16};
  _header.subchunk1Size = FromBytesToInt<uint32_t, 4>(fileData.begin() + 16);
  _header.audioFormat = FromBytesToInt<uint16_t, 2>(fileData.begin() + 20);
  _header.numChannels = FromBytesToInt<uint16_t, 2>(fileData.begin() + 22);
  _header.sampleRate = FromBytesToInt<uint32_t, 4>(fileData.begin() + 24);
  _header.byteRate = FromBytesToInt<uint32_t, 4>(fileData.begin() + 28);
  _header.blockAlign = FromBytesToInt<uint16_t, 2>(fileData.begin() + 32);
  _header.bitsPerSample = FromBytesToInt<uint16_t, 2>(fileData.begin() + 34);
  _header.subchunk2Id = {fileData.begin() + 36, fileData.begin() + 40};
  _header.subchunk2Size = FromBytesToInt<uint32_t, 4>(fileData.begin() + 40);
}

File::Duration File::ComputeDuration() const {
  auto seconds = static_cast<uint64_t>(
      1. * _header.subchunk2Size / (_header.bitsPerSample / 8.) /
      _header.numChannels / _header.sampleRate);
  Duration d{};
  d.hours = seconds / 60 / 60;
  d.minutes = seconds / 60 % 60;
  d.seconds = seconds % 60;
  return d;
}

void File::Load(const std::string &filePath) {
  std::ifstream file(filePath, std::ios::binary);
  if (!file.good()) {
    throw std::runtime_error("File not found!");
  }
  file.unsetf(std::ios::skipws);
  std::istream_iterator<Byte> begin(file), end;
  _data = {begin, end};
  LoadHeader(_data);
  _duration = ComputeDuration();
}

void File::Save(const std::string &filePath) const {
  std::ofstream file(filePath, std::ios::binary);
  if (!file.good()) {
    throw std::runtime_error("File can not be openned!");
  }
  file.unsetf(std::ios::skipws);
  std::ostream_iterator<Byte> out(file);
  std::copy(_data.begin(), _data.end(), out);
}

std::vector<File::Byte> File::ExtractData() const {
  return {_data.begin() + HEADER_SIZE, _data.end()};
}

void File::UpdateData(const std::vector<File::Byte> &newData) {
  if (_data.size() != HEADER_SIZE + newData.size()) {
    throw std::invalid_argument(
        "Header won't be correct after update.\nSizes don't match.");
  }
  std::copy(newData.begin(), newData.end(), _data.begin() + HEADER_SIZE);
}
} // namespace Wav
