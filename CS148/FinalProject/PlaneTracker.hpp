#include "Common.hpp"
#include "KLTTracker.hpp"
#include "ORBTracker.hpp"


class PlaneTracker
{
public:
    
    PlaneTracker() = default;
    
    virtual ~PlaneTracker() = default;
    
    void initialize(const GrayscaleImage& frame);
    
    bool track(const GrayscaleImage& frame, Homography& H);
    
private:        
    
    KLTTracker klt_tracker_;

    Homography current_H;
    
    ORBTracker orb_tracker_;
    
    bool is_lost_;
            
    bool estimate_homography(const PointArray& src_points, const PointArray& dst_points, Homography& H);
    
};
