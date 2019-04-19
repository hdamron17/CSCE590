@ \section*{Common Code Sections}

This section provides several insignificant code snippets which are used by the other code sections.

<<Include>>=
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <cmath>
#include <set>
#include <limits>
#include <fstream>

<<Global constants>>=
const std::vector<int> PNG_COMPRESSION = {CV_IMWRITE_PNG_COMPRESSION, 9};

<<Command line args>>=
std::string path;
bool display;
if (argc == 2 || (argc == 3 && std::string("display") == argv[2])) {
  display = true;
} else if (argc == 3 && std::string("nodisplay") == (argv[2])) {
  display = false;
} else {
  std::cout << "Usage: `" << argv[0] << " <data_dir> [nodisplay|display]`" << std::endl
            << "  where data_directory contains subdirectories `images` and `output`"
            << "  and nodisplay or display is optional (display is on by default)"
            << std::endl;
  return 1;
}
path = argv[1];

<<[[im_load]] Function>>=
template<typename T>
bool im_load(const std::string& im_path, cv::Mat_<T>* mat, int mode=cv::IMREAD_COLOR) {
  cv::Mat tmp = cv::imread(im_path, mode);
  if(!tmp.data) {
    std::cout << "Failed to read image " << im_path << std::endl;
    return false;
  }
  tmp.assignTo(*mat, cv::traits::Type<T>::value);
  return true;
}

<<[[in_range]] Function>>=
template<typename T>
cv::Mat_<T> in_range(const cv::Mat_<T>& image) {
  double min_p, max_p;
  cv::minMaxLoc(image, &min_p, &max_p);
  return (image - min_p) / (max_p - min_p) * std::numeric_limits<T>::max();
}
