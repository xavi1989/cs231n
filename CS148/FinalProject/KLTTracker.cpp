#include "KLTTracker.hpp"
#include "CornerDetection.hpp"

using namespace cv;

KLTTracker::KLTTracker(int max_points, cv::Size win_size)
{
    max_points_ = max_points;
    win_size_ = win_size;
    term_crit_ = cv::TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 20, 0.03);
}

// Initialize your tracking subsystem using the provided frame.
// If the provided keypoints array is empty, detect them using a corner detector.
// Otherwise, use the provided points to initialize your tracker.
// Note that this function may be called multiple times to restart tracking.
// Make sure you reset all necessary internal state!
void KLTTracker::initialize(const GrayscaleImage& frame, const PointArray& keypoints)
{
    if(keypoints.empty())
    {
        // Set this to true for Section 3.
        const bool use_my_harris_detector = false;
        
        if(!use_my_harris_detector)
        {
            int maxCorners = max_points_;
            double qualityLevel = 0.01;
            double minDistance = 10;
            int blockSize = 9;
            bool useHarrisDetector = false;
            double k = 0.04;

            // Use OpenCV's goodFeaturesToTrack function.
            goodFeaturesToTrack(frame, preKeypoints_, maxCorners,
                                    qualityLevel, minDistance, cv::Mat(),
                                    blockSize, useHarrisDetector, k);
        }
        else
        {
            // Call detect_features with the same parameters as goodFeaturesToTrack.
        }
        
        // Optional: use cornerSubPix to refine your corners.
        cornerSubPix(frame, preKeypoints_, win_size_,
                        Size(-1, -1), term_crit_);
    }
    else
    {
        // Use the provided keypoints as reference.
        preKeypoints_ = keypoints;
    }

    preFrame_ = frame.clone();
}

// Track the points in the provided grayscale frame. If you find sufficiently many
// correspondences (the exact number is up to you), call the match_handler callback
// function. The syntax is match_handler(previous_points, current_points).
void KLTTracker::track(const GrayscaleImage& frame, MatchHandler& match_handler)
{    
    // Step 1 : Compute optical flow.
    // Hint   : calcOpticalFlowPyrLK
    std::vector<uchar> status;
    std::vector<float> errors;
    std::vector<cv::Point2f> srcPoints;
    std::vector<cv::Point2f> dstPoints;

    calcOpticalFlowPyrLK(preFrame_, frame,
                         preKeypoints_, keypoints_,
                         status, errors,
                         win_size_, 3,
                         term_crit_, 0);
    // Step 2 : Prune points based on the status returned by calcOpticalFlowPyrLK.
    for(size_t i=0; i<status.size(); i++) {
        if(status[i]) {
            srcPoints.push_back(preKeypoints_[i]);
            dstPoints.push_back(keypoints_[i]);
        }
    }

    // Step 3 : Call the match handler with the correspondences.
    match_handler(srcPoints, dstPoints);

    // Step 4 : Update your internal variables.
    preFrame_ = frame.clone();
    preKeypoints_ = keypoints_;
}

// Given a source image (src), create and populate the provided score_matrix such
// that each pixel in the score_matrix contains the Harris corner response for the
// corresponding pixel in the source image.
// Use the provided window size (block_size) and kappa value (k) for your algorithm.
void KLTTracker::harris_corner_detector(const GrayscaleImage& src, Mat& score_matrix, int block_size, float k)
{
    // Step 1 : Compute the horizontal and vertical image derivatives.
    // Hint   : The Sobel function will be handy here.
    
    // Step 2 : Compute the Harris matrix.
    // Hint   : A three channel image might be handy here.
    
    // Step 3 : Compute the Harris Score for each pixel
}

void KLTTracker::detect_features(const GrayscaleImage& image,
                                 PointArray& corners,
                                 double quality_level,
                                 double min_distance,
                                 int block_size)
{
    HarrisCornerDetector harris = [&](const GrayscaleImage& hc_img, cv::Mat& hc_eig, int hc_block_size)
    {
        harris_corner_detector(hc_img, hc_eig, hc_block_size, 0.04);
    };
    DetectGoodFeaturesToTrack(image, corners, max_points_, quality_level, min_distance, block_size, harris);
}

// You can use this method to debug your corner detection algorithm.
void KLTTracker::debug_corner_detector()
{
    // Create a checkerboard pattern.
    int pattern_size = 600;
    int block_size = 60;
    GrayscaleImage pattern(pattern_size, pattern_size, CV_8UC1);
    for(int i=0; i<pattern_size; ++i)
    {
        for(int j=0; j<pattern_size; ++j)
        {
            pattern.at<uint8_t>(i, j) = 255*(((i/block_size)%2)==((j/block_size)%2));
        }
    }
    
    PointArray corners;
    // Call your corner detector here.
    
    // Display the detected corners.
    ColorImage debug_image;
    cvtColor(pattern, debug_image, CV_GRAY2BGR);
    for(auto& pt: corners)
    {
        cv::circle(debug_image, pt, 3, cv::Scalar(0, 0, 255), 3);
    }
    
    cv::imshow("Detected Corners", debug_image);
    //cv::waitKey();
}
