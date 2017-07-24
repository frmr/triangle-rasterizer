#pragma once

namespace tr
{
	template<typename T>
	class Buffer
	{
	public:
		const size_t    height;
		const size_t    width;
		const size_t    size;
		T* const        data;

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

		void Fill(const T& value)
		{
			for (int i = 0; i < size; ++i)
			{
				data[i] = value;
			}
		}

		T GetAt(const size_t& x, const size_t& y) const
		{
			return data[y*width+x];
		}

		T GetAt(const double& x, const double& y, const bool filter) const
		{
			return data[0];
		}

	public:
		Buffer(const size_t& width, const size_t& height) :
			height(height),
			width(width),
			size(width * height),
			data(new T[size])
		{
			Fill(T());
		}

		~Buffer()
		{
			delete[] data;
		}
	};
}
