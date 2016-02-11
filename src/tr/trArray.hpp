#ifndef TR_ARRAY_HPP
#define TR_ARRAY_HPP

#include <cstddef>
#include <vector>

using std::vector;

namespace tr
{
    template<typename T>
    class Array
    {
    private:
        vector<size_t>  dims;
        T*              data;

    public:
        T& At(const size_t i)
        {
            return data[i];
        }

        T& At(const size_t i, const size_t j)
        {
            return data[i + j * dims[0]];
        }

        T& At(const size_t i, const size_t j, const size_t k)
        {
            //return data[ i + j * dims[0] + k * dims[0] * dims[1] ];
            return data[dims[2] * (j * dims[0] + i) + k]; //TODO: Make this kind of access consistent for all dimensions
        }

        void Delete()
        {
            if (data != NULL)
            {
                delete[] data;
                data = NULL;
            }
        }

        T GetAt(const size_t i) const
        {
            return data[i];
        }

        T GetAt(const size_t i, const size_t j) const
        {
            return data[i + j * dims[0]];
        }

        T GetAt(const size_t i, const size_t j, const size_t k) const
        {
            //return data[ i + j * dims[0] + k * dims[0] * dims[1] ];
            return data[dims[2] * (j * dims[0] + i) + k];
        }

        T* GetData() const
        {
            return data;
        }

        void SetSize(const size_t xDim, const size_t yDim, const size_t zDim)
        {
            dims.clear();
            dims.push_back(xDim);
            dims.push_back(yDim);
            dims.push_back(zDim);
            Delete();
            data = new T[xDim * yDim * zDim];
        }

    public:
        explicit Array(const size_t xDim)
            :   dims({xDim}),
                data(new T[xDim])
        {
        }

        Array(const size_t xDim, const size_t yDim)
            :   dims({xDim, yDim}),
                data(new T[xDim*yDim])
        {
        }

        Array(const size_t xDim, const size_t yDim, const size_t zDim)
            :   dims({xDim, yDim, zDim}),
                data(new T[xDim * yDim * zDim])
        {
        }

        ~Array()
        {
            Delete();
        }
    };
}

#endif
