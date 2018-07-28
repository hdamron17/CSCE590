@ \section*{Part 1: Medial Axes}

The goal of this section is to implement the grassfire transform and use it to mark the skeleton of a binary image

@ \subsection*{Convenience Functions}

We begin by defining several convenience functions to be used in the grassfire and skeleton procedures.
The first of these convenience functions [[neighbors_le]] takes a pixel location $(y,x)$ for image $I$ and returns true if $I_{y',x'} \leq q ~\exists~ I_{y',x'} \in N_8(I_{y,x})$ where $q$ is the comparison value.
This function is used in [[grassfire]] to determine the boundaries of the binary image.

<<Convenience Functions>>=
template <typename T>
bool neighbors_le(const int& y, const int& x, const cv::Mat& I, const int& q) {
  cv::Size size = I.size();
  for (int i = std::max(0, y-1); i <= std::min(y+1, size.height-1); ++i) {
    for (int j = std::max(0, x-1); j <= std::min(x+1, size.width-1); ++j) {
      if (I.at<T>(i, j) <= q) return true;
    }
  }
  return false;
}

@ The second convenience function [[neighbors_ge]] returns a set $\left\{(y',x') \mid I_{y',x'} \in N_8(I_{y,x}) \land I_{y',x'} \geq q\right\}$.
This function is used in [[grassfire]] on the distances matrix $D$ to determine the next boundary from the current boundary.

<<Convenience Functions>>=
template <typename T_in, typename T_out=uint16_t>
std::set<std::pair<T_out, T_out> > neighbors_ge(const int& y, const int& x,
                                                const cv::Mat& I, const int& q) {
  cv::Size size = I.size();
  std::set<std::pair<T_out, T_out> > neighbors;
  for (int i = std::max(0, y-1); i <= std::min(y+1, size.height-1); ++i) {
    for (int j = std::max(0, x-1); j <= std::min(x+1, size.width-1); ++j) {
      if (I.at<T_in>(i, j) >= q) {
        neighbors.insert(std::make_pair(i, j));
      }
    }
  }
  return neighbors;
}

@ The third convenience function [[neighbors_bend]] compares the average of the surrounding pixels to the pixel of interest.
If the pixel of interest is larger than the average of its neighbors in the distances matrix $D$, it is considered part of the skeleton because it is likely a collision of wavefronts in the grassfire transform.
To improve robustness to noise, pixels with a distance smaller than a constant [[NOISE_DIST]] are excluded from the skeleton.
<<Convenience Functions>>=
const int NOISE_DIST = 3;
template<typename T>
bool neighbors_bend(const int& y, const int& x, const cv::Mat& I) {
  const T& val = I.at<T>(y, x);

  if (val < NOISE_DIST)
    return false;

  cv::Size size = I.size();
  int sum = 0,
      count = 0;
  for (int i = std::max(0, y-1); i <= std::min(y+1, size.height-1); ++i) {
    for (int j = std::max(0, x-1); j <= std::min(x+1, size.width-1); ++j) {
      if (i != y || j != x) {
        sum += I.at<T>(i, j);
        ++count;
      }
    }
  }
  return sum < count * val;
}

@ \section*{Grassfire Transform}

Next, the grassfire transform is defined in Algorithm \ref{alg:grassfire} and then implemented as [[grassfire]].

\begin{algorithm}
\caption{Grassfire transform} \label{alg:grassfire}
\begin{algorithmic}
\Function{Grassfire}{$I$} \Comment{Grassfire transform of image matrix $I$}
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
      \State $B \gets B \cup \left\{ I_{y',x'} \mid I_{y',x'} \in N_8(I_{y,x}) \land D_{y',x'} > D_{y,x} \right\}$
        \Comment{Create next boundary}
    \EndFor
  \EndWhile
  \State \Return{$D$}
\EndFunction
\end{algorithmic}
\end{algorithm}

