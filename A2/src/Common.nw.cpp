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
