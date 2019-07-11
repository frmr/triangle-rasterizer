#pragma once

#include <algorithm>
#include <memory>
#include <vector>

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

		void fill(const T& value)
		{
			std::fill(m_data.begin(), m_data.end(), value);
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

				const float  uFloor    = std::floorf(u);
				const float  vFloor    = std::floorf(v);

				size_t       x0        = size_t(uFloor);
				size_t       y0        = size_t(vFloor);

				const float  uDiff     = u - uFloor;
				const float  vDiff     = v - vFloor;

				const float  uOpposite = 1.0f - uDiff;
				const float  vOpposite = 1.0f - vDiff;

				size_t       x1        = x0 + 1;
				size_t       y1        = y0 + 1;

				if (x0 == s_maxSize)
				{
					x0 = (textureWrappingMode == TextureWrappingMode::Clamp) ? 0 : m_width - 1;
				}
				else if (x1 == m_width)
				{
					x1 = (textureWrappingMode == TextureWrappingMode::Clamp) ? x0 : 0;
				}

				if (y0 == s_maxSize)
				{
					y0 = (textureWrappingMode == TextureWrappingMode::Clamp) ? 0 : m_height - 1;
				}
				else if (y1 == m_height)
				{
					y1 = (textureWrappingMode == TextureWrappingMode::Clamp) ? y0 : 0;
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

		float getFloatWidth() const
		{
			return m_floatWidth;
		}

		float getFloatHeight() const
		{
			return m_floatHeight;
		}

	protected:
		static float fastFloor(const float x)
		{
			const int32_t xi = int32_t(x);
			return float(x < xi ? xi - 1 : xi);
		}

	protected:
		static constexpr size_t s_maxSize = std::numeric_limits<size_t>::max();
		size_t                  m_width;
		size_t                  m_height;
		float                   m_floatWidth;
		float                   m_floatHeight;
		std::vector<T>          m_data;
	};
}
