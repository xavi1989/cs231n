#include <stdio.h>

#include "opencv2/opencv.hpp"

#include "PM.hpp"

using namespace cv;

int main(int, char**)
{
    PM tmp;

    VideoCapture cap("test-videos/mona-lisa.avi"); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;

    Mat frame;
    while(cap.read(frame))
    {
        imshow("edges", frame);
        if(waitKey(30) >= 0) break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor

    return 0;
}
