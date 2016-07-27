#include "SimpleImage.h"
#include "Utility.h"
#include <cstdlib>
#include <ctime>
#include <omp.h>
#include <Windows.h>
#include <iostream>

using namespace std;

void sse_test(int img_w, int img_h, int tracing_scene, int effort) {

    // Allocate intermediate image, 16-byte memory aligned
    RGBColor *i_image = (RGBColor *)_aligned_malloc(img_w * img_h * sizeof(RGBColor), 16);
    for (int h = 0; h < img_h; h++) {
        for (int w = 0; w < img_w; w++) {
            *(i_image + h * img_w + w) = RGBColor(0.5, 0.5, 0.5);
        }
    }

#pragma omp parallel
    {
        int i = omp_get_thread_num();
        printf_s("Hello from sse test thread %d\n", i);
    }

    double wall0 = get_wall_time();
    double cpu0 = get_cpu_time();

    // Generate ray based on effort for each pixel and trace for the pixel's color.
#pragma omp parallel for
    for (int h = 0; h < img_h; h++) {
#pragma omp critical(print_progress)
        {
            std::cout << "Progress sse test: working on row " << (h + 1) << " out of " << img_h << " rows." << std::endl;
        }

        for (int w = 0; w < img_w; w++) {
            for (int e = 0; e < effort; e++) {
                *(i_image + h * img_w + w) = (*(i_image + h * img_w + w)) + (*(i_image + h * img_w + w));
            }
        }
    }

    double wall1 = get_wall_time();
    double cpu1 = get_cpu_time();
    cout << "SSE Wall Time = " << wall1 - wall0 << endl;
    cout << "SSE CPU Time  = " << cpu1 - cpu0 << endl;

}
