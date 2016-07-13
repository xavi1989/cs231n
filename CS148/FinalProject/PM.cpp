#include "PM.hpp"

PM::PM()
{
    has_been_initialized_ = false;
}

void PM::initialize(ColorImage &frame)
{
    //Initialize tracking subsystem.
    convert_to_grayscale(frame, grayscaleFrame_);
    tracker_.initialize(grayscaleFrame_);

    //Initialize augmentor.
    PointArray pts;
    int inset = 5;
    cv::Size frame_size = frame.size();
    pts.push_back(cv::Point2f(inset, inset));
    pts.push_back(cv::Point2f(frame_size.width - 2*inset, inset));
    pts.push_back(cv::Point2f(frame_size.width - 2*inset, frame_size.height - 2*inset));
    pts.push_back(cv::Point2f(inset, frame_size.height - 2*inset));
    augmentor_.initialize(pts);
    //augmentor_.set_title(art_info.title);

    has_been_initialized_ = true;
}

void PM::process_frame(ColorImage& frame)
{
    assert(has_been_initialized_ && "Must be initialized before calling process_frame!");

    // Track target.
    convert_to_grayscale(frame, grayscaleFrame_);
    Homography H;
    bool tracking_successful = tracker_.track(grayscaleFrame_, H);

    // Augment frame.
    augmentor_.augment(frame, H, !tracking_successful);
}

void PM::convert_to_grayscale(const ColorImage& colorImage, GrayscaleImage& grayscaleImage)
{
    assert(colorImage.type()==CV_8UC4 && "Frame type must be BGRA!");
    cv::cvtColor(colorImage, grayscaleImage, CV_BGRA2GRAY);
}
