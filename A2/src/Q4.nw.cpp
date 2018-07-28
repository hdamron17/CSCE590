@ \section*{Part 4: Correlation}

The goal of this section is to implement the correlation operator ($\circ$) as defined in Equation~\ref{eqn:correlation} where $I$ is the image, $T$ is the template image, and $M,N$ are height and width of $T$.

\begin{equation} \label{eqn:correlation}
  T(y, x) \circ I(y, x) = \dfrac{1}{MN} \sum_{m=0}^{M} \sum_{n=0}^{N} (T(m, n) \cdot I(y+m, x+n))
\end{equation}

\noindent The normalized version of the correlation operator is defined in Equation~\ref{eqn:norm_correlation}.

\begin{equation} \label{eqn:norm_correlation}
  T(y, x) \circ I(y, x) = \dfrac{1}{MN} \dfrac{\displaystyle \sum_{m=0}^{M} \sum_{n=0}^{N} (T(m, n) \cdot I(y+m, x+n))}{\displaystyle \sqrt{\sum_{m=0}^{M} \sum_{n=0}^{N} T(m, n)^2 \cdot \sum_{m=0}^{M} \sum_{n=0}^{N} I(y+m, x+n)^2}}
\end{equation}

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

@ \subsection*{Correlation Operator}

Next we implement the [[correlate]] function which allows both traditional and normalized correlation.
The implementation creates a padded image then loops over all pixel locations which are central enough for the full correlation template to be applied.
The pixel-wise operator follows Equation~\ref{eqn:correlation} for traditional correlation and Equation~\ref{eqn:norm_correlation} for normalized correlation.

<<[[correlate]] Function>>=
template<typename T_in, typename T_out, PadType pad_type=PadType::NONE>
cv::Mat_<T_out> correlate(const cv::Mat& mat, const cv::Mat& templ, bool normed=false) {
  cv::Size templ_size = templ.size();
  cv::Mat mat_padded = pad<pad_type>(mat, templ_size);

  int size_h = templ_size.height - ((templ_size.height + 1) % 2),
      size_w = templ_size.width - ((templ_size.width + 1) % 2);

  int shrink_h = (size_h - 1) / 2,
      shrink_w = (size_w - 1) / 2;

  cv::Mat_<T_out> correlated(mat_padded.size().height - 2 * shrink_h,
                             mat_padded.size().width - 2 * shrink_w);
  for (int i = 0; i < correlated.size().height; ++i) {
    for (int j = 0; j < correlated.size().width; ++j) {
      double sum = 0, im_sq_sum = 0, templ_sq_sum = 0;
      for (int di = 0; di < size_h; ++di) {
        for (int dj = 0; dj < size_w; ++dj) {
          T_in val = mat_padded.at<T_in>(i + di, j + dj);
          T_in tval = templ.at<T_in>(di, dj);
          sum += val * tval;
          if (normed) {
            im_sq_sum += val * val;
            templ_sq_sum += tval * tval;
          }
        }
      }
      if (normed)
        correlated(i,j) = sum / sqrt(im_sq_sum * templ_sq_sum);
      else
        correlated(i, j) = sum;
    }
  }
  correlated /= (size_h * size_w);
  return correlated;
}

@ \subsection*{Visualization}

To provide a better visualization for the [[correlate]] function, we provide functions which find the location of maximum intensity in the correlation image and draw a rectangle on the original image corresponding to that location of maximum correlation.

<<Annotation Functions>>=
void drawRect(cv::Mat* mat, const cv::Point& pt, const cv::Size& size) {
  cv::Point corner(pt.x - size.width / 2, pt.y - size.height / 2);
  cv::rectangle(*mat, cv::Rect(corner, size), 0);
  cv::drawMarker(*mat, pt, 0);
}

cv::Mat annotate_correlation(const cv::Mat& image, const cv::Mat& correlation,
                             const cv::Size& templ_size) {
  cv::Point max_loc;
  cv::minMaxLoc(correlation, nullptr, nullptr, nullptr, &max_loc);
  cv::Mat im_copy = image.clone();
  drawRect(&im_copy, max_loc, templ_size);
  return im_copy;
}

@ The [[in_range]] function is provided for visualization to scale the correlation image to span the whole range of its integer type.

<<[[in_range]] Function>>=
template<typename T>
cv::Mat in_range(const cv::Mat& image) {
  double min_p, max_p;
  cv::minMaxLoc(image, &min_p, &max_p);
  return (image - min_p) / (max_p - min_p) * std::numeric_limits<T>::max();
}

@ \subsection*{Implementation of [[main]]}

Finally, the [[main]] function is implemented to read several images and apply the correlation operator on them.
The OpenCV equivalent is provided to validate each operation, but the results are not included in this report.

<<Q4.cpp>>=
<<Include>>
<<Global constants>>
<<[[pad]] Function>>
<<[[correlate]] Function>>
<<Annotation Functions>>
<<[[in_range]] Function>>

