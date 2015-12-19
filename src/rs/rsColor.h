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
        double  a;

    public:
        Color()
            :   rs::Vec3d(),
                r(x),
                g(y),
                b(z),
                a(1.0)
        {
        }

        Color(const double r, const double g, const double b, const double a)
            :   rs::Vec3d(r, g, b),
                r(x),
                g(y),
                b(z),
                a(a)
        {
        }
    };
}

#endif // RS_COLOR_H
