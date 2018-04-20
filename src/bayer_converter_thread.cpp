#include "bayer_converter.h"

#include <opencv2/opencv.hpp>

BayerConverterThread::BayerConverterThread(
  QList<ImagePath> inputFileNames,
  QString outputPath,
  RawImageInfo rawImageInfo
)
  : inputFileNames_{std::move(inputFileNames)}
  , outputPath_{std::move(outputPath)}
  , rawImageInfo_{std::move(rawImageInfo)}
{
}

void BayerConverterThread::run() {
  transcodeImages();
}

void BayerConverterThread::transcodeImages() {
  // pre-allocation
  std::vector<uint8_t> imageData;
  imageData.reserve(rawImageInfo_.nbBytes);

  int cvType = CV_MAKETYPE(CV_16U, 1);
  cv::Mat bayerSource(rawImageInfo_.height, rawImageInfo_.width, cvType, &imageData[0]);
  cv::Mat rgb16Dest(rawImageInfo_.height, rawImageInfo_.width, CV_16UC3);
  cv::Mat rgb8Dest(rawImageInfo_.height, rawImageInfo_.width, CV_8UC3);

  int i = 0;
  for (auto & inputFilePath : inputFileNames_) {
    imageData.clear();

    // read image
    std::ifstream inputFile(inputFilePath.path.toStdString(), std::ios::binary);
    read_n_bytes(inputFile, rawImageInfo_.nbBytes, std::back_inserter(imageData));

    // put data in MSB bits (on 16bits images)
    bayerSource *= 64;

    cv::cvtColor(bayerSource, rgb16Dest, CV_BayerGB2RGB_EA);
    rgb16Dest.convertTo(rgb8Dest, CV_8UC3, 1.0 / 255.0);

    // use ffmpeg to reconstruct video from images
    // ffmpeg -r 24 -f image2 -s 2048x1080 -i %04d.tif -vcodec libx264 -crf 25 -pix_fmt rgb24 outputfile.mp4
    cv::imwrite(outputPath_.toStdString() + "/" + std::to_string(inputFilePath.index) + ".tif", rgb8Dest);
    emit imageTranscoded();
  }
}
