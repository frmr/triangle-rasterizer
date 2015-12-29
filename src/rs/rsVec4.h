#ifndef RS_VEC4_H
#define RS_VEC4_H

#include <cmath>
#include "rsVector.h"

namespace rs
{
    template<typename T>
    class Vec4 : public rs::Vector<4,T>
    {
    public:
        T&  x;
        T&  y;
        T&  z;
        T&  w;
        static constexpr T epsilon = (T) 0.000000;

    public:
        double Length() const
        {
            return sqrt(x*x + y*y + z*z);
        }

        Vec4<T>& Reverse()
        {
            x = -x;
            y = -y;
            z = -z;
            return *this;
        }

        Vec4<T>& Unit()
        {
            T length = Length();
            x /= length;
            y /= length;
            z /= length;
            return *this;
        }

        template<typename U>
        Vec4<T> operator+(const Vec4<U>& rhs) const
        {
            return rs::Vec4<T>(x + (T) rhs.x, y + (T) rhs.y, z + (T) rhs.z, w);
        }

        template<typename U>
        Vec4<T> operator-(const Vec4<U>& rhs) const
        {
            return rs::Vec4<T>(x - (T) rhs.x, y - (T) rhs.y, z - (T) rhs.z, w);
        }

        template<typename U>
        Vec4<T> operator*(const U& rhs) const
        {
            return rs::Vec4<T>(x * (T) rhs, y * (T) rhs, z * (T) rhs, w);
        }

        template<typename U>
        Vec4<T> operator/(const U& rhs) const
        {
            return rs::Vec4<T>(x / (T) rhs, y / (T) rhs, z / (T) rhs, w);
        }

        template<typename U>
        Vec4<T>& operator+=(const Vec4<U>& rhs)
        {
            x += (T) rhs.x;
            y += (T) rhs.y;
            z += (T) rhs.z;
            return *this;
        }

        template<typename U>
        Vec4<T>& operator-=(const Vec4<U>& rhs)
        {
            x -= (T) rhs.x;
            y -= (T) rhs.y;
            z -= (T) rhs.z;
            return *this;
        }

        template<typename U>
        Vec4<T>& operator*=(const U& rhs)
        {
            x *= (T) rhs;
            y *= (T) rhs;
            z *= (T) rhs;
            w *= (T) rhs;
            return *this;
        }

        template<typename U>
        Vec4<T>& operator/=(const U& rhs)
        {
            x /= (T) rhs;
            y /= (T) rhs;
            z /= (T) rhs;
            w /= (T) rhs;
            return *this;
        }

        template<typename U>
        bool operator==(const U& rhs) const
        {
            return (x <= (T) rhs.x + epsilon &&
                    x >= (T) rhs.x - epsilon &&
                    y <= (T) rhs.y + epsilon &&
                    y >= (T) rhs.y - epsilon &&
                    z <= (T) rhs.z + epsilon &&
                    z >= (T) rhs.z - epsilon &&
                    w <= (T) rhs.w + epsilon &&
                    w >= (T) rhs.w - epsilon);
        }

        explicit operator rs::Vec4<double>() const
        {
            return Vec4<double>((double) x, (double) y, (double) z, (double) w);
        }

        explicit operator rs::Vec4<float>() const
        {
            return Vec4<float>((float) x, (float) y, (float) z, (float) w);
        }

    public:
        Vec4<T>()
            :   rs::Vector<4,T>(),
                x(this->data[0]),
                y(this->data[1]),
                z(this->data[2]),
                w(this->data[3])
        {
        }

        Vec4<T>(const rs::Vector<4,T>& vecA)
            :   rs::Vector<4,T>(),
                x(this->data[0]),
                y(this->data[1]),
                z(this->data[2]),
                w(this->data[3])
        {
            for (size_t i = 0; i < 4; ++i)
            {
                this->data[i] = vecA.GetAt(i);
            }
        }

        Vec4<T>(const T& xInit, const T& yInit, const T& zInit, const T& wInit)
            :   rs::Vector<4,T>(),
                x(this->data[0]),
                y(this->data[1]),
                z(this->data[2]),
                w(this->data[3])
        {
            x = xInit;
            y = yInit;
            z = zInit;
            w = wInit;
        }
    };

    typedef	Vec4<float>     Vec4f;
    typedef	Vec4<double>    Vec4d;
    typedef Vec4<int>       Vec4i;
}

#endif
