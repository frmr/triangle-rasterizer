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
			m_width(width),
			m_height(height),
			m_size(width * height)
		{
			m_data.resize(m_size, T());
		}

		T& at(const size_t x, const size_t y)
		{
			return m_data.at(y * m_width + x);
		}

		T& at(const double x, const double y, const bool filter)
		{
			//interpolate
			return m_data.at(0);
		}

		void fill(const T& value)
		{
			for (auto& element : m_data)
			{
				element = value;
			}
		}

		T getAt(const size_t& x, const size_t& y) const
		{
			return m_data[y*width+x];
		}

		T getAt(const double& x, const double& y, const bool filter) const
		{
			return m_data[0];
		}

		T* getData()
		{
			return &m_data.front();
		}

		size_t getWidth() const
		{
			return m_Width;
		}

		size_t getHeight() const
		{
			return m_Height;
		}

	private:
		const size_t   m_width;
		const size_t   m_height;
		const size_t   m_size;
		std::vector<T> m_data;
	};
}
