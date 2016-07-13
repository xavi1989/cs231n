#include "PlaneTracker.hpp"

static const char* TAG = "PlaneTracker";

// Re-initialize any internal state that you introduce in this function.
void PlaneTracker::initialize(const GrayscaleImage &frame)
{
    klt_tracker_.initialize(frame);
    orb_tracker_.initialize(frame);
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
    return false;
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
            LOG_ERROR(TAG, "Tracker provided less than 4 correspondences for homography estimation!");
        }
    };
    
    // For 2.1: Just forward the call to klt_tracker.
    
    // For 2.2: Introduce the relocalization logic.    
    
    return estimation_successful;
}
