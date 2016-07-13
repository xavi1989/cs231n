#ifndef __Artsy__ORBTracker__
#define __Artsy__ORBTracker__

#include "ar/Common.hpp"

namespace ar
{

class ORBTracker
{
public:

    ORBTracker();

    virtual ~ORBTracker() = default;

    void initialize(const GrayscaleImage& frame);

    void track(const GrayscaleImage& frame, MatchHandler& match_handler);

private:

};

}

#endif /* defined(__Artsy__ORBTracker__) */