<<[[grassfire]] function>>=
template<typename T_in, typename T_out=uint16_t>
cv::Mat_<T_out> grassfire(cv::Mat I) {
  T_out max_val = std::numeric_limits<T_out>::max();
  cv::Size size = I.size();
  cv::Mat_<T_out> D(size, max_val);
  std::set<std::pair<T_out, T_out> > *B = new std::set<std::pair<T_out, T_out> >();
  for (int i = 0; i < size.height; ++i) {
    for (int j = 0; j < size.width; ++j) {
      if (I.at<T_in>(i, j) > 0) {
        if (neighbors_le<T_in>(i, j, I, 0)) {
          // Not black but has black neighbors -> B
          D(i, j) = 1;
          std::set<std::pair<T_out, T_out> > B_update = neighbors_ge<T_in>(i, j, I, 1);
          B->insert(B_update.begin(), B_update.end());
        }
      } else {
        // Interior pixel
        D(i, j) = 0;
      }
    }
  }

  T_out n = 2;
  while (!B->empty()) {
    std::set<std::pair<T_out, T_out> > *new_B = new std::set<std::pair<T_out, T_out> >();
    for (auto&& B_pt : *B) {
      D(B_pt.first, B_pt.second) = n;
    }
    for (auto&& B_pt : *B) {
      std::set<std::pair<T_out, T_out> > update
          = neighbors_ge<T_out>(B_pt.first, B_pt.second, D, max_val);
      new_B->insert(update.begin(), update.end());
    }
    ++n;
    delete B;  B = new_B;
  }

  return D;
}

@ \section*{Skeleton Detection}

After the grassfire transform is applied, the skeleton $S$ is extracted by the [[skeleton]] function.
The [[skeleton]] function extracts points using the [[neighbors_bend]] function described earlier.

<<[[skeleton]] function>>=
template<typename T_in, typename T_out=uint8_t>
cv::Mat_<T_out> skeleton(cv::Mat D) {
  cv::Size size = D.size();
  cv::Mat_<T_out> S = cv::Mat_<T_out>::zeros(size);
  for (int i = 0; i < size.height; ++i) {
    for (int j = 0; j < size.width; ++j) {
      if (neighbors_bend<T_in>(i, j, D))
        S(i, j) = 255;
    }
  }
  return S;
}

@ \subsection*{Implementation of [[main]]}

Finally, the [[grassfire]] and [[skeleton]] functions are applied in the [[main]] function.

<<Q1.cpp>>=
<<Include>>
<<Global constants>>
<<Convenience Functions>>
<<[[grassfire]] function>>
<<[[skeleton]] function>>

int main(int argc, char* argv[]) {
  <<Command line args>>

  std::string im_path = path + "/images/for_skeleton.png";
  cv::Mat I = cv::imread(im_path, 0);
  if(!I.data) {
    std::cout << "Failed to read image " << im_path << std::endl;
    return 1;
  }

  cv::Mat D = grassfire<uint8_t>(I);

  cv::Mat S = skeleton<uint16_t>(D);

  uint16_t max_val = std::numeric_limits<uint16_t>::max();
  double max_dist_fp;
  cv::minMaxIdx(D, nullptr, &max_dist_fp);
  uint16_t max_dist = max_dist_fp;
  cv::Mat display_D = D * max_val / max_dist;
  cv::imwrite(path + "/output/grassfire.png", display_D, PNG_COMPRESSION);
  cv::imwrite(path + "/output/skeleton.png", S, PNG_COMPRESSION);

  if (display) {
    cv::imshow("Grassfire Distances", display_D);
    cv::waitKey(0);

    cv::imshow("Skeleton", S);
    cv::waitKey(0);
  }
}

@ \subsection*{Results}

The results of this grassfire transform and skeleton detection can be seen in Figure~\ref{fig:skeleton}.

\begin{figure}[!ht]
  \subfloat[\label{subfig:for_skeleton}]{\includegraphics[width=0.33\textwidth]{images/for_skeleton}} \hfill
  \subfloat[\label{subfig:grassfire}]{\includegraphics[width=0.33\textwidth]{output/grassfire}} \hfill
  \subfloat[\label{subfig:skeleton}]{\includegraphics[width=0.33\textwidth]{output/skeleton}}
  \caption{\protect\subref{subfig:for_skeleton} Original binary image. \protect\subref{subfig:grassfire} Result of grassfire transform. \protect\subref{subfig:skeleton} Skeleton calculated from grassfire transform.}
  \label{fig:skeleton}
\end{figure}

@ \subsection*{Discussion}

As seen in Figure~\ref{fig:skeleton}, the [[grassfire]] transform produces good results, but the [[skeleton]] function is not resistant to noise.
It is capable of detecting horizontal and vertical lines in skeleton without much issue, and successfully detects other skeleton lines, but diagonals produce false skeleton components because of the discretization of the image.
This could possibly be improved by using a different [[neighbors_bend]] implementation, but the current implementation is not very robust to noise.
