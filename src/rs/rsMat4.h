#ifndef RS_MAT4_H
#define RS_MAT4_H

#include "rsMatrix.h"

namespace rs
{
    template<typename T>
    class Mat4 : public rs::Matrix<4,T>
    {
    public:
        void SetOrthographic(const double left, const double right, const double bottom, const double top, const double near, const double far)
        {
            this->SetZero();
            (*this)[0][0] = 2.0 / (right - left);
            (*this)[1][1] = 2.0 / (top - bottom);
            (*this)[2][2] = 2.0 / (far - near);
            (*this)[0][3] = -((right + left) / (right - left));
            (*this)[1][3] = -((top + bottom) / (top - bottom));
            (*this)[2][3] = -((far + near) / (far - near));
            (*this)[3][3] = 1.0;
        }

        void SetPerspective(const double left, const double right, const double bottom, const double top, const double near, const double far)
        {
            this->SetZero();
            (*this)[0][0] = (2.0 * near) / (right - left);
            (*this)[0][2] = -((right + left) / (right - left));
            (*this)[1][1] = (2.0 * near) / (top - bottom);
            (*this)[1][2] = -((top + bottom) / (top - bottom));
            (*this)[2][2] = (far + near) / (far - near);
            (*this)[2][3] = -((2.0 * far * near) / (far - near));
            (*this)[3][2] = 1.0;
        }

    public:
        Mat4()
            :   rs::Matrix<4,T>()
        {
        }
    };

    typedef Mat4<double> Mat4d;
}

#endif // RS_MAT4_H
