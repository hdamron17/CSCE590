@ \section*{Part 1: Convolution}

In this part of the assignment, the goal is to implement the convolution operation and apply it on a variety of images.
We begin by defining several kernels we will apply using convolution.

@ \subsection*{Kernel Definitions}

@ The first kernel, we define is the average kernel which simply averages each pixel with its neighborhood.

<<Kernel Definitions>>=
const cv::Mat_<double> makeAverageKernel(const int n) {
  return cv::Mat_<double>(n, n, 1.0 / (n * n));
}

@ As specified in the documentation of OpenCV, we can create a Gaussian blur kernel with
\[G_{y,x} = \alpha \cdot \exp\left\{-\frac{\left(y - \frac{n+1}{2}\right)^2 + \left(x - \frac{n+1}{2}\right)^2}{2\sigma^2}\right\}\]
where $n$ is the size of the kernel, $y,x \in \{0 \dots n-1\}$, and $\alpha$ is chosen to normalize the kernel such that $\sum_{y=0}^{n-1} \sum_{x=0}^{n-1} G_{y,x} = 1$.
Note that $n$ should be odd and that the default standard deviation is $\sigma = 0.3 \cdot \left(\frac{n-1}{2} - 1\right) + 0.8$.

<<Kernel Definitions>>=
const cv::Mat_<double> makeGaussianKernel(const int n, double sigma=-1) {
  cv::Mat_<double> kernel(n, n);
  if (sigma < 0) {
    sigma = 0.3*((n-1) * 0.5 - 1) + 0.8;
  }

  int middle = (n-1) / 2;
  double G_sum = 0;
  for (int y = 0; y < n; ++y) {
    for (int x = 0; x < n; ++x) {
      int y_dev = y - middle,
          x_dev = x - middle;
      double G_yx = exp(-(y_dev * y_dev + x_dev * x_dev) / (2 * sigma * sigma));
      kernel(y, x) = G_yx;
      G_sum += G_yx;
    }
  }
  kernel /= G_sum;  // Normalize kernel
  return kernel;
}

@ We now create the vertical edge kernel with columns $C_i = -\frac{n-1}{2} \dots \frac{n-1}{2}$.
The horizontal edge kernel follows the same pattern for rows.

<<Kernel Definitions>>=
const cv::Mat_<double> makeVEdgeKernel(const int n) {
  cv::Mat_<double> kernel(n, n);
  int half = n / 2;
  for (int i = 0; i < n; ++i) {
    kernel.col(i) = i - half;
  }
  return kernel;
}

const cv::Mat_<double> makeHEdgeKernel(const int n) {
  cv::Mat_<double> kernel(n, n);
  int half = n / 2;
  for (int i = 0; i < n; ++i) {
    kernel.row(i) = i - half;
  }
  return kernel;
}

@ For the sharpening kernel, I did not know a way to generalize to an arbitrary sized square, so this implementation only provides a 3 by 3 sharpening kernel:
\[\begin{bmatrix}
  -1 & -1 & -1 \\
  -1 &  9 & -1 \\
  -1 & -1 & -1
\end{bmatrix}\]

<<Kernel Definitions>>=
const cv::Mat_<double> makeSharpenKernel3x3() {
  int n = 3;
  cv::Mat_<double> kernel(n, n, -1);
  int half = (n-1) / 2;
  kernel(half, half) = n * n;
  return kernel;
}

@ Lastly, for playing around, we use a custom kernel:
\[\begin{bmatrix}
   4 &  2 & -2 \\
   2 &  3 & -2 \\
  -2 & -2 & -2
\end{bmatrix}\]

<<Kernel Definitions>>=
const int kCustomN = 3;
double kCustomKernel[kCustomN*kCustomN] = {
   4,  2, -2,
   2,  3, -2,
  -2, -2, -2
};
const cv::Mat_<double> makeCustomKernel3x3() {
  return cv::Mat_<double>(kCustomN, kCustomN, kCustomKernel);
}

@ \subsection*{Padding}
We begin by providing the [[pad]] function which allows various types of image padding.
This implementation only uses zero padding and truncation, but the enumeration [[PadType]] would make it easy to provide alternative padding schemes.

<<[[pad]] Function>>=
enum PadType {
  NONE = 0,
  ZEROS = 1,
  REPEAT_BOUNDARY = 2,
  REPEAT_SEQUENCE = 3
};

template<PadType pad_type>
cv::Mat pad(const cv::Mat& mat, cv::Size size) {
  pad<pad_type>(mat, size);
}

template<>
cv::Mat pad<PadType::NONE>(const cv::Mat& mat, cv::Size size) {
  return mat;
}

template<>
cv::Mat pad<PadType::ZEROS>(const cv::Mat& mat, cv::Size size) {
  int padding_h = (size.height - 1) / 2,
      padding_w = (size.width - 1) / 2;
  cv::Mat padded = cv::Mat::zeros(mat.size().height + 2 * padding_h,
                                  mat.size().width + 2 * padding_w,
                                  mat.type());
  mat.copyTo(padded(cv::Rect(cv::Point(padding_h, padding_w), mat.size())));
  return padded;
}

@ \subsection*{Convolution operator ($\ast$)}

The correlation operationg is defined in Equation~\ref{eqn:convolution} and implemented in the [[conv]] function.
It calculates the dot product of the neighborhood around each pixel and the kernel, applying the resulting sum to the corresponding pixel in the output.

\begin{equation} \label{eqn:convolution}
  T(y, x) \ast I(y, x) = \sum_{m=0}^{M} \sum_{n=0}^{N} (T(m, n) \cdot I(y-m, x-n))
