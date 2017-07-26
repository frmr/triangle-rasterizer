#pragma once

#include <memory>
#include <vector>

namespace tr
{
	template<typename T>
	class Buffer
	{
	public:
		Buffer(const size_t& width, const size_t& height) :
			height(height),
			width(width),
			size(width * height)
		{
			data.resize(size, T());
		}

		T& at(const size_t x, const size_t y)
		{
			return data.at(y * width + x);
		}

		T& at(const double x, const double y, const bool filter)
		{
			//interpolate
			return data.at(0);
		}

		void fill(const T& value)
		{
			for (auto& element : data)
			{
				element = value;
			}
		}

		T getAt(const size_t& x, const size_t& y) const
		{
			return data[y*width+x];
		}

		T getAt(const double& x, const double& y, const bool filter) const
		{
			return data[0];
		}

		T* getData()
		{
			return &data.front();
		}

	public:
		const size_t   height;
		const size_t   width;
		const size_t   size;

	private:
		std::vector<T> data;
	};
}
