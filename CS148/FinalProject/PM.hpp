#include "Common.hpp"
#include "PlaneTracker.hpp"
#include "Augmentor.hpp"

class ARApp
{
public:
    
    ARApp();
    
    virtual ~ARApp() = default;
    
    void initialize(ColorImage& frame);
    
    // The main entry point into our augmented reality sub-system.
    // Note that this function mutates the input frame with the augmentations.
    // Expected input: An 8UC3 RGB image.
    void process_frame(ColorImage& frame);
    
private:
    
    bool has_been_initialized_;
    
    GrayscaleImage grayscaleFrame_;
    
    PlaneTracker tracker_;
    
    Augmentor augmentor_;
    
    Classifier classifier_;
    
    void convert_to_grayscale(const ColorImage& colorImage, GrayscaleImage& grayscaleImage);
};
