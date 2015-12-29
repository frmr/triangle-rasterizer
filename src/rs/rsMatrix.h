#ifndef RS_MATRIX_H
#define RS_MATRIX_H

#include <iostream>
#include "rsVector.h"

namespace rs
{
    template<size_t S, typename T>
    class Matrix
    {
    public:
        T* const data;

    public:
        T* operator[](const size_t i) const
        {
            return &data[S*i];
        }

        template<typename U, typename V, typename W>
        static void Multiply(const rs::Matrix<S,U>& lhs, const rs::Matrix<S,V>& rhs, rs::Matrix<S,W>& result)
        {
            for (size_t row = 0; row < S; ++row)
            {
                for (size_t col = 0; col < S; ++col)
                {
                    for (size_t inner = 0; inner < S; ++inner)
                    {
                        result[row][col] += (W) lhs[row][inner] * (W) rhs[inner][col];
                    }
                }
            }
        }

        template<typename U, typename V, typename W>
        static void Multiply(const rs::Matrix<S,U>& lhs, const Vector<S,V>& rhs, Vector<S,W>& result)
        {
            for (size_t row = 0; row < S; ++row)
            {
                for (size_t col = 0; col < S; ++col)
                {
                    result[row] += (W) lhs[row][col] * (W) rhs.GetAt(col);
                }
            }
        }

        void SetIdentity()
        {
            SetZero();
            for (int i = 0; i < S; ++i)
            {
                (*this)[i][i] = (T) 1;
            }
        }

        void SetZero()
        {
            for (int i = 0; i < S*S; ++i)
            {
                data[i] = (T) 0;
            }
        }

        template<typename U>
        rs::Matrix<S,T>& operator*=(const rs::Matrix<S,U>& rhs)
        {
            const rs::Matrix<S,T> original(*this);
            Multiply(original, rhs, *this);
            return *this;
        }

        template<typename U>
        rs::Matrix<S,T> operator*(const rs::Matrix<S,U>& rhs) const
        {
            rs::Matrix<S,T> result;
            Multiply(*this, rhs, result);
            return result;
        }

        template<typename U>
        Vector<S,T> operator*(const Vector<S,U>& rhs) const
        {
            Vector<S,T> result;
            Multiply(*this, rhs, result);
            return result;
        }

        void Print() const
        {
            for (size_t i = 0; i < S; ++i)
            {
                for (size_t j = 0; j < S; ++j)
                {
                    std::cout << (*this)[i][j] << "\t";
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }

        Matrix()
            :   data(new T[S*S])
        {
            SetZero();
        }

        template<typename U>
        Matrix(const rs::Matrix<S,U>& matA)
            :   data(new T[S*S])
        {
            for (size_t i = 0; i < S*S; ++i)
            {
                data[i] = (T) matA.data[i];
            }
        }

        template<typename U>
        Matrix(const U* const dataInit)
            :   data(new T[S*S])
        {
            for (size_t i = 0; i < S*S; ++i)
            {
                data[i] = (T) dataInit[i];
            }
        }

        ~Matrix()
        {
            delete[] data;
        }
    };
}

#endif
