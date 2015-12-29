#ifndef RS_VECTOR_H
#define RS_VECTOR_H

#include <iostream>

namespace rs
{
    template<size_t S, typename T>
    class Vector
    {
    public:
        T* const data;

    public:
        T& operator[](const size_t i)
        {
            return data[i];
        }

        T GetAt(const size_t i) const
        {
            return data[i];
        }

        void Print() const
        {
            for (size_t i = 0; i < S; ++i)
            {
                std::cout << data[i] << "\t";
            }
            std::cout << std::endl;
        }

        Vector()
            :   data(new T[S])
        {
            for (size_t i = 0; i < S; ++i)
            {
                data[i] = (T) 0;
            }
        }

//        Vector(Vector<S,T>& vecA)
//            :   data(new T[S])
//        {
//            for (int i = 0; i < S; ++i)
//            {
//                data[i] = (T) vecA.data[i];
//            }
//        }

        ~Vector()
        {
            delete[] data;
        }
    };
}

#endif // RS_VECTOR_H
