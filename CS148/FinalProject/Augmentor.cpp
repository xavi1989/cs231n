#include "Augmentor.hpp"

static const int TEXT_OUTER_THICKNESS = 10;

static const int TEXT_INNER_THICKNESS = 2;

Augmentor::Augmentor()
: font_face_(CV_FONT_HERSHEY_SIMPLEX)
, font_scale_(1.0)
{
    
}

void Augmentor::initialize(const PointArray& target_bounds)
{
    reference_bounds_ = target_bounds;
    current_bounds_ = target_bounds;
}
    
void Augmentor::set_title(const std::string& title)
{
    title_ = title;
}

// Draw the current bounding box for the painting by transforming
// the reference points using the provided homography.
// The reference_bounds_ variable contains the locations of the four
// corners in the reference coordinate frame.
// Make sure you update the current_bounds_ variable!
void Augmentor::render_bounds(ColorImage& frame, const Homography& H)
{
    cv::Scalar color(255, 255, 255);
    PointArray new_bounds;

    cv::perspectiveTransform(current_bounds_, new_bounds, H);

    for(int i=0; i<current_bounds_.size()-1; i++) {
        cv::line(frame, new_bounds[i], new_bounds[i+1], color);
    }
    cv::line(frame, new_bounds[0], new_bounds[1], color);
    cv::line(frame, new_bounds[1], new_bounds[2], color);
    cv::line(frame, new_bounds[2], new_bounds[3], color);
    cv::line(frame, new_bounds[3], new_bounds[0], color);
    
    current_bounds_ = new_bounds;
}

void Augmentor::render_title(ColorImage& frame)
{
    if(current_bounds_.size())
    {
        render_text(frame, title_, current_bounds_[0]);
    }
}
    
void Augmentor::augment(ColorImage &frame, const Homography& H, bool lost)
{
#if DEBUG
    std::cout<<"augment lost "<<lost<<std::endl;
#endif
    if(!lost)
    {
        render_bounds(frame, H);
        render_title(frame);
    }
    else
    {
        std::string lost_msg = "LOST!";
        cv::Size text_size = cv::getTextSize(lost_msg, font_face_, font_scale_, TEXT_OUTER_THICKNESS, nullptr);
        cv::Point text_origin = cv::Point((frame.cols - text_size.width)/2, (frame.rows - text_size.height)/2);
        render_text(frame, lost_msg, text_origin);
    }
}

void Augmentor::render_text(ColorImage& frame, const std::string& text, const cv::Point2f& pt)
{
    // A quick and dirty trick for rendering bordered text.
    // Feel free to replace this with a more sophisticated method.
    cv::putText(frame, text, pt, font_face_, font_scale_, cv::Scalar(0, 0, 0), TEXT_OUTER_THICKNESS);
    cv::putText(frame, text, pt, font_face_, font_scale_, cv::Scalar(255, 255, 255), TEXT_INNER_THICKNESS);
}
