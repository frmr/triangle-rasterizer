#ifndef RS_COLOR_H
#define RS_COLOR_H

#include "rsVec3.h"

namespace rs
{
    class Color : public rs::Vec3d
    {
    public:
        double& r;
        double& g;
        double& b;

    public:
        rs::Color::Color( const double r, const double g, const double b )
            :   rs::Vec3d( r, g, b ),
                r( x ),
                g( y ),
                b( z )
        {
        }
    };
}

#endif // RS_COLOR_H
