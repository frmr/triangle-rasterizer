#ifndef RS_COLOR_H
#define RS_COLOR_H

#include "rsVec3.h"

namespace rs
{
    class Color : public rs::Vec3B
    {
    public:
        unsigned char& r;
        unsigned char& g;
        unsigned char& b;
        unsigned char  a;

    public:
        Color& operator=(const Color& rhs)
        {
            r = rhs.r;
            g = rhs.g;
            b = rhs.b;
            a = rhs.a;
            return *this;
        }

        Color()
            :   rs::Vec3B(),
                r(x),
                g(y),
                b(z),
                a(1.0)
        {
        }

        Color(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a)
            :   rs::Vec3B(r, g, b),
                r(x),
                g(y),
                b(z),
                a(a)
        {
        }
    };
}

#endif // RS_COLOR_H
