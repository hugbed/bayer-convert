#ifndef BAYER_CONVERT_TYPES_H
#define BAYER_CONVERT_TYPES_H

#include <QString>

#include <cstddef>

struct RawImageInfo {
  int width;
  int height;
  size_t nbBytes;
};

struct ImagePath {
  int index;
  QString path;
};

#endif //BAYER_CONVERT_TYPES_H
