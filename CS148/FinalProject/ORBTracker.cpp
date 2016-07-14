#include "ORBTracker.hpp"

using namespace cv;
using namespace std;

static const char* TAG = "ORBTracker";

ORBTracker::ORBTracker()
{
}

// Initialize your tracking using the provided image.
void ORBTracker::initialize(const GrayscaleImage& frame)
{
    printf(TAG, "Initialized.");
}

// Initialize your tracking subsystem using the provided frame.
// Note that this function may be called multiple times to restart tracking.
// Make sure you reset all necessary internal state!
void ORBTracker::track(const GrayscaleImage& frame, MatchHandler& match_handler)
{
    // Step 1: Detect ORB features.

    // Step 2: Find matches (using Hamming distance).

    // Step 3: Prune matches based on some heuristic (like the ratio test).

    // Step 4: Call match_handler if sufficiently many correspondences were found.
}
