#ifndef RS_VEC3_H
#define RS_VEC3_H

#include <cmath>
#include "rsVector.h"

namespace rs
{
    template<typename T>
    class Vec3 : public rs::Vector<3,T>
    {
    public:
        T&  x;
        T&  y;
        T&  z;
        static constexpr T epsilon = (T) 0.000000;

    public:
        double Length() const
        {
            return sqrt(x*x + y*y + z*z);
        }

        Vec3<T>& Reverse()
        {
            x = -x;
            y = -y;
            z = -z;
            return *this;
        }

        Vec3<T>& Unit()
        {
            T length = Length();
            x /= length;
            y /= length;
            z /= length;
            return *this;
        }

        template<typename U>
        Vec3<T> operator+(const Vec3<U>& rhs) const
        {
            return rs::Vec3<T>(x + (T) rhs.x, y + (T) rhs.y, z + (T) rhs.z);
        }

        template<typename U>
        Vec3<T> operator-(const Vec3<U>& rhs) const
        {
            return rs::Vec3<T>(x - (T) rhs.x, y - (T) rhs.y, z - (T) rhs.z);
        }

        template<typename U>
        Vec3<T> operator*(const U& rhs) const
        {
            return rs::Vec3<T>(x * (T) rhs, y * (T) rhs, z * (T) rhs);
        }

        template<typename U>
        Vec3<T> operator*(const Vec3<U>& rhs) const
        {
            return rs::Vec3<T>(x * (T) rhs.x, y * (T) rhs.y, z * (T) rhs.z);
        }

        template<typename U>
        Vec3<T> operator/(const U& rhs) const
        {
            return rs::Vec3<T>(x / (T) rhs, y / (T) rhs, z / (T) rhs);
        }

        template<typename U>
        Vec3<T>& operator+=(const Vec3<U>& rhs)
        {
            x += (T) rhs.x;
            y += (T) rhs.y;
            z += (T) rhs.z;
            return *this;
        }

        template<typename U>
        Vec3<T>& operator-=(const Vec3<U>& rhs)
        {
            x -= (T) rhs.x;
            y -= (T) rhs.y;
            z -= (T) rhs.z;
            return *this;
        }

        template<typename U>
        Vec3<T>& operator*=(const U& rhs)
        {
            x *= (T) rhs;
            y *= (T) rhs;
            z *= (T) rhs;
            return *this;
        }

        template<typename U>
        Vec3<T>& operator*=(const Vec3<U>& rhs)
        {
            x *= (T) rhs.x;
            y *= (T) rhs.y;
            z *= (T) rhs.z;
            return *this;
        }

        template<typename U>
        Vec3<T>& operator/=(const U& rhs)
        {
            x /= (T) rhs;
            y /= (T) rhs;
            z /= (T) rhs;
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
                    z >= (T) rhs.z - epsilon);
        }

        explicit operator rs::Vec3<double>() const
        {
            return Vec3<double>((double) x, (double) y, (double) z);
        }

        explicit operator rs::Vec3<float>() const
        {
            return Vec3<float>((float) x, (float) y, (float) z);
        }

    public:
        Vec3<T>()
            :   rs::Vector<3,T>(),
                x(this->data[0]),
                y(this->data[1]),
                z(this->data[2])
        {
        }

        Vec3<T>(const T& xInit, const T& yInit, const T& zInit)
            :   rs::Vector<3,T>(),
                x(this->data[0]),
                y(this->data[1]),
                z(this->data[2])
        {
            x = xInit;
            y = yInit;
            z = zInit;
        }
    };

    typedef	Vec3<float>         Vec3f;
    typedef	Vec3<double>        Vec3d;
    typedef Vec3<int>           Vec3i;
}

#endif // RS_VEC3_H
