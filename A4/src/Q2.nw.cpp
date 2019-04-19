@ \section*{Part 2: Hough Transform}

In part 2, we use the Hough transform to detect the longest linear edges in an image.

We begin by using the Sobel detection mechanism built into OpenCV and a threshold to get a binary image of candidate edges.

<<[[edge_detect]] Function>>=
template <typename T>
cv::Mat_<T> edge_detect(const cv::Mat_<T>& I, const double thresh=0.7) {
  cv::Mat_<double> dx, dy, edges_db;
  cv::Mat_<T> edges;
  cv::Sobel(I, dx, cv::DataType<double>::type, 1, 0);
  cv::Sobel(I, dy, cv::DataType<double>::type, 0, 1);
  cv::magnitude(dx, dy, edges_db);
  edges_db.convertTo(edges, cv::DataType<T>::type);
  double min_v, max_v;
  cv::minMaxLoc(edges, &min_v, &max_v);
  cv::threshold(edges, edges, min_v + thresh * (max_v - min_v),
                std::numeric_limits<T>::max(), cv::THRESH_BINARY);
  return edges;
}

@ Next, we provide the [[hough_transform]] function. In this function, we create a 2D matrix $P$ with indices $\theta,\rho$ both centered around 0.
For each pixel $I(y,x) > 0$ where $I$ is the original image, we increment each pixel $P(\rho,\theta)$ in the the sinusoidal waveform defined by $x\cos\theta + y\sin\theta = \rho$ for $\theta \in \left[-\pi,\pi\right]$.
The resulting matrix $P$ is returned.

<<[[hough_transform]] Function>>=
template <typename T_out, typename T_in>
cv::Mat_<T_out> hough_transform(const cv::Mat_<T_in>& E, const int theta_bins=600,
                                const int rho_bins=600, double* max_rho_ptr=nullptr) {
  const int max_rho = std::max(E.rows, E.cols) * 1.05;
  const double max_theta = M_PI/2;

  const double d_theta = 2.0 * max_theta / theta_bins,
               d_rho = 2.0 * max_rho / rho_bins;

  if (max_rho_ptr)
    *max_rho_ptr = max_rho;

  cv::Mat_<T_out> parametric = cv::Mat_<T_out>::zeros(rho_bins, theta_bins);
  for (int y = 0; y < E.rows; ++y) {
    for (int x = 0; x < E.cols; ++x) {
      T_in val = E(y, x);
      if (val > 0) {
        for (int theta_i = 0; theta_i < parametric.cols; ++theta_i) {
          double theta = d_theta * theta_i - max_theta;
          double rho = x * cos(theta) + y * sin(theta);
          int rho_i = (rho + max_rho) / d_rho;

          if (rho_i < 0 || rho_i >= parametric.rows) {
            // std::cout << "WARN: rho = " << rho_i << " " << rho
            //           << " out of bounds [0, " << parametric.rows << ")"
            //           << std::endl;
            rho_i = parametric.rows-1;
            continue;
          }

          ++parametric(rho_i, theta_i);
        }
      }
    }
  }

  return parametric;
}

@ In the [[hough_lines]] function, we operate on the result of the [[hough_transform]] function to find strongest lines.
The first step is to identify the $n$ most intense pixels $P(i_\rho, i_\theta)$ where $P$ is the input matrix.
For each $(i_\rho, i_\theta)$, we convert back to the original ranges of $\rho$ and $\theta$, thus returning the pairs
\[\left( \frac{2 i_\rho \rho_{max}}{N_\rho} - \rho_{max}, \frac{2 i_\theta \theta_{max}}{N_\theta} - \theta_{max} \right).\]

