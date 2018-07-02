\documentclass{article}
\usepackage{noweb}
\usepackage{graphicx}
\usepackage{pgfplots}
\pgfplotsset{compat=1.8}

\begin{document}

\section{Part 4: Single Images Operations}
For this section, the goal is to create a histogram of intensities in an image.

We begin by including the OpenCV2 library necessary for image operations and plotting.

<<Include>>=
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

@ Finally we begin the main function.

<<Q2.cpp>>=
<<Include>>

template<typename T> void saveMatCsv(const cv::Mat& mat, std::ofstream& out) {
  for (int i = 0; i < mat.size().height; ++i) {
    for (int j = 0; j < mat.size().width; ++j) {
      out << mat.at<T>(i, j);
      if (j + 1 != mat.size().width)
        out << ", ";
    }
    out << std::endl;
  }
}

cv::Mat my_calcHist(cv::Mat image, int bins) {
  double category_size = 1.0 / (bins-1);

  cv::Mat hist(bins, 2, CV_64F);

  for (int i = 0; i < bins; ++i) {
    hist.at<double>(i, 0) = i;
  }

  hist.col(1) = cv::Mat::zeros(bins, 1, CV_64F);

  for (int i = 0; i < image.size().height; ++i) {
    for (int j = 0; j < image.size().width; ++j) {
      const cv::Vec3b& p = image.at<cv::Vec3b>(i, j);
      double intensity = (p[0] + p[1] + p[2]) / 255.0 / 3;
      int category = floor(intensity / category_size);
      hist.at<double>(category, 1) += 1;  // Count this category
    }
  }
  return hist;
}

cv::Mat cv_calcHist(cv::Mat image, int bins) {
  cv::Mat intensity, hist;
  cvtColor(image, intensity, CV_RGB2GRAY);

  float histSize_f[2] = {0, 255};
  const float* histSizePtr = histSize_f;
  cv::calcHist(&intensity, 1, 0, cv::Mat(), hist, 1, &bins, &histSizePtr, true);

  cv::Mat enumHist(hist.size().height, 2, hist.type());  // hist matrix with bins labelled
  for (int i = 0; i < enumHist.size().height; ++i) {
    enumHist.at<float>(i, 0) = i * 255 / (bins-1);
    enumHist.at<float>(i, 1) = hist.at<float>(i, 1);
  }

  return enumHist;
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cout << "Usage: `A1_Q2 <data_directory>`" << std::endl
              << "  where data_directory contains subdirectories `images` and `data`" << std::endl;
  }
  std::string path = argv[1];

  cv::Mat image;
  image = cv::imread(path + "/images/cl2_inverted.png", cv::IMREAD_COLOR);

  cv::Mat my_hist = my_calcHist(image, 256);
  cv::Mat cv_hist = cv_calcHist(image, 256);

  std::ofstream out;
  out.open(path + "/outputs/my_histogram.csv");
  saveMatCsv<double>(my_hist, out);
  out.close();

  out.open(path + "/outputs/cv_histogram.csv");
  saveMatCsv<float>(cv_hist, out);
  out.close();
}

@ Now on to histograms (kind of)

@ Of course we want to include some of the output figures. The histogram can be seen in \ref{fig:hist}

\begin{figure}
  \centering

  \pgfplotsset{scaled y ticks=false}
  \begin{tikzpicture}\label{fig:hist}
    \begin{axis}[
        ybar,fill=blue,xtick distance=50,bar width=1,
        title={Manually Calculated Histogram},
        xlabel={Pixel Intensity (0 to 255)},
        ylabel={Count of Pixels with specified intensity}
      ]
      \addplot table [col sep=comma] {A1/outputs/my_histogram.csv};
    \end{axis}
  \end{tikzpicture}

  \begin{tikzpicture}\label{fig:hist}
    \begin{axis}[
        ybar,fill=blue,xtick distance=50,bar width=1,
        title={OpenCV Calculated Histogram},
        xlabel={Pixel Intensity (0 to 255)},
        ylabel={Count of Pixels with specified intensity}
      ]
      \addplot table [col sep=comma] {A1/outputs/cv_histogram.csv};
    \end{axis}
  \end{tikzpicture}
  \caption{Histogram of intensity}
\end{figure}

@ Lastly, we used several other standard libraries which must be included
<<Include>>=
#include <iostream>
#include <fstream>
#include <cmath>
@

\end{document}
