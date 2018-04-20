#ifndef BAYER_CONVERT_BAYER_CONVERTER_H
#define BAYER_CONVERT_BAYER_CONVERTER_H

#include "bayer_converter_thread.h"
#include "types.h"

#include <QObject>
#include <QList>

class BayerConverter : public QObject {
  Q_OBJECT
public:
  BayerConverter(QList<QString> inputFilenames, QString outputPath, RawImageInfo rawImageInfo);

  ~BayerConverter() override;

  void start();

  QList<ImagePath> subList(const QList<QString> inputList, int firstIndex, int lastIndex);

signals:
  void conversionProgressed(int currentNb, int total);

private:
  void onImageTranscoded();

  int nbTranscoded_{};
  QList<QString> inputFilenames_;
  QList<QThread*> threadPool_;
};

#endif //BAYER_CONVERT_BAYER_CONVERTER_H
