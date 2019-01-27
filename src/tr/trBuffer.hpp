#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include "trFixed.hpp"
#include "trTextureWrappingMode.hpp"

namespace tr
{
	template<typename T>
	class Buffer
	{
	public:
		Buffer(const size_t& width, const size_t& height) :
			m_width(width),
			m_height(height),
			m_floatWidth(float(width)),
			m_floatHeight(float(height))
		{
			m_data.resize(width * height, T());
		}

		T& at(const size_t x, const size_t y)
		{
			return m_data.at(y * m_width + x);
		}

		T& at(const double x, const double y, const bool filter)
		{
			//TODO: interpolate
			return m_data.at(0);
		}

		void fill(const T& value)
		{
			//TODO: optimize
			for (auto& element : m_data)
			{
				element = value;
			}
		}

		T getAt(const size_t& x, const size_t& y) const
		{
			return m_data[y * m_width + x];
		}

		T getAt(Fixed x, Fixed y, const bool filter, const TextureWrappingMode textureWrappingMode) const
		{
			//if (textureWrappingMode == TextureWrappingMode::Clamp)
			//{
			//	//constexpr float upperLimit = 1.0f - std::numeric_limits<float>::epsilon();
			//	constexpr Fixed upperLimit(0.999f);
			//
			//	x = std::clamp(x, Fixed(0), upperLimit);
			//	y = std::clamp(y, Fixed(0), upperLimit);
			//}
			//else
			//{
			//	//x -= std::floorf(x);
			//	//y -= std::floorf(y);
			//}
			//
			//return getAt(size_t(x * m_floatWidth), size_t(y * m_floatHeight));

			return getAt(0, 0);
		}

		T* getData()
		{
			return m_data.data();
		}

		size_t getWidth() const
		{
			return m_width;
		}

		size_t getHeight() const
		{
			return m_height;
		}

	private:
		const size_t   m_width;
		const size_t   m_height;
		const float    m_floatWidth;
		const float    m_floatHeight;
		std::vector<T> m_data;
	};
}