<<[[hough_lines]] Function>>=
template <typename T_in>
std::vector<cv::Point_<double> > hough_lines(const cv::Mat_<T_in>& hough,
                      const double max_rho, const int n,
                      std::vector<cv::Point_<T_in> >* point_indices=nullptr) {
  const double max_theta = M_PI/2;
  std::vector<std::pair<cv::Point_<T_in>, T_in> > max_vals;
  for (int rho_i = 0; rho_i < hough.rows; ++rho_i) {
    for (int theta_i = 0; theta_i < hough.cols; ++theta_i) {
      T_in v = hough(rho_i, theta_i);
      int i;
      for (i = 0; i < max_vals.size() && v > max_vals[i].second; ++i) {}
      if (i > 0 || max_vals.size() == 0) {
        max_vals.insert(max_vals.begin() + i,
                        std::make_pair(cv::Point_<double>(theta_i, rho_i), v));
        if (max_vals.size() > n)
          max_vals.erase(max_vals.begin());
      }
    }
  }

  if (point_indices)
    point_indices->resize(max_vals.size());

  std::vector<cv::Point_<double> > lines(max_vals.size());
  for (int i = 0; i < max_vals.size(); ++i) {
    std::pair<cv::Point_<T_in>, T_in> line = max_vals[i];
    cv::Point_<T_in> loc = line.first;
    double theta = loc.x * 2 * max_theta / hough.cols - max_theta;
    double rho = loc.y * 2 * max_rho / hough.rows - max_rho;
    lines[i] = cv::Point_<double>(theta, rho);

    (*point_indices)[i] = loc;
  }
  return lines;
}

@ For using the resulting lines more easily, the functions [[yline]] is provided to convert the polar form
$x\cos\theta + y\sin\theta = \rho$
into the slope-intercept form
$y = -x\cot\theta + \rho\csc\theta$.
[[xline]] provides the equivalent convertion when the axes are interted.
This is beneficial to avoid division by zero in the case of vertical lines.

<<[[yline]] and [[xline]] Functions>>=
cv::Point_<double> yline(const cv::Point_<double>& polar_p) {
  double sin_theta = sin(polar_p.x),
         m = -cos(polar_p.x) / sin_theta,
         b = polar_p.y / sin_theta;
  return cv::Point_<double>(m, b);
}

cv::Point_<double> xline(const cv::Point_<double>& polar_p) {
  double cos_theta = cos(polar_p.x),
         m = -sin(polar_p.x) / cos_theta,
         b = polar_p.y / cos_theta;
  return cv::Point_<double>(m, b);
}

@ In the [[draw_line]] function, we calculate the longest segment of a line which is contained in the nonzero pixels of a binary mask.
A small separation is allowed between pixels, allowing for more cohesive segments.
Lines are calculated either by [[xline]] if $\left|\sin\theta\right| \leq \frac{1}{\sqrt{2}}$ else by [[yline]].

<<[[draw_line]] Function>>=
template <typename T_out, typename T_in>
void draw_line(cv::Mat_<cv::Vec<T_out, 3> >* im, const cv::Mat_<T_in>& mask,
               cv::Point_<double> polar_coords, const int connect_thresh=4) {
  double theta = polar_coords.x,
         rho = polar_coords.y;
  bool xy = abs(sin(theta)) <= 1/sqrt(2);  // To fairly allocate

  cv::Mat_<T_in> this_mask;
  if (xy) {
    cv::transpose(*im, *im);
    cv::transpose(mask, this_mask);
  } else {
    this_mask = mask;
  }

  cv::Point_<double> mb = (xy) ? xline(polar_coords) : yline(polar_coords);
  std::vector<int>  yvals(im->cols);
  std::vector<bool> yvalids(im->cols);
  for (int x = 0; x < yvals.size(); ++x) {
    int y = mb.x * x + mb.y;
    yvals[x] = y;
    yvalids[x] = y >= 0 && y < im->rows && mask(y,x) > 0;
  }

  int longest_start = 0, current_start = 0, longest = 0, recent = yvals.size();
  for (int x = 0; x < yvals.size(); ++x) {
    int valid = yvalids[x];
    int length = x - current_start;
    if (valid) {
      if (x - recent <= connect_thresh) {
        recent = x;
        if (length > longest) {
          longest = length;
          longest_start = current_start;
        }
      }
    } else {
      current_start = x;
    }
  }

  int x0 = longest_start,
      x1 = longest_start + longest;
  cv::line(*im, cv::Point(x0, yvals[x0]), cv::Point(x1, yvals[x1]),
           cv::Scalar(0, 0, std::numeric_limits<T_out>::max()), 2);

  if (xy)
    cv::transpose(*im, *im);
}

