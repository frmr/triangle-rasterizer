#ifndef RS_MAT4_H
#define RS_MAT4_H

#include <cmath>
#include <iostream>

#include "rsVec4.h"

namespace rs
{
    template<typename T>
    class Mat4
    {
    public:
        T* const                data;
        static constexpr size_t size = 4;

    public:
        template<typename U, typename V, typename W>
        static void Multiply(const rs::Mat4<U>& lhs, const rs::Mat4<V>& rhs, rs::Mat4<W>& result)
        {
            for (size_t row = 0; row < size; ++row)
            {
                for (size_t col = 0; col < size; ++col)
                {
                    for (size_t inner = 0; inner < size; ++inner)
                    {
                        result[row][col] += (W) lhs[row][inner] * (W) rhs[inner][col];
                    }
                }
            }
        }

        template<typename U, typename V, typename W>
        static void Multiply(const rs::Mat4<U>& lhs, const rs::Vec4<V>& rhs, rs::Vec4<W>& result)
        {
            for (size_t row = 0; row < size; ++row)
            {
                for (size_t col = 0; col < size; ++col)
                {
                    result[row] += (W) lhs[row][col] * (W) rhs.GetAt(col);
                }
            }
        }

         void Print() const
        {
            for (size_t i = 0; i < size; ++i)
            {
                for (size_t j = 0; j < size; ++j)
                {
                    std::cout << (*this)[i][j] << "\t";
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }

        void RotateX(const double& radians)
        {
            rs::Mat4<double> rotationMatrix;
            rotationMatrix[0][0] = 1.0;
            rotationMatrix[1][1] = cos(radians);
            rotationMatrix[1][2] = -sin(radians);
            rotationMatrix[2][1] = sin(radians);
            rotationMatrix[2][2] = cos(radians);
            rotationMatrix[3][3] = 1.0;
            (*this) *= rotationMatrix;
        }

        void RotateY(const double& radians)
        {
            rs::Mat4<double> rotationMatrix;
            rotationMatrix[0][0] = cos(radians);
            rotationMatrix[0][2] = sin(radians);
            rotationMatrix[1][1] = 1.0;
            rotationMatrix[2][0] = -sin(radians);
            rotationMatrix[2][2] = cos(radians);
            rotationMatrix[3][3] = 1.0;

            (*this) *= rotationMatrix;
        }

        void RotateZ(const double& radians)
        {
            rs::Mat4<double> rotationMatrix;
            rotationMatrix[0][0] = cos(radians);
            rotationMatrix[0][1] = -sin(radians);
            rotationMatrix[1][0] = sin(radians);
            rotationMatrix[1][1] = cos(radians);
            rotationMatrix[2][2] = 1.0;
            rotationMatrix[3][3] = 1.0;

            (*this) *= rotationMatrix;
        }

        void SetIdentity()
        {
            SetZero();
            for (size_t i = 0; i < size; ++i)
            {
                (*this)[i][i] = (T) 1;
            }
        }

        void SetOrthographic(const T& left, const T& right, const T& bottom, const T& top, const T& near, const T& far)
        {
            SetZero();
            (*this)[0][0] = (T) 2 / (right - left);
            (*this)[1][1] = (T) 2 / (top - bottom);
            (*this)[2][2] = (T) 2 / (far - near);
            (*this)[0][3] = -((right + left) / (right - left));
            (*this)[1][3] = -((top + bottom) / (top - bottom));
            (*this)[2][3] = -((far + near) / (far - near));
            (*this)[3][3] = (T) 1;
        }

        void SetPerspective(const T& left, const T& right, const T& bottom, const T& top, const T& near, const T& far)
        {
            SetZero();
            (*this)[0][0] = ((T) 2 * near) / (right - left);
            (*this)[0][2] = -((right + left) / (right - left));
            (*this)[1][1] = ((T) 2 * near) / (top - bottom);
            (*this)[1][2] = -((top + bottom) / (top - bottom));
            (*this)[2][2] = (far + near) / (far - near);
            (*this)[2][3] = -(((T) 2 * far * near) / (far - near));
            (*this)[3][2] = (T) 1;
        }

        void SetZero()
        {
            for (size_t i = 0; i < size*size; ++i)
            {
                data[i] = (T) 0;
            }
        }

        void Translate(const T& dx, const T& dy, const T& dz)
        {
            (*this)[0][3] += dx;
            (*this)[1][3] += dy;
            (*this)[2][3] += dz;
        }

    public:
        T* operator[](const size_t i) const
        {
            return &(data[size*i]);
        }

        template<typename U>
        rs::Mat4<T>& operator*=(const rs::Mat4<U>& rhs)
        {
            const rs::Mat4<T> original(data);
            Multiply(original, rhs, *this);
            return *this;
        }

        template<typename U>
        rs::Mat4<T> operator*(const rs::Mat4<U>& rhs) const
        {
            rs::Mat4<T> result;
            Multiply(*this, rhs, result);
            return result;
        }

        template<typename U>
        rs::Vec4<T> operator*(const Vec4<U>& rhs) const
        {
            Vec4<T> result;
            Multiply(*this, rhs, result);
            return result;
        }

        Mat4<T>& operator=(const Mat4<T>& matA)
        {
            for (int i = 0; i < 16; ++i)
            {
                data[i] = matA.data[i];
            }
        }

        Mat4()
            :   data(new T[size*size])
        {
            SetZero();
        }

        template<typename U>
        Mat4(const U* const dataInit)
            :   data(new T[size*size])
        {
            for (size_t i = 0; i < size*size; ++i)
            {
                data[i] = (T) dataInit[i];
            }
        }

        ~Mat4()
        {
            delete[] data;
        }
    };

    typedef Mat4<double> Mat4d;
}

#endif // RS_MAT4_H
