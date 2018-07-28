@ \section*{Part 2: Adaptive Thesholding}

For this section, we implement adaptive thresholding which finds the optimal threshold for segmenting a binary image.
The algorithm uses the midpoint of the means of each side of the current threshold to pick the next threshold and continues until the two thresholds are approximately equal.
The [[thresh_avg]] function performs the calculation of the upper and lower means for a certain threshold.

<<[[thresh_avg]] Function>>=
template <typename T>
std::pair<double, double> thresh_avg(const cv::Mat_<T> mat, T thresh) {
  double above_sum = 0, below_sum = 0;
  double above_cnt = 0, below_cnt = 0;
  for (int y = 0; y < mat.rows; ++y) {
    for (int x = 0; x < mat.cols; ++x) {
      T val = mat(y, x);
      if (val >= thresh) {
        above_sum += val;
        above_cnt++;
      } else {
        below_sum += val;
        below_cnt++;
      }
    }
  }
  double above_avg = (above_cnt > 0) ? above_sum / above_cnt
                                     : std::numeric_limits<T>::max();
  double below_avg = (below_cnt > 0) ? below_sum / below_cnt
                                     : std::numeric_limits<T>::min();
  return std::make_pair(above_avg, below_avg);
}

@ Then the [[adaptive_theshold]] perform the iteration of threshold updating and checking for convergence.

<<[[adaptive_theshold]] Function>>=
<<[[thresh_avg]] Function>>

template <typename T>
std::vector<T> adaptive_theshold(const cv::Mat_<T> mat) {
  const double tolerance = 0.01;
  std::vector<T> thresholds;
  thresholds.push_back(std::numeric_limits<T>::max() / 2);
  T prev;
  do {
    prev = thresholds.back();
    std::pair<double, double> means = thresh_avg(mat, prev);
    thresholds.push_back((means.first + means.second) / 2);
  } while (abs(thresholds.back() - prev) > tolerance);
  return thresholds;
}

@ Finally, the [[threshold]] function performs the thresholding operation for a specific threshold.
This function is provided for visualizing the results of the adaptive threshold operation.

<<[[threshold]] Function>>=
template <typename T>
cv::Mat_<T> threshold(cv::Mat_<T> mat, T threshold) {
  cv::Mat_<T> result(mat.size());
  for (int y = 0; y < mat.rows; ++y) {
    for (int x = 0; x < mat.cols; ++x) {
      result(y, x) = (mat(y, x) >= threshold) ? std::numeric_limits<T>::max()
                                              : std::numeric_limits<T>::min();
    }
  }
  return result;
}

@ \subsection*{Implementation of [[main]]}

In the [[main]] function, we use the [[adaptive_theshold]] operation on three images and record the intermediate threshold values.

<<Q2.cpp>>=
<<Include>>
<<Global constants>>
<<[[im_load]] Function>>
<<[[adaptive_theshold]] Function>>
<<[[threshold]] Function>>

int main(int argc, char* argv[]) {
  <<Command line args>>

  const int n_images = 3;
  cv::Mat_<uint8_t> images[n_images];
  bool read_success = true;
  read_success &= im_load<uint8_t>(path + "/images/for_thresh_1.png", &images[0],
                                   cv::IMREAD_GRAYSCALE);
  read_success &= im_load<uint8_t>(path + "/images/for_thresh_2.png", &images[1],
                                   cv::IMREAD_GRAYSCALE);
  read_success &= im_load<uint8_t>(path + "/images/for_thresh_3.png", &images[2],
                                   cv::IMREAD_GRAYSCALE);
  if (!read_success)
    return 1;

  std::vector< std::vector< uint8_t> > thresholds;
  int max_size = 0;
  for (int i = 0; i < n_images; ++i) {
    std::vector<uint8_t> thresh_iter = adaptive_theshold(images[i]);
    thresholds.push_back(thresh_iter);
    if (thresh_iter.size() > max_size)
      max_size = thresh_iter.size();
    cv::imwrite(path + "/output/thresh_" + std::to_string(i+1) + ".png",
                threshold(images[i], thresh_iter.back()));
  }

  std::ofstream thresh_csv;
  thresh_csv.open(path + "/output/thresholds.csv");
  for (int n = 0; n < thresholds.size(); ++n) {
    thresh_csv << n + 1;
    if (n != thresholds.size() - 1)
      thresh_csv << " ";
  }
  thresh_csv << std::endl;
  for (int i = 0; i < max_size; ++i) {
    for (int n = 0; n < thresholds.size(); ++n) {
      thresh_csv << ((i < thresholds.at(n).size()) ?
          std::to_string((int) thresholds.at(n).at(i)) : "{}");
      if (n != thresholds.size() - 1)
        thresh_csv << " ";
    }
    thresh_csv << std::endl;
  }
  thresh_csv.close();
}

@ \subsection*{Results}

The adaptive thresholding algorithm has been applied on three images with two obvious segments.
The threshold values through the iteration process are shown in Table~\ref{tab:thresh} where the last value in each row is the selected threshold.

\begin{table}
  \caption{Iteration of thresholds for images 1 through 3 in Figure~\ref{fig:thresh} \label{tab:thresh}}
  \begin{center}
  \pgfplotstabletypeset[
    every head row/.style = {before row=\toprule,after row=\midrule},
    every last row/.style = {after row=\bottomrule},
    header = has colnames
  ]{output/thresholds.csv}
  \end{center}
\end{table}

\begin{figure}[h]
  \begin{center}
  \begin{tabular}{cc}
    \subfloat[Image 1]{\includegraphics[width=0.4\textwidth]{images/for_thresh_1}} &
    \subfloat[Image 1 Thresholded]{\includegraphics[width=0.4\textwidth]{output/thresh_1}} \\
    \subfloat[Image 2]{\includegraphics[width=0.4\textwidth]{images/for_thresh_2}} &
    \subfloat[Image 2 Thresholded]{\includegraphics[width=0.4\textwidth]{output/thresh_2}} \\
    \subfloat[Image 3]{\includegraphics[width=0.4\textwidth]{images/for_thresh_3}} &
    \subfloat[Image 3 Thresholded]{\includegraphics[width=0.4\textwidth]{output/thresh_3}}
  \end{tabular}
  \end{center}
  \caption{Results of adaptive thresholding on three images with two distinct segments.\label{fig:thresh}}
\end{figure}
