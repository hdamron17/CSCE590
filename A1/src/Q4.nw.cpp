\documentclass{article}
\usepackage{noweb}
\usepackage{graphicx}

\begin{document}

\section{Part 4: Single Images Operations}
For this section, the goal is to transform between RGB and HSI color models in order to invert the intensity of an image.

We begin by including the OpenCV2 library necessary for image operations.

<<Include>>=
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

@ Now we provide the function to apply on each pixel, transforming a 3-channel RGB pixel.

<<Transform function>>=
cv::Vec3d rgb2hsi(const cv::Vec3b& p) {
  cv::Vec3d rgb, rgb_unit, hsi;

  rgb = p;  // create Vec3d for floating point operations
  rgb /= 255;  // put each of rgb into [0, 1]

  double mag = sqrt(rgb[0] * rgb[0] + rgb[1] * rgb[1] + rgb[2] * rgb[2]);
  rgb_unit = rgb / mag;  // normalize vector rgb to have length 1

  double R = rgb_unit[0], G = rgb_unit[1], B = rgb_unit[2];
  double theta = acos(0.5 * ((R - G) + (R - B)) / sqrt((R - G) * (R - G) + (R - B) * (G - B))) / (2 * M_PI);  // theta in range [0, 1)

  hsi[0] = (B <= G) ? theta : 1 - theta;  // Set hue
  hsi[2] = (rgb[0] + rgb[1] + rgb[2]) / 3;  // Set intensity
  hsi[1] = 1 - 1 / hsi[2] * fmin(rgb[0], fmin(rgb[1], rgb[2]));

  if (p[0] == p[1] && p[1] == p[2]) {
    // In this case there will be a singularity where hue is arbitrary so set hue and saturation to 0
    hsi[0] = 0;
    hsi[1] = 0;
  }

  return hsi;
}

<<Transform function>>=
cv::Vec3d hsi2rgb(const cv::Vec3d& p) {
  cv::Vec3d hsi, gb_sector, rgb_inv;

  // Set gb_sector to values as if hue is in RG sector then move them around if necessary
  double H = fmod(p[0], 1/3.0) * 2 * M_PI,  S = p[1],  I = p[2];  // recover hue pixel to range [0, 2*pi)
  // std::cout << "DBG: HSI = " << H << "," << S << "," << I << std::endl;
  gb_sector[0] = I * (1 - S);
  gb_sector[1] = I * (1 + (S * cos(H)) / (cos(M_PI / 3 - H)));
  gb_sector[2] = 3 * I - (gb_sector[0] + gb_sector[1]);

  // gb_sector[1] /= 3;  // TODO(HD) remove this probably

  // Rearrange colors depending on which sector hue is in
  if (p[0] < 1.0 / 3) {
    // RG sector: R->B, G->R, B->G
    rgb_inv[0] = gb_sector[1];
    rgb_inv[1] = gb_sector[2];
    rgb_inv[2] = gb_sector[0];
  } else if (p[0] < 2.0 / 3) {
    // GB sector: copy gb_sector exactly
    rgb_inv = gb_sector;
  } else {
    // BR sector: R->G, G->B, B->R
    rgb_inv[0] = gb_sector[2];
    rgb_inv[1] = gb_sector[0];
    rgb_inv[2] = gb_sector[1];
  }

  if (rgb_inv[0] < 0 || rgb_inv[0] > 1
    ||rgb_inv[1] < 0 || rgb_inv[1] > 1
    ||rgb_inv[2] < 0 || rgb_inv[2] > 1)
      std::cout << "ERROR: invalid gb_sector" << std::endl;
  // std::cout << "DBG gb_sector: " << gb_sector << " from hsi = " << p << " with H = " << H * 180/M_PI << std::endl;

  // std::cout << "DBG: hsi2rgb(" << p << ") = " << rgb_inv * 255  << " (gb_sector = " << gb_sector * 255 << ")" << std::endl;
  return rgb_inv;
}

