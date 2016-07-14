#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <cassert>
#include <functional>
#include <memory>
#include <vector>

typedef cv::Matx33f Homography;

typedef cv::Mat ColorImage;

typedef cv::Mat GrayscaleImage;

typedef std::vector<cv::Point2f> PointArray;

typedef std::function<void(const PointArray&, const PointArray&)> MatchHandler;
