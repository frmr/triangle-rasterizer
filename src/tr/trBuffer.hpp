#pragma once

#include <algorithm>
#include <memory>
#include <vector>

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
			m_floatHeight(float(height)),
			m_maxSize(std::numeric_limits<size_t>::max())
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

		T getAt(float u, float v, const bool filter, const TextureWrappingMode textureWrappingMode) const
		{
			if (textureWrappingMode == TextureWrappingMode::Clamp)
			{
				constexpr float upperLimit = 1.0f - std::numeric_limits<float>::epsilon();

				u = std::clamp(u, 0.0f, upperLimit);
				v = std::clamp(v, 0.0f, upperLimit);
			}
			else
			{
				u -= fastFloor(u);
				v -= fastFloor(v);
			}

			u *= m_floatWidth;
			v *= m_floatHeight;

			if (filter)
			{
				u -= 0.5f;
				v -= 0.5f;

				const float  uFloor    = std::floor(u);
				const float  vFloor    = std::floor(v);

				size_t       x0        = size_t(uFloor);
				size_t       y0        = size_t(vFloor);

				const float  uDiff     = u - uFloor;
				const float  vDiff     = v - vFloor;

				const float  uOpposite = 1.0f - uDiff;
				const float  vOpposite = 1.0f - vDiff;

				size_t       x1        = x0 + 1;
				size_t       y1        = y0 + 1;

				if (x0 == m_maxSize)
				{
					x0 = m_width - 1;
				}
				else if (x1 == m_width)
				{
					x1 = 0;
				}

				if (y0 == m_maxSize)
				{
					y0 = m_height - 1;
				}
				else if (y1 == m_height)
				{
					y1 = 0;
				}

				const Vector4 topLeft     = (m_data.data() + (y0 * m_width + x0))->ToVector();
				const Vector4 topRight    = (m_data.data() + (y0 * m_width + x1))->ToVector();
				const Vector4 bottomLeft  = (m_data.data() + (y1 * m_width + x0))->ToVector();
				const Vector4 bottomRight = (m_data.data() + (y1 * m_width + x1))->ToVector();

				return (topLeft    * uOpposite + topRight    * uDiff) * vOpposite +
				       (bottomLeft * uOpposite + bottomRight * uDiff) * vDiff;
			}
			else
			{
				return getAt(size_t(u), size_t(v));
			}

			
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
		float fastFloor(const float x) const
		{
			const int xi = int(x);
			return float(x < xi ? xi - 1 : xi);
		}

	private:
		const size_t   m_width;
		const size_t   m_height;
		const float    m_floatWidth;
		const float    m_floatHeight;
		const size_t   m_maxSize;
		std::vector<T> m_data;
	};
}
