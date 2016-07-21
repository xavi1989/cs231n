#include "Common.hpp"

class ORBTracker
{
public:

    ORBTracker();

    virtual ~ORBTracker() = default;

    void initialize(const GrayscaleImage& frame);

    void track(const GrayscaleImage& frame, MatchHandler& match_handler);

private:

};
