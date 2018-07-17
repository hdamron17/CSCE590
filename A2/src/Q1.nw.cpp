@ \section*{Part 1: Medial Axes}

The goal of this section is to implement the grassfire transform and use it to mark the skeleton of a binary image. The grassfire transform is defined in Algorithm \ref{alg:grassfire}.

\begin{algorithm}
\caption{Grassfire transform} \label{alg:grassfire}
\begin{algorithmic}
\Procedure{Grassfire}{$I$} \Comment{Grassfire transform of image matrix $I$}
  \State Define $D \textrm{ where } D_{y,x} \gets
    \begin{cases}
      0                & \textrm{if } I_{y,x} = 0   \\
      1                & \textrm{if } I_{y,x} > 0 \land q=0 ~\exists~ q \in N_8(I_{y,x}) \\
      \textrm{MAX INT} & \textrm{otherwise}
    \end{cases}$
    \Comment{Create distances matrix $D$}

  \State Define $B \gets \left\{ I_{y,x} \mid D_{y,x} > 1 \land D_{y',x'} = 1 ~\exists~ I_{y',x'} \in N_8(I_{y,x}) \right\}$
    \Comment{Create boundary set $B$}

  \State Define $n \gets 2$
    \Comment{Create distances counter starting from 2}

  \While{$B \neq \emptyset$}
    \For{$I_{y,x} \textrm{ in } B$}
      \State $D_{y,x} \gets n ~\forall~ I_{y,x} \in B$
        \Comment{Update distances of current boundary}
      \State $n \gets n + 1$
        \Comment{Increment distances counter}
      \State $B' \gets B \cup \left\{ I_{y',x'} \mid I_{y',x'} \in N_8(I_{y,x}) \land D_{y',x'} > D_{y,x} \right\}$
        \Comment{Create new boundary $B'$}
    \EndFor
  \EndWhile
  \State \Return{$D$}
\EndProcedure
\end{algorithmic}
\end{algorithm}

@ We begin by defining several convenience functions to be used in the grassfire and skeleton procedures.

<<Convenience Functions>>=
template <typename T>
bool neighbors_le(const int& y, const int& x, const cv::Mat& im, const int& cmp) {
  cv::Size size = im.size();
  for (int i = std::max(0, y-1); i <= std::min(y+1, size.height-1); ++i) {
    for (int j = std::max(0, x-1); j <= std::min(x+1, size.width-1); ++j) {
      if (im.at<T>(i, j) <= cmp) return true;
    }
  }
  return false;
}

<<Convenience Functions>>=
template <typename T>
std::set<std::pair<uint16_t, uint16_t> > neighbors_ge(const int& y, const int& x,
                                                      const cv::Mat& im, const int& cmp) {
  cv::Size size = im.size();
  std::set<std::pair<uint16_t, uint16_t> > neighbors;
  for (int i = std::max(0, y-1); i <= std::min(y+1, size.height-1); ++i) {
    for (int j = std::max(0, x-1); j <= std::min(x+1, size.width-1); ++j) {
      if (im.at<T>(i, j) >= cmp) {
        neighbors.insert(std::make_pair(i, j));
      }
    }
  }
  return neighbors;
}

<<Convenience Functions>>=
const int NOISE_DIST = 3;
template<typename T>
bool neighbors_bend(const int& y, const int& x, const cv::Mat& im) {
  const T& val = im.at<T>(y, x);

  if (val < NOISE_DIST)
    return false;

  cv::Size size = im.size();
  int sum = 0,
      count = 0;
  for (int i = std::max(0, y-1); i <= std::min(y+1, size.height-1); ++i) {
    for (int j = std::max(0, x-1); j <= std::min(x+1, size.width-1); ++j) {
      if (i != y || j != x) {
        sum += im.at<T>(i, j);
        ++count;
      }
    }
  }
  return sum < count * val;
}

<<Q1.cpp>>=
<<Include>>
<<Global constants>>
<<Convenience Functions>>

int main(int argc, char* argv[]) {
  <<Command line args>>

  std::string im_path = path + "/images/for_skeleton.png";
  cv::Mat image = cv::imread(im_path, 0);
  if(!image.data) {
    std::cout << "Failed to read image " << im_path << std::endl;
    return 1;
  }

  uint16_t max_val = std::numeric_limits<uint16_t>::max();
  cv::Size size = image.size();
  cv::Mat distances(size, CV_16U, max_val);
  std::set<std::pair<uint16_t, uint16_t> > *boundary = new std::set<std::pair<uint16_t, uint16_t> >();
  for (int i = 0; i < size.height; ++i) {
    for (int j = 0; j < size.width; ++j) {
      if (image.at<uint8_t>(i, j) > 0) {
        if (neighbors_le<uint8_t>(i, j, image, 0)) {
          // Not black but has black neighbors -> boundary
          distances.at<uint16_t>(i, j) = 1;
          std::set<std::pair<uint16_t, uint16_t> > boundary_update = neighbors_ge<uint8_t>(i, j, image, 1);
          boundary->insert(boundary_update.begin(), boundary_update.end());
        }
      } else {
        // Interior pixel
        distances.at<uint16_t>(i, j) = 0;
      }
    }
  }

  uint16_t dist = 2;
  cv::Mat visualize = cv::Mat::zeros(size, CV_8U);  // TODO(HD) remove this and all uses
  while (!boundary->empty()) {
    std::set<std::pair<uint16_t, uint16_t> > *new_boundary = new std::set<std::pair<uint16_t, uint16_t> >();
    for (auto&& boundary_pt : *boundary) {
      distances.at<uint16_t>(boundary_pt.first, boundary_pt.second) = dist;
      visualize.at<uint8_t>(boundary_pt.first, boundary_pt.second) = 255;
    }
    for (auto&& boundary_pt : *boundary) {
      std::set<std::pair<uint16_t, uint16_t> > update = neighbors_ge<uint16_t>(boundary_pt.first, boundary_pt.second, distances, max_val);
      new_boundary->insert(update.begin(), update.end());
    }
    ++dist;
    delete boundary;  boundary = new_boundary;

    if (display) {
      cv::imshow("Grassfire Distances", visualize);
      cv::waitKey(100);
    }
    visualize = cv::Mat::zeros(size, CV_8U);
  }

  cv::Mat skeleton = cv::Mat::zeros(size, CV_8U);
  for (int i = 0; i < size.height; ++i) {
    for (int j = 0; j < size.width; ++j) {
      if (neighbors_bend<uint16_t>(i, j, distances))
        skeleton.at<uint8_t>(i, j) = 255;
    }
  }

  double max_dist_fp;
  cv::minMaxIdx(distances, nullptr, &max_dist_fp);
  uint16_t max_dist = max_dist_fp;
  cv::Mat display_distances = distances * max_val / max_dist;
  cv::imwrite(path + "/output/grassfire.png", display_distances, PNG_COMPRESSION);
  cv::imwrite(path + "/output/skeleton.png", skeleton, PNG_COMPRESSION);

  if (display) {
    cv::imshow("Grassfire Distances", display_distances);
    cv::waitKey(0);

    cv::imshow("Skeleton", skeleton);
    cv::waitKey(0);
  }
}

@

The results of this grassfire transform and skeleton detection can be seen in Figure~\ref{fig:skeleton}.

\begin{figure}[h]
  \subfloat[\label{fig:for_skeleton}]{\includegraphics[width=0.33\textwidth]{images/for_skeleton}} \hfill
  \subfloat[\label{fig:grassfire}]{\includegraphics[width=0.33\textwidth]{output/grassfire}} \hfill
  \subfloat[\label{fig:out_skeleton}]{\includegraphics[width=0.33\textwidth]{output/skeleton}}
  \caption{(\ref{fig:for_skeleton}) Original binary image. (\ref{fig:grassfire}) Result of grassfire transform. (\ref{fig:out_skeleton}) Skeleton calculated from grassfire transform.}
  \label{fig:skeleton}
\end{figure}
