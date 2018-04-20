#include "bayer_converter.h"

BayerConverter::BayerConverter(
  QList<QString> inputFilenames,
  QString outputPath,
  RawImageInfo rawImageInfo
)
: inputFilenames_{std::move(inputFilenames)}
{
  const auto nbFiles = static_cast<int>(inputFilenames_.size());
  const int nbThreads = 8;
  const int nbFilesPerThread = nbFiles / nbThreads;
  for (int i = 0; i < nbThreads; ++i) {
    int firstIndex = i * nbFilesPerThread;
    int lastIndex = firstIndex + nbFilesPerThread;
    if (i == (nbThreads - 1)) {
      lastIndex = static_cast<int>(inputFilenames_.size());
    }

    QList<ImagePath> threadInputFiles = subList(inputFilenames_, firstIndex, lastIndex);
    auto* converterThread = new BayerConverterThread(threadInputFiles, outputPath, rawImageInfo);
    connect(converterThread, &BayerConverterThread::imageTranscoded, [this] { onImageTranscoded(); });
    threadPool_.push_back(converterThread);
  }
}

BayerConverter::~BayerConverter() {
  for (auto* thread : threadPool_) {
    thread->wait();
    delete thread;
  }
  threadPool_.clear();
}

void BayerConverter::start() {
  nbTranscoded_ = 0;
  for (auto* thread : threadPool_) {
    thread->start();
  }
}

QList<ImagePath> BayerConverter::subList(const QList<QString> inputList, int firstIndex, int lastIndex) {
  QList<ImagePath> newList;
  for (int i = firstIndex; i < lastIndex; ++i) {
    newList.push_back({i, inputList[i]});
  }
  return newList;
}

void BayerConverter::onImageTranscoded() {
  nbTranscoded_++;
  emit conversionProgressed(nbTranscoded_+1, inputFilenames_.size());
};