int main(int argc, char* argv[]) {
  <<Command line args>>

  std::string im_path = path + "/images/for_correlation_small.jpg",
              templ_path = path + "/images/for_correlation_copy_template_small.jpg";
  cv::Mat image = cv::imread(im_path, 0);
  if(!image.data) {
    std::cout << "Failed to read image " << im_path << std::endl;
    return 1;
  }

  cv::Mat templ = cv::imread(templ_path, 0);
  if(!templ.data) {
    std::cout << "Failed to read template image " << templ_path << std::endl;
    return 1;
  }

  cv::Mat correlation
      = correlate<uint8_t, float, PadType::ZEROS>(image, templ);
  cv::Mat correlation_normed
      = correlate<uint8_t, float, PadType::ZEROS>(image, templ, true);

  cv::Mat correlation_display
      = in_range<uint8_t>(correlation);
  cv::Mat correlation_annotated
      = annotate_correlation(image, correlation, templ.size());

  cv::Mat correlation_normed_display
      = in_range<uint8_t>(correlation_normed);
  cv::Mat correlation_normed_annotated
      = annotate_correlation(image, correlation_normed, templ.size());

  cv::imwrite(path + "/output/correlation.png",
              correlation_display, PNG_COMPRESSION);
  cv::imwrite(path + "/output/correlation_annotated.png",
              correlation_annotated, PNG_COMPRESSION);

  cv::imwrite(path + "/output/correlation_normed.png",
              correlation_normed_display, PNG_COMPRESSION);
  cv::imwrite(path + "/output/correlation_normed_annotated.png",
              correlation_normed_annotated, PNG_COMPRESSION);

  /* Comparable OpenCV operations provided for validation */
  cv::Mat cv_correlation, cv_correlation_normed;
  cv::matchTemplate(pad<PadType::ZEROS>(image, templ.size()), templ,
                                        cv_correlation, CV_TM_CCORR);
  cv::matchTemplate(pad<PadType::ZEROS>(image, templ.size()), templ,
                                        cv_correlation_normed, CV_TM_CCORR_NORMED);

  cv::Mat cv_correlation_display
      = in_range<uint8_t>(cv_correlation);
  cv::Mat cv_correlation_annotated
      = annotate_correlation(image, cv_correlation, templ.size());

  cv::Mat cv_correlation_normed_display
      = in_range<uint8_t>(cv_correlation_normed);
  cv::Mat cv_correlation_normed_annotated
      = annotate_correlation(image, cv_correlation_normed, templ.size());

  cv::imwrite(path + "/output/cv_correlation.png",
              cv_correlation_display, PNG_COMPRESSION);
  cv::imwrite(path + "/output/cv_correlation_annotated.png",
              cv_correlation_annotated, PNG_COMPRESSION);

  cv::imwrite(path + "/output/cv_correlation_normed.png",
              cv_correlation_normed_display, PNG_COMPRESSION);
  cv::imwrite(path + "/output/cv_correlation_normed_annotated.png",
              cv_correlation_normed_annotated, PNG_COMPRESSION);

  if (display) {
    cv::imshow("Correlation", correlation_display);
    cv::waitKey(0);

    cv::imshow("Correlation Annotated", correlation_display);
    cv::waitKey(0);

    cv::imshow("Normalized Correlation", correlation_display);
    cv::waitKey(0);

    cv::imshow("Normalized Correlation Annotated", correlation_display);
    cv::waitKey(0);
  }
}

@ \subsection*{Results}

The results of the correlation operator can be seen in Figure~\ref{fig:correlation}.

\newlength{\correlationheight}
\setlength{\correlationheight}{\heightof{\includegraphics[width=0.33\textwidth]{images/for_correlation_small}}}

\begin{figure}[!ht]
  \begin{tabular}{ccc}
    \subfloat[\label{subfig:correlation_image}]{\includegraphics[width=0.3\textwidth]{images/for_correlation_small}} &
    \subfloat[\label{subfig:correlation}]{\includegraphics[width=0.3\textwidth]{output/correlation}} &
    \subfloat[\label{subfig:correlation_norm}]{\includegraphics[width=0.3\textwidth]{output/correlation_annotated}} \\
    \subfloat[\label{subfig:correlation_kernel}]{\includegraphics[width=0.3\textwidth,height=\correlationheight,keepaspectratio]{images/for_correlation_copy_template_small}} &
    \subfloat[\label{subfig:correlation_annotated}]{\includegraphics[width=0.3\textwidth]{output/correlation_normed}} &
    \subfloat[\label{subfig:correlation_norm_annotated}]{\includegraphics[width=0.3\textwidth]{output/correlation_normed_annotated}} \\
  \end{tabular}

  \caption{Illustration of correlation process.
    \protect\subref{subfig:correlation_image} Original greyscale image.
    \protect\subref{subfig:correlation} Result of normal correlation using zero padding.
    \protect\subref{subfig:correlation_annotated} Annotation of region with highest correlation.
    \protect\subref{subfig:correlation_kernel} Template used in correlation, enlarged.
    \protect\subref{subfig:correlation_norm} Result of normalized correlation using zero padding.
    \protect\subref{subfig:correlation_norm_annotated} Annotation of region with highest normalized correlation.
  }
  \label{fig:correlation}
\end{figure}
