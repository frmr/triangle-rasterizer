#ifndef RS_BUFFER_H
#define RS_BUFFER_H

namespace rs
{
    template<typename T>
    class Buffer
    {
    public:
        const size_t    height;
        const size_t    width;

    private:
        T*              data;

    public:
        T* GetData() const
        {
            return data;
        }

        T& Sample(const size_t x, const size_t y)
        {
            return data[y*width+x];
        }

        T& Sample(const double x, const double y, const bool filter)
        {
            //interpolate
            return data[0];
        }

        T& Sample(const size_t index)
        {
            return data[index];
        }

    public:
        Buffer(const size_t width, const size_t height)
            :   height(height),
                width(width)
        {
            data = new T[width*height];
        }

        ~Buffer()
        {
            delete[] data;
        }
    };
}


#endif // RS_BUFFER_H
