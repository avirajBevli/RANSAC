# RANSAC
The file "clustering_sorted.cpp" detects lines by RANSAC algorithm, draws them as lines of infinite length, clusters the lines by using K-means clustering algorithm and finally outputs the clustered set of detected lines
The file "3_lines.cpp" detects the points which voted for valid lines by RANSAC and then colors all those points. Hence all the finally colored points are a part of the detected line
