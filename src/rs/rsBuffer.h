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

        T& At(const size_t x, const size_t y)
        {
            return data[y*width+x];
        }

        T& At(const double x, const double y, const bool filter)
        {
            //interpolate
            return data[0];
        }

        T& At(const size_t index)
        {
            return data[index];
        }

        T GetAt(const size_t& x, const size_t& y) const
        {
            return data[y*width+x];
        }

        T GetAt(const double& x, const double& y, const bool filter) const
        {
            return data[0];
        }

        T GetAt(const size_t& index) const
        {
            return data[index];
        }

        T* GetData() const
        {
            return data;
        }

    public:
        Buffer(const size_t& width, const size_t& height)
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