\end{equation}

<<[[conv]] Function>>=
<<[[pad]] Function>>
template <typename T, typename K, PadType pad_type=PadType::ZEROS>
cv::Mat_<T> conv(const cv::Mat_<T> mat, const cv::Mat_<K> kernel) {
  if (kernel.rows % 2 == 0 || kernel.cols % 2 == 0) {
    std::cerr << "ERROR: kernel dimensions must be odd for `conv` function" << std::endl;
    return cv::Mat_<T>();
  }
  cv::Mat_<T> mat_padded = pad<pad_type>(mat, kernel.size());

  int size_h = kernel.rows - ((kernel.rows + 1) % 2),
      size_w = kernel.cols - ((kernel.cols + 1) % 2);

  int M = kernel.rows,
      N = kernel.cols;

  int shrink_h = (size_h - 1) / 2,
      shrink_w = (size_w - 1) / 2;

  cv::Mat_<T> convolved(mat_padded.size().height - 2 * shrink_h,
                        mat_padded.size().width  - 2 * shrink_w);
  for (int y = 0; y < convolved.rows; ++y) {
    for (int x = 0; x < convolved.cols; ++x) {
      double sum = 0;
      for (int dy = 0; dy < kernel.rows; ++dy) {
        for (int dx = 0; dx < kernel.cols; ++dx) {
          sum += kernel(dy, dx) * mat_padded(y - dy + (M-1), x - dx + (N-1));
        }
      }
      K k_min = std::numeric_limits<K>::min(),
        k_max = std::numeric_limits<K>::max();
      if (sum < k_min)
        convolved(y, x) = k_min;
      else if (sum > k_max)
        convolved(y, x) = k_max;
      else
        convolved(y, x) = sum;
    }
  }

  return convolved;
}

@ \subsection*{Implementation of [[main]]}

The [[main]] function simply uses the [[conv]] function on two images with various kernels.
The OpenCV alternatives are included also for comparison, but they perform correlation rather than convolution.

<<Q1.cpp>>=
<<Include>>
<<Global constants>>
<<[[im_load]] Function>>
<<[[conv]] Function>>

<<Kernel Definitions>>

int main(int argc, char* argv[]) {
  <<Command line args>>

  const int n_images = 2;
  cv::Mat_<uint8_t> images[n_images];
  bool read_success = true;
  read_success &= im_load<uint8_t>(path + "/images/for_conv_1.png", &images[0],
                                   cv::IMREAD_GRAYSCALE);
  read_success &= im_load<uint8_t>(path + "/images/for_conv_2.png", &images[1],
                                   cv::IMREAD_GRAYSCALE);
  if (!read_success)
    return 1;

  std::vector<std::string> save_names = {"avg", "gauss", "vedge",
                                         "hedge", "sharp", "custom"};
  std::vector<cv::Mat_<double> > kernels = {
      makeAverageKernel(15), makeGaussianKernel(15), makeVEdgeKernel(3),
      makeHEdgeKernel(3), makeSharpenKernel3x3(), makeCustomKernel3x3()
  };

  for (int im = 0; im < n_images; ++im) {
    for (int i = 0; i < kernels.size(); ++i) {
      cv::Mat_<uint8_t> conved = conv(images[im], kernels[i]);
      cv::Mat_<uint8_t> cv_conved;
      cv::filter2D(images[im], cv_conved, -1, kernels[i]);
      cv::imwrite(path + "/output/conv_" + save_names[i] + "_"
                  + std::to_string(im+1) + ".png", conved, PNG_COMPRESSION);
      cv::imwrite(path + "/output/cv_conv_" + save_names[i] + "_"
                  + std::to_string(im+1) + ".png", cv_conved, PNG_COMPRESSION);
    }
  }
}

@ \subsection*{Results}

The results of the convolutions can be seen in the following table of images.

\begin{longtable}{|c|c|c|}
  \hline
  \multicolumn{3}{|c|}{Results of Convolution on Two Images}
  \endfirsthead
  \hline\endhead
  \hline\endfoot
  \hline\endlastfoot
  \hline
  Original  &
    \includegraphics[width=0.3\textwidth]{images/for_conv_1} &
    \includegraphics[width=0.3\textwidth]{images/for_conv_2} \\ \hline
  Average  &
    \includegraphics[width=0.3\textwidth]{output/conv_avg_1} &
    \includegraphics[width=0.3\textwidth]{output/conv_avg_2} \\ \hline
  Gaussian &
    \includegraphics[width=0.3\textwidth]{output/conv_gauss_1} &
    \includegraphics[width=0.3\textwidth]{output/conv_gauss_2} \\ \hline
  Vertical Edge &
    \includegraphics[width=0.3\textwidth]{output/conv_vedge_1} &
    \includegraphics[width=0.3\textwidth]{output/conv_vedge_2} \\ \hline
  horizontal Edge &
    \includegraphics[width=0.3\textwidth]{output/conv_hedge_1} &
    \includegraphics[width=0.3\textwidth]{output/conv_hedge_2} \\ \hline
  Sharpening &
    \includegraphics[width=0.3\textwidth]{output/conv_sharp_1} &
    \includegraphics[width=0.3\textwidth]{output/conv_sharp_2} \\ \hline
  Custom Kernel &
    \includegraphics[width=0.3\textwidth]{output/conv_custom_1} &
    \includegraphics[width=0.3\textwidth]{output/conv_custom_2} \\ \hline
\end{longtable}
