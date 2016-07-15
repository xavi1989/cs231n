#include <stdio.h>

#include "opencv2/opencv.hpp"

#include "PM.hpp"

using namespace cv;

int main(int, char**)
{
    VideoCapture cap("test-videos/mona-lisa.avi"); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;

    Mat frame1;
    Mat frame2;

    PM tmp;
    namedWindow("Display window", cv::WINDOW_AUTOSIZE);

    // init
    cap.read(frame1);
    frame1.convertTo(frame2, CV_8UC4);
    tmp.initialize(frame2);

    while(cap.read(frame1))
    {
        frame1.convertTo(frame2, CV_8UC4);
        tmp.process_frame(frame2);

        imshow("Display window", frame2);
        if(waitKey(30) >= 0) break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor

    return 0;
}
