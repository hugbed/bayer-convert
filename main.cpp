#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>

#include <cstdint>
#include <experimental/filesystem>
#include <future>

#include "subprocess.h"

using namespace std;
using namespace cv;
namespace fs = std::experimental::filesystem::v1;

/// perform the Simplest Color Balancing algorithm
void SimplestCB(Mat& in, Mat& out, float percent) {
  assert(in.channels() == 3);
  assert(percent > 0 && percent < 100);

  float half_percent = percent / 200.0f;

  vector<Mat> tmpsplit; split(in,tmpsplit);
  for(int i=0;i<3;i++) {
    //find the low and high precentile values (based on the input percentile)
    Mat flat; tmpsplit[i].reshape(1,1).copyTo(flat);
    cv::sort(flat,flat,CV_SORT_EVERY_ROW + CV_SORT_ASCENDING);
    int lowval = flat.at<uchar>(cvFloor(((float)flat.cols) * half_percent));
    int highval = flat.at<uchar>(cvCeil(((float)flat.cols) * (1.0 - half_percent)));
//    cout << lowval << " " << highval << endl;

    //saturate below the low percentile and above the high percentile
    tmpsplit[i].setTo(lowval,tmpsplit[i] < lowval);
    tmpsplit[i].setTo(highval,tmpsplit[i] > highval);

    //scale the channel
    normalize(tmpsplit[i],tmpsplit[i],0,255,NORM_MINMAX);
  }
  merge(tmpsplit,out);
}


template <class OIt, typename Size_t>
bool read_n_bytes(std::istream& stream, Size_t n, OIt it) {
  if (stream.peek() == EOF || n == 0) {
    return false;
  }
  std::copy_n(std::istreambuf_iterator<char>{stream}, n, it);
  stream.get();
  return !stream.eof();
}

std::vector<std::string> listFilesInDirectory(const std::string& path) {
  std::vector<std::string> fileNames;
  for (auto &entry : fs::directory_iterator(path)) {
    fileNames.emplace_back(entry.path());
  }
  std::sort(fileNames.begin(), fileNames.end());
  return fileNames;
}

void transcodeImages(std::vector<std::string> fileNames,
                     const std::string outputFilePath,
                     int height, int width, size_t nbBytes,
                     std::mutex* mutex,
                     int firstIndex, int lastIndex) {

  // allocation
  std::vector<uint8_t> imageData;
  imageData.reserve(nbBytes);

  int cvType = CV_MAKETYPE(CV_16U, 1);
  cv::Mat bayerSource(height, width, cvType, &imageData[0]);
  cv::Mat rgb16Dest(height, width, CV_16UC3);
  cv::Mat rgb8Dest(height, width, CV_8UC3);

  for (int i = firstIndex; i < lastIndex; ++i) {
    std::string inputFilePath = fileNames[i];
    {
      std::unique_lock<std::mutex> lock(*mutex);
      cout << i - firstIndex << "/" << lastIndex - firstIndex << "\n";
    }

    imageData.clear();

    // read image
    ifstream inputFile(inputFilePath, ios::binary);
    read_n_bytes(inputFile, nbBytes, std::back_inserter(imageData));

    // put data in MSB bits (on 16bits images)
    bayerSource *= 64;

    cv::cvtColor(bayerSource, rgb16Dest, CV_BayerGB2RGB_EA);
    rgb16Dest.convertTo(rgb8Dest, CV_8UC3, 1.0 / 255.0);
//    cv::imshow("before", rgb8Dest);
//
//    SimplestCB(rgb8Dest, rgb8Dest, 30.0f);
//    cv::imshow("after", rgb8Dest);
//    cv::waitKey();

    // use ffmpeg to reconstruct video from images
    // ffmpeg -r 24 -f image2 -s 2048x1080 -i %04d.tif -vcodec libx264 -crf 25 -pix_fmt rgb24 outputfile.mp4
    cv::imwrite(outputFilePath + "/" + std::to_string(i) + ".tif", rgb8Dest);
  }
}

// example usage:
// ./bayerconvert 2048 1080 /media/data/Dataset/Legault/elections_1/right /media/data/Dataset/Legault/elections_1/right_rgb ../right_rgb.mp4
int main (int argc, char* argv[])
{
  if (argc < 4)
  {
    cerr << "Usage : " << argv[0] << " width heigth [raw files directory] outputfilename " << endl;
    exit(-1);
  }

  int argumentCounter = 1;
  const int width = std::stoi(argv[argumentCounter++]);
  const int height = std::stoi(argv[argumentCounter++]);
  const std::string inputDirectory = argv[argumentCounter++];
  const std::string outputDirectory = argv[argumentCounter++];
  const std::string outputFileName = argv[argumentCounter++];
  const size_t nbBytes = width * height * sizeof(uint16_t);

  std::vector<std::string> fileNames = listFilesInDirectory(inputDirectory);

  std::cout << "width: " << width << '\n';
  std::cout << "height: " << height << '\n';
  std::cout << "output directory: " << outputDirectory << '\n';

  std::mutex writeMutex;

  std::vector<std::thread> threads;

  auto t1 = std::chrono::high_resolution_clock::now();

  const auto nbFiles = static_cast<int>(fileNames.size());
  const int nbThreads = 8;
  const int nbFilesPerThread = nbFiles / nbThreads;
  for (int i = 0; i < nbThreads; ++i) {
    int firstIndex = i * nbFilesPerThread;
    int lastIndex = firstIndex + nbFilesPerThread;
    if (i == (nbThreads - 1)) {
      lastIndex = static_cast<int>(fileNames.size());
    }
    threads.emplace_back(&transcodeImages, fileNames, outputDirectory, height, width, nbBytes, &writeMutex, firstIndex, lastIndex);
  }

  for (auto & thread : threads) {
    thread.join();
  }

  auto t2 = std::chrono::high_resolution_clock::now();

  std::cout << "Time to execute: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

  // calls something like
  // ffmpeg -r 24 -f image2 -s 2048x1080
  //   -i /media/data/Dataset/Legault/elections_3/left_rgb/%d.tif
  //   -vcodec libx264 -crf 25 -pix_fmt rgb24
  //   /media/data/Dataset/Legault/elections_3/left_rgb.mp4
  auto obuf = subprocess::check_output({"ffmpeg",
                                        "-r", "24",
                                        "-f", "image2",
                                        "-s", (std::to_string(width) + "x" + std::to_string(height)).c_str(),
                                        "-i", (outputDirectory + "/%d.tif").c_str(),
                                        "-vcodec", "libx264",
                                        "-crf", "25",
                                        "-pix_fmt", "rgb24",
                                        (outputDirectory + "/" + outputFileName).c_str()});
  std::cout << "Data : " << obuf.buf.data() << std::endl;

  return 0;
}
