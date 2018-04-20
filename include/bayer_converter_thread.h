#ifndef BAYER_CONVERT_BAYER_CONVERTER_THREAD_H
#define BAYER_CONVERT_BAYER_CONVERTER_THREAD_H

#include <QThread>

#include "file.h"
#include "types.h"

#include <QList>
#include <QString>

class BayerConverterThread : public QThread {
Q_OBJECT
public:
  BayerConverterThread(
    QList<ImagePath> inputFileNames,
    QString outputPath,
    RawImageInfo rawImageInfo
  );

signals:
  void imageTranscoded();

protected:
  void run() override;

private:
  void transcodeImages();

  QList<ImagePath> inputFileNames_;
  QString outputPath_;
  RawImageInfo rawImageInfo_;
};

#endif //BAYER_CONVERT_BAYER_CONVERTER_THREAD_H