@ \subsection*{Implementation of [[main]]}

The implementation of the [[main]] function consists simply of loading the image and applying the hough transform and longest edge annotation on it.
Annotations are also drawn on the hough transform graph to show locations of highest intensity.

<<Q2.cpp>>=
<<Include>>
<<Global constants>>
<<[[im_load]] Function>>
<<[[in_range]] Function>>

<<[[edge_detect]] Function>>
<<[[hough_transform]] Function>>
<<[[hough_lines]] Function>>
<<[[yline]] and [[xline]] Functions>>
<<[[draw_line]] Function>>

int main(int argc, char* argv[]) {
  <<Command line args>>

  cv::Mat_<uint8_t> im;
  bool read_success = im_load<uint8_t>(path + "/images/for_hough.jpg", &im,
                                       cv::IMREAD_GRAYSCALE);
  if (!read_success)
    return 1;

  cv::Mat_<uint8_t> edges = edge_detect(im);

  double max_rho;
  cv::Mat_<uint16_t> hough = hough_transform<uint16_t>(edges, 600, 600, &max_rho);

  cv::Mat_<cv::Vec3b> hough_display, im_display;
  cv::Mat_<uint8_t> hough_8;
  hough.convertTo(hough_8, cv::DataType<uint8_t>::type);
  cv::cvtColor(in_range(hough_8), hough_display, cv::COLOR_GRAY2BGR);
  cv::cvtColor(im, im_display, cv::COLOR_GRAY2BGR);

  std::vector<cv::Point_<uint16_t> > hough_indices;
  std::vector<cv::Point_<double> > hough_vec = hough_lines(hough, max_rho, 2,
                                                           &hough_indices);
  for (int i = 0; i < hough_vec.size(); ++i) {
    cv::Point_<double> line_polar = hough_vec[i];
    cv::Point_<uint16_t> indices = hough_indices[i];

    cv::circle(hough_display, indices, 5, cv::Scalar(0,0,255), 2);
    draw_line(&im_display, edges, line_polar, 10);
  }

  cv::imwrite(path + "/output/hough_edges.png", edges, PNG_COMPRESSION);
  cv::imwrite(path + "/output/hough.png", hough_display, PNG_COMPRESSION);
  cv::imwrite(path + "/output/hough_annotated.png", im_display, PNG_COMPRESSION);

  if (display) {
    cv::imshow("Edges by Sobel Detector", edges);
    cv::waitKey(0);
    cv::imshow("Hough Transform", hough_display);
    cv::waitKey(0);
    cv::imshow("Image with Longest Edges Annotated", im_display);
    cv::waitKey(0);
  }
}

@ \subsection*{Results}

The method described above was applied on an image with two very strong edges. The results of the method are shown in Figure~\ref{fig:hough}.

\begin{figure}[!ht]
  \begin{center}
  \begin{tabular}{cc}
    \subfloat[\label{subfig:hough_orig}]{\includegraphics[width=0.45\textwidth]{images/for_hough}} &
    \subfloat[\label{subfig:hough_edges}]{\includegraphics[width=0.45\textwidth]{output/hough_edges}} \\
    \subfloat[\label{subfig:hough}]{\includegraphics[width=0.45\textwidth]{output/hough}} &
    \subfloat[\label{subfig:hough_annotated}]{\includegraphics[width=0.45\textwidth]{output/hough_annotated}}
  \end{tabular}
  \end{center}
  \caption{Hough transform and longest edge detection applied on image with obvious edges.
    \protect\subref{subfig:hough_orig} Original image.
    \protect\subref{subfig:hough_edges} Edge detection by threshold on the magnitude of x and y Sobel derivatives.
    \protect\subref{subfig:hough} Results of Hough transform showing maximum intensity locations in red.
    \protect\subref{subfig:hough_annotated} Original image with longest edges annotated in red.
  }
  \label{fig:hough}
\end{figure}
