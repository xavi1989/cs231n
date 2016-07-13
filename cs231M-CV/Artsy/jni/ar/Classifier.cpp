#include "ar/Classifier.hpp"
#include "ar/ResourceLocator.hpp"
#include <sstream>
#include <string>

namespace ar
{

Classifier::Classifier(float confidence_threshold) : confidence_threshold_(confidence_threshold)
{
    //Build art info db.
    art_info_.push_back({.title = "Convergence", .artist = "Jackson Pollock"});
    art_info_.push_back({.title = "Concetto Spaziale", .artist = "Lucio Fontana"});
    art_info_.push_back({.title = "Mona Lisa", .artist = "Leonardo da Vinci"});
    art_info_.push_back({.title = "Nighthawks", .artist = "Edward Hopper"});
    art_info_.push_back({.title = "The Scream", .artist = "Edvard Munch"});

    //Construct convolutional neural network.
    conv_net_ = std::make_shared<ConvNet>(PathForResource("jetpac.ntwk"));

    //Load binary SVMs
    size_t num_classes = art_info_.size();
    for(int i=0; i<num_classes; ++i)
    {
        std::ostringstream s;
        s << "svm-params-" << i << ".txt";
        std::string params_path = PathForResource(s.str());
        svm_classifiers_.push_back(std::make_shared<BinarySVM>(params_path));
    }
}

ArtInfo Classifier::get_art_info(int label) const
{
    if(label==-1)
    {
        return {.title = "Unknown", .artist="Unknown"};
    }
    assert(label<art_info_.size());
    return art_info_[label];
}

// Classify the image and return the class index (0<=index<5).
int Classifier::classify(const ColorImage& image) const
{
    // Step 1.
    // Use the conv_net_ object to extract features
    // using the convolutional neural network.

    // Step 2.
    // Use our bank of SVM classifiers to detect the class.
    // Pick the index with the highest probability.
    // Enforce a lower bound (confidence_threshold_) on the
    // probability: if none of the classifiers are
    // sufficiently confident, return -1.
    return -1;
}

}