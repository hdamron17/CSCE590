\documentclass[10pt]{article}
\usepackage{noweb}
\usepackage{graphicx}
\usepackage[margin=1in]{geometry}

\title{CSCE 590 Assignment 1}
\author{Hunter Damron}
\date{\today}

\begin{document}

\maketitle

\section{Part 4: Single Images Operations}
For this section, the goal is to transform between RGB and HSI color models in order to invert the intensity of an image.

We begin by including the OpenCV2 library necessary for image operations.

<<Include>>=
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

<<Command line args>>=
cv::Mat image;
std::string path;
bool display;
if (argc == 2 || (argc == 3 && std::string("display") == argv[2])) {
  display = true;
} else if (argc == 3 && std::string("nodisplay") == (argv[2])) {
  display = false;
} else {
  std::cout << "Usage: `A1_Q4 <data_directory> [nodisplay|display]`" << std::endl
            << "  where data_directory contains subdirectories `images` and `output`"
            << "  and nodisplay or display is optional (display is on by default)"
            << std::endl;
  return 1;
}
path = argv[1];

@ Now we provide the function to apply on each pixel, transforming a 3-channel RGB pixel.

<<Transform function>>=
cv::Vec3d rgb2hsi(const cv::Vec3b& p) {
  cv::Vec3d rgb, rgb_unit, hsi;

  rgb = p;  // create Vec3d for floating point operations
  rgb /= 255;  // put each of rgb into [0, 1]

  double R = rgb[0], G = rgb[1], B = rgb[2];
  double theta = acos(0.5 * ((R - G) + (R - B)) / sqrt((R - G) * (R - G) + (R - B) * (G - B))) / (2 * M_PI);  // theta in range [0, 1)

  hsi[0] = (B <= G) ? theta : 1 - theta;  // Set hue
  hsi[2] = (rgb[0] + rgb[1] + rgb[2]) / 3;  // Set intensity
  hsi[1] = 1 - 1 / hsi[2] * fmin(rgb[0], fmin(rgb[1], rgb[2]));  // Set saturation

  if (p[0] == p[1] && p[1] == p[2]) {
    // In this case there will be a singularity where hue is arbitrary so set hue and saturation to 0
    hsi[0] = 0;
    hsi[1] = 0;
  }

  return hsi;
}

<<Transform function>>=
cv::Vec3d hsi2rgb(const cv::Vec3d& p) {
  cv::Vec3d hsi, gb_sector, rgb;

  // Set gb_sector to values as if hue is in RG sector then move them around if necessary
  double H = fmod(p[0], 1/3.0) * 2 * M_PI,  S = p[1],  I = p[2];  // recover hue pixel to range [0, 2*pi)
  gb_sector[0] = I * (1 - S);
  gb_sector[1] = I * (1 + (S * cos(H)) / (cos(M_PI / 3 - H)));
  gb_sector[2] = 3 * I - (gb_sector[0] + gb_sector[1]);

  for (int i = 0; i < 3; ++i) {
    // Truncate to valid range
    if (gb_sector[i] < 0)
      gb_sector[i] = 0;
    if (gb_sector[i] > 1)
      gb_sector[i] = 1;
  }

  // Rearrange colors depending on which sector hue is in
  if (p[0] < 1.0 / 3) {
    // RG sector: R->B, G->R, B->G
    rgb[0] = gb_sector[1];
    rgb[1] = gb_sector[2];
    rgb[2] = gb_sector[0];
  } else if (p[0] < 2.0 / 3) {
    // GB sector: copy gb_sector exactly
    rgb = gb_sector;
  } else {
    // BR sector: R->G, G->B, B->R
    rgb[0] = gb_sector[2];
    rgb[1] = gb_sector[0];
    rgb[2] = gb_sector[1];
  }

  return rgb;
}

<<Transform function>>=
cv::Vec3b invertPixel(const cv::Vec3b& p) {
  cv::Vec3d hsi, hsi_inv, rgb_inv;

  hsi = rgb2hsi(p);

  hsi_inv = hsi;
  hsi_inv[2] = 1 - hsi_inv[2];  // Set hsi_inv to hsi and invert the intensity  // TODO(HD) add this back

  rgb_inv = hsi2rgb(hsi_inv);
  return rgb_inv * 255;
}

@ Finally we begin the main function..

<<Q4.cpp>>=
<<Include>>
<<Transform function>>

int main(int argc, char* argv[]) {
  <<Command line args>>

  image = cv::imread(path + "/images/base_image.png", cv::IMREAD_COLOR);
  if(!image.data) {
    std::cout << "Failed to read image" << std::endl;
    return 1;
  }

  for (int i = 0; i < image.size().height; ++i) {
    for (int j = 0; j < image.size().width; ++j) {
      image.at<cv::Vec3b>(i, j) = invertPixel((image.at<cv::Vec3b>(i, j)));  // TODO(HD) only invert once in the future
    }
  }

  if (display) {
    cv::imshow("Inverted image", image);
    cv::waitKey(0);
  }

  imwrite(path + "/output/image_inverted.png", image);
}

@ Of course we want to include some of the output figures. The inverted image can be seen in Figure \ref{fig:image_inverted}

\begin{figure}[h]\label{fig:image_inverted}
  \centering
  \includegraphics[width=0.49\textwidth]{A1/images/base_image} \hfill%
  \includegraphics[width=0.49\textwidth]{A1/output/image_inverted}
  \caption{Original image (left) and image inverted (right)}
\end{figure}

@ Lastly, we used several other standard libraries which must be included
<<Include>>=
#include <iostream>
#include <cmath>
@

\end{document}
