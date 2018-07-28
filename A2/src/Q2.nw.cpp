@ \section*{Part 2: Arithmetic Operations}

For this section, we use image arithmetic operations to produce a resultant image.
The specific use of image addition and subtraction in this implementation is to superimpose a visualization of wind patterns in the United States onto a political map for better visualization.

@ \subsection*{Arithmetic Operation Implementations}

To produce this result, we start by defining functions for the arithmetic operations, beginning with [[op]] and [[scalar_op]] which apply arbitrary functions on image matrices.
These will be used to more easily produce the desired operations.

<<Convenience Functions>>=
template<typename T, typename OP>
cv::Mat_<T> op(cv::Mat_<T> m1, cv::Mat_<T> m2, OP operation) {
  cv::Size size(std::min(m1.size().width, m2.size().width),
                std::min(m1.size().height, m2.size().height));
  cv::Mat_<T> out(size);
  for (int i = 0; i < size.height; ++i) {
    for (int j = 0; j < size.width; ++j) {
      out(i, j) = operation(m1(i,j), m2(i,j));
    }
  }
  return out;
}

template<typename T, typename S, typename OP>
cv::Mat_<T> scalar_op(cv::Mat_<T> m, S s, OP operation) {
  cv::Size size = m.size();
  cv::Mat_<T> out(size);
  for (int i = 0; i < size.height; ++i) {
    for (int j = 0; j < size.width; ++j) {
      out(i, j) = operation(m(i,j), s);
    }
  }
  return out;
}

@ The provided arithmetic operations are as the following:
\begin{itemize}
  \item [[add]] performs element-wise addition of two matrices
  \item [[sub]] performs element-wise subtraction of two matrices where negative results are casted to 0
  \item [[scalar_div]] performs division of all elements in a matrix by a scalar
  \item [[normed_add]] which perform [[add]] then remaps the resultant matrix to have the same minimum and maximum values as the original matrix
\end{itemize}

<<Arithmetic Ops>>=
template<typename T>
cv::Mat_<T> add(cv::Mat_<T> m1, cv::Mat_<T> m2) {
  return op(m1, m2, [](const T& p1, const T& p2) { return p1 + p2; });
}

template<typename T>
cv::Mat_<T> sub(cv::Mat_<T> m1, cv::Mat_<T> m2) {
  return op(m1, m2, [](const T& p1, const T& p2) { return (p1 > p2) ? p1 - p2 : 0; });
}

template<typename T, typename S>
cv::Mat_<T> scalar_div(cv::Mat_<T> m, S s) {
  return scalar_op(m, s, [](const T& p1, const S& p2) { return p1 / p2; });
}

template<typename T>
cv::Mat_<T> normed_add(cv::Mat_<T> m1, cv::Mat_<T> m2) {
  double min_p, max_p;
  cv::minMaxLoc(m1, &min_p, &max_p);
  cv::Mat_<T> added = add(m1, m2);
  double new_min, new_max;
  cv::minMaxLoc(added, &new_min, &new_max);
  return (added + (min_p - new_min)) * (max_p - min_p) / (new_max - new_min);
}

@ The final function we must define is [[zero_pad]] which creates a zero matrix of a specific size and inserts another image into that zero matrix at a specific location.

<<[[zero_pad]] Function>>=
template<typename T>
cv::Mat_<T> zero_pad(cv::Mat_<T> im, cv::Point ul_corner, cv::Size size) {
  cv::Mat_<T> out = cv::Mat_<T>::zeros(size);
  int new_w = std::min(im.size().width, ul_corner.x + size.width),
      new_h = std::min(im.size().height, ul_corner.y + size.height);
  cv::Size cp_size(new_w, new_h);
  cv::Rect loc(ul_corner, cp_size);
  im(cv::Rect(cv::Point(0, 0), cp_size)).copyTo(out(loc));
  return out;
}

@ \subsection*{Implementation of [[main]]}

The resulting procedure is fairly simple. The defined arithmetic operations are used to produce the desired image.
Because the windmap image is slightly smaller than the political map, the [[zero_pad]] function is used to place it at location $(15, 15)$.

<<Q2.cpp>>=
<<Include>>
<<Global constants>>
<<[[im_load]] Function>>

<<Convenience Functions>>
<<Arithmetic Ops>>
<<[[zero_pad]] Function>>

int main(int argc, char* argv[]) {
  <<Command line args>>

  cv::Mat_<cv::Vec3w> america, windmap, result;
  cv::Mat_<cv::uint16_t> windmap_orig, america_mask, windmap_g;
  bool loaded = true;
  loaded &= im_load(path + "/images/america.png", &america, 1);
  loaded &= im_load(path + "/images/windmap.jpg", &windmap_orig, 0);
  loaded &= im_load(path + "/images/america_mask.png", &america_mask, 0);
  if (!loaded)
    return 1;

  windmap_g = sub(windmap_orig, america_mask);
  cv::cvtColor(windmap_g, windmap, cv::COLOR_GRAY2RGB);
  windmap = zero_pad(windmap, cv::Point(15, 15), america.size());
  windmap = scalar_div(windmap, 2);

  result = normed_add(america, windmap);
  cv::Mat_<cv::Vec3b> result_8;
  result.convertTo(result_8, cv::traits::Type<cv::Vec3b>::value);

  imwrite(path + "/output/final_windmap.png", result_8, PNG_COMPRESSION);

  if (display) {
    cv::imshow("America Windmap", result_8);
    cv::waitKey(0);
  }
}

@ \subsection*{Results}

The image pipeline and the output image is illustrated in Figure~\ref{fig:q2_images}.

\begin{figure}[!ht]
  \begin{equation*}
      \begin{array}{c}
        \frame{\includegraphics[width=0.18\textwidth]{images/america}}
      \end{array}
    + \dfrac{1}{2} \times
      \left(
      \begin{array}{c}
       \frame{\includegraphics[width=0.18\textwidth]{images/windmap}}
      \end{array}
    - \begin{array}{c}
        \frame{\includegraphics[width=0.18\textwidth]{images/america_mask}}
      \end{array}
      \right)
    = \begin{array}{c}
        \frame{\includegraphics[width=0.18\textwidth]{output/final_windmap}}
      \end{array}
  \end{equation*}
  \caption{Description of arithmetic operations applied on images to produce windmap of the United States.}
  \label{fig:q2_images}
\end{figure}
