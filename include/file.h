#ifndef BAYER_CONVERT_FILE_H
#define BAYER_CONVERT_FILE_H

#include <fstream>

template <class OIt, typename Size_t>
bool read_n_bytes(std::istream& stream, Size_t n, OIt it) {
  if (stream.peek() == EOF || n == 0) {
    return false;
  }
  std::copy_n(std::istreambuf_iterator<char>{stream}, n, it);
  stream.get();
  return !stream.eof();
}

#endif //BAYER_CONVERT_FILE_H
