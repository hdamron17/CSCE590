\documentclass[10pt]{article}
\usepackage{noweb}
\usepackage{graphicx}
\usepackage[margin=1in]{geometry}

\begin{document}

\section{Part 1: Single Images Operations}
For this section, the goal is to report on the differences between various image file formats.

We begin by including the OpenCV2 library necessary for image operations.

<<Include>>=
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

<<Part 1 Globals>>=
const std::array<std::string, 4> file_types = {"jpg", "png", "ppm", "tiff"};

<<[[multisave]] function>>=
void multisave(const std::string& file_type, const std::string& file_name, const std::string& rel_path) {
  // input file path is `{rel_path}/images/{file_name}.{file_type}`

  cv::Mat image;
  image = cv::imread(rel_path + "/images/" + file_name + "." + file_type, cv::IMREAD_COLOR);
  if(!image.data) {
    std::cout << "Failed to read " << file_type << " image " << (file_name + "." + file_type) << std::endl;
    return;
  }

  for (int i = 0; i < file_types.size(); ++i) {
    if (file_types[i] != file_type)
      cv::imwrite(rel_path + "/output/" + file_name + "_to_" + file_types[i] + "." + file_types[i], image);
  }
}

@ Finally we begin the main function..

<<Q1.cpp>>=
<<Include>>
<<Part 1 Globals>>
<<[[multisave]] function>>

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cout << "Usage: `A1_Q4 <data_directory>`" << std::endl
              << "  where data_directory contains subdirectories `images` and `output`"
              << std::endl;
    return 1;
  }
  std::string path = argv[1];

  multisave("png",  "png_image",  path);
  multisave("jpg",  "jpg_image",  path);
  multisave("ppm",  "ppm_image",  path);
  multisave("tiff", "tiff_image", path);
}

@ The images before and after conversion can be seen in Figure \ref{fig:image_matrix}.


\begin{figure}\label{fig:image_matrix}
  \resizebox{\textwidth}{!}{
    \begin{tabular}{|c|c|c|c|c|}
      \hline  ~ & PNG & JPEG & TIFF & PPM \\ \hline
      PNG  & \includegraphics[width=0.24\textwidth]{A1/images/png_image} &
             \includegraphics[width=0.24\textwidth]{A1/output/png_image_to_jpg} &
             \includegraphics[width=0.24\textwidth]{A1/output/png_image_to_tiff} &
             \includegraphics[width=0.24\textwidth]{A1/output/png_image_to_ppm} \\ \hline
      JPG  & \includegraphics[width=0.24\textwidth]{A1/output/jpg_image_to_png} &
             \includegraphics[width=0.24\textwidth]{A1/images/jpg_image} &
             \includegraphics[width=0.24\textwidth]{A1/output/jpg_image_to_tiff} &
             \includegraphics[width=0.24\textwidth]{A1/output/jpg_image_to_ppm} \\ \hline
      TIFF & \includegraphics[width=0.24\textwidth]{A1/output/tiff_image_to_png} &
             \includegraphics[width=0.24\textwidth]{A1/output/tiff_image_to_jpg} &
             \includegraphics[width=0.24\textwidth]{A1/images/tiff_image} &
             \includegraphics[width=0.24\textwidth]{A1/output/tiff_image_to_ppm} \\ \hline
      PPM  & \includegraphics[width=0.24\textwidth]{A1/output/ppm_image_to_png} &
             \includegraphics[width=0.24\textwidth]{A1/output/ppm_image_to_jpg} &
             \includegraphics[width=0.24\textwidth]{A1/output/ppm_image_to_tiff} &
             \includegraphics[width=0.24\textwidth]{A1/images/ppm_image} \\ \hline
    \end{tabular}
  }
  \caption{Matrix of images before and after conversion (left axis is input type; top axis is output type)}
\end{figure}

@ Lastly, we used several other standard libraries which must be included
<<Include>>=
#include <iostream>
#include <cmath>
#include <array>
@

\end{document}
