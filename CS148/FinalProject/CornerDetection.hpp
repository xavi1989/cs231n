#include "Common.hpp"

typedef std::function<void(const GrayscaleImage&, cv::Mat&, int)> HarrisCornerDetector;

void DetectGoodFeaturesToTrack(const GrayscaleImage& image,
                               PointArray& _corners,
                               int maxCorners,
                               double qualityLevel,
                               double minDistance,
                               int blockSize,
                               HarrisCornerDetector harrisCornerDetector);