<<Transform function>>=
cv::Vec3b invertPixel(const cv::Vec3b& p) {
  cv::Vec3d hsi, hsi_inv, rgb_inv;

  hsi = rgb2hsi(p);

  hsi_inv = hsi;
  hsi_inv[2] = 1 - hsi_inv[2];  // Set hsi_inv to hsi and invert the intensity

  rgb_inv = hsi2rgb(hsi_inv);
  return rgb_inv * 255;
}

@ Finally we begin the main function..

<<Q4.cpp>>=
<<Include>>
<<Transform function>>

cv::Vec3b rgb(int r, int g, int b) {
  cv::Vec3b v;
  v[0] = r;  v[1] = g;  v[2] = b;
  return v;
}

cv::Vec3d hsi(double h, double s, double i) {
  cv::Vec3d v;
  v[0] = h;  v[1] = s;  v[2] = i;
  return v;
}

void p(const cv::Vec3d& v) {
  std::cout << "PRINT: " << v << std::endl;
}

int main() {
  cv::Mat image;
  image = cv::imread("../../A1/images/cl2.png", cv::IMREAD_COLOR);
  if(!image.data) {
    std::cout << "Failed to read image" << std::endl;
  }

  std::cout << "DBG: " << image.size() << ", " << image.type() << ", " << image.channels() << ", " << image.at<cv::Vec3b>(0,0) << std::endl;
  for (int i = 0; i < image.size().height; ++i) {
    for (int j = 0; j < image.size().width; ++j) {
      image.at<cv::Vec3b>(i, j) = invertPixel(invertPixel((image.at<cv::Vec3b>(i, j))));  // TODO(HD) only invert once in the future
    }
  }
  // cv::resize(image, image, cv::Size(300, 300));
  // cv::imshow("Inverted image", image);
  // cv::waitKey(0);

  imwrite( "../../A1/images/cl2_inverted.png", image );

  // rgb2hsi(rgb(0, 0, 0));  // should be 0,0,0
  // rgb2hsi(rgb(255, 0, 0));  // should be 0,1,0.333
  // rgb2hsi(rgb(0, 255, 0));  // should be 0.33,1,0.333
  // rgb2hsi(rgb(0, 0, 255));  // should be 0.66,1,0.333
  // rgb2hsi(rgb(255, 255, 0));  // should be 0.16,1,0.666
  // rgb2hsi(rgb(0, 128, 0));  // should be 0.33,1,0.167
  // rgb2hsi(rgb(255, 255, 255));  // should be 0,0,1

  p(hsi2rgb(hsi(0, 0, 0)));  // should be 0,0,0
  p(hsi2rgb(hsi(0, 1, 1/3.0)));  // should be 1,0,0
  p(hsi2rgb(hsi(1/3.0, 1, 1/3.0)));  // should be 0,1,0
  p(hsi2rgb(hsi(2/3.0, 1, 1/3.0)));  // should be 0,0,1
  p(hsi2rgb(hsi(1/6.0, 1, 2/3.0)));  // should be 1,1,0
  p(hsi2rgb(hsi(1/3.0, 1, 1/6.0)));  // should be 0,0.5,0
  p(hsi2rgb(hsi(0, 0, 1)));  // should be 1,1,1
  p(hsi2rgb(hsi(0.5, 1, 0.5)));  // 0,1,1
  p(hsi2rgb(hsi(1, 1, 1)));  // 0,0,1
  p(hsi2rgb(hsi(0.00, 0, 0.6)));
  p(hsi2rgb(hsi(0.34, 0, 0.6)));
  p(hsi2rgb(hsi(0.67, 0, 0.6)));
  p(hsi2rgb(hsi(0, 1, 1)));
}

@ Of course we want to include some of the output figures. The inverted image can be seen in Figure \ref{fig:cl2_inverted}

\begin{figure}[h]\label{fig:cl2_inverted}
  \centering
  \caption{Inverted image}
  \includegraphics[width=0.5\textwidth]{A1/images/cl2_inverted}
\end{figure}

@ Now on to histograms (kind of)

@ Lastly, we used several other standard libraries which must be included
<<Include>>=
#include <iostream>
#include <cmath>
@

\end{document}
