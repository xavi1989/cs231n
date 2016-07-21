#include "Common.hpp"

using namespace cv;

class KLTTracker
{
    
public:
    
    KLTTracker(int max_points=500, cv::Size win_size=cv::Size(9, 9));
    
    virtual ~KLTTracker() = default;
    
    void initialize(const GrayscaleImage& frame, const PointArray& keypoints = PointArray());
    
    void track(const GrayscaleImage& frame, MatchHandler& match_handler);
    
private:
    
    int max_points_;
    
    cv::Size win_size_;

    PointArray keypoints_;

    PointArray preKeypoints_;

    GrayscaleImage preFrame_;
    
    cv::TermCriteria term_crit_;
    
    void harris_corner_detector(const GrayscaleImage& image, cv::Mat& corners, int block_size, float k);
    
    void detect_features(const GrayscaleImage& image,
                         PointArray& corners,
                         double quality_level,
                         double min_distance,
                         int block_size);
    
    void debug_corner_detector();
};
