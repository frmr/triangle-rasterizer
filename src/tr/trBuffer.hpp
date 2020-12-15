#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include "trQuadFloat.hpp"
#include "trTextureWrappingMode.hpp"
#include "..//matrix/Vectors.h"

namespace tr
{
	template<typename T>
	class Buffer
	{
	public:
		Buffer() :
			m_width(0),
			m_height(0),
			m_floatWidth(0.0f),
			m_floatHeight(0.0f)
		{
		}

		Buffer(const size_t width, const size_t height) :
			m_width(int(width)),
			m_height(int(height)),
			m_floatWidth(float(width)),
			m_floatHeight(float(height))
		{
			m_data.resize(width * height, T());
		}

		T& at(const size_t x, const size_t y)
		{
			return m_data.at(y * m_width + x);
		}

		void fill(const T& value)
		{
			std::fill(m_data.begin(), m_data.end(), value);
		}

		T getAt(const size_t x, const size_t y) const
		{
			return m_data[y * m_width + x];
		}

		T* getData()
		{
			return m_data.data();
		}

		int getWidth() const
		{
			return m_width;
		}

		int getHeight() const
		{
			return m_height;
		}

		float getFloatWidth() const
		{
			return m_floatWidth;
		}

		float getFloatHeight() const
		{
			return m_floatHeight;
		}

	protected:
		int            m_width;
		int            m_height;
		float          m_floatWidth;
		float          m_floatHeight;
		std::vector<T> m_data;
	};
}
