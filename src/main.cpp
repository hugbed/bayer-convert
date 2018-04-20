#include "subprocess.h"

#include <QApplication>

#include "ui/mainwindow.h"

//#include "subprocess.h"

int main (int argc, char* argv[])
{
  QApplication app(argc, argv);
  MainWindow mainWindow;
  mainWindow.show();
  app.exec();

// example to build video from separated images

//  // calls something like (2048: width, 1080: h)
//  // ffmpeg -r 24 -f image2 -s 2048x1080
//  //   -i /path/to/output/directory/%d.tif
//  //   -vcodec libx264 -crf 25 -pix_fmt rgb24
//  //   /path/to/output/video.mp4
//  auto obuf = subprocess::check_output({"ffmpeg",
//                                        "-r", "24",
//                                        "-f", "image2",
//                                        "-s", (std::to_string(width) + "x" + std::to_string(height)).c_str(),
//                                        "-i", (outputDirectory + "/%d.tif").c_str(),
//                                        "-vcodec", "libx264",
//                                        "-crf", "25",
//                                        "-pix_fmt", "rgb24",
//                                        (outputDirectory + "/" + outputFileName).c_str()});
//  std::cout << "Data : " << obuf.buf.data() << std::endl;

  return 0;
}
