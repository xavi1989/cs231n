#include "PlaneTracker.hpp"

static const char* TAG = "PlaneTracker";

// Re-initialize any internal state that you introduce in this function.
void PlaneTracker::initialize(const GrayscaleImage &frame)
{
    klt_tracker_.initialize(frame);
    orb_tracker_.initialize(frame);

    current_H(0, 0) = 1.0;
    current_H(1, 1) = 1.0;
    current_H(2, 2) = 1.0;

#if DEBUG
    std::cout<<"PlaneTracker initialize Matrix H: "<<std::endl;
    std::cout<<current_H<<std::endl;
#endif
    is_lost_ = false;
}

// Given a set of correspondences, compute the homography using RANSAC.
// Compute the inlier ratio (number of RANSAC inliers/number of correspondences),
// and return true if it's above a certain threshold. Otherwise, return false.
// Use the test videos to determine a reasonable threshold.
// Note that we expect the homography to be between the reference frame (the one provided during
// initialization) and the current frame.
// The KLT tracker provides correspondences between the previous frame and the current frame.
// The ORB tracker provides correspondences between the reference frame and the current frame.
// Hint: Introduce a member variable in the class for an "internal" homography.
bool PlaneTracker::estimate_homography(const PointArray& src_points, const PointArray& dst_points, Homography& H)
{
    if(src_points.size() != dst_points.size() ||
       src_points.size() < 3) {
        return false;
    }

    double ransacReprojThreashold = 3;
    std::vector<unsigned char> inliersMask(src_points.size());
    int count = 0;
    float ratio = 0;
    float inlierThreshold = 0.75;

    H = findHomography(src_points, dst_points, CV_RANSAC, ransacReprojThreashold, inliersMask);

    for(size_t i=0; i<inliersMask.size(); i++) {
        if(inliersMask[i])
            count++;
    }

    ratio = (float)count / inliersMask.size();

    if(ratio < inlierThreshold)
        return false;

    current_H = current_H * H;

#if DEBUG
    std::cout<<"estimate_homography H "<<std::endl;
    std::cout<<H<<std::endl;
    std::cout<<"estimate_homography final H "<<std::endl;
    std::cout<<current_H<<std::endl;
#endif

    return true;
}

// Implement the tracking logic as described on the project page.
// Your implementation should follow the logic described in the flowchart.
bool PlaneTracker::track(const GrayscaleImage &frame, Homography &H)
{
    bool estimation_successful = false;
    MatchHandler match_handler = [&](const PointArray& src_points, const PointArray& dst_points)
    {
        assert(src_points.size()==dst_points.size());
        if(dst_points.size()>=4)
        {
            estimation_successful = this->estimate_homography(src_points, dst_points, H);
        }
        else
        {
            printf(TAG, "Tracker provided less than 4 correspondences for homography estimation!");
        }
    };
    
    // For 2.1: Just forward the call to klt_tracker.
    this->klt_tracker_.track(frame, match_handler);

    // For 2.2: Introduce the relocalization logic.    
    if(!estimation_successful) {
        
    }

    return estimation_successful;
}
