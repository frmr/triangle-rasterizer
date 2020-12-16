#include "trColorBuffer.hpp"
#include "trQuadVec4.hpp"

const tr::QuadFloat upperLimit(1.0f - std::numeric_limits<float>::epsilon());
const tr::QuadFloat allZeroesFloat(0.0f);
const tr::QuadFloat allOnesFloat(1.0f);
const tr::QuadFloat pointFive(0.5f);
const tr::QuadInt   allZeroesInt(0);
const tr::QuadInt   allOnesInt(1);
const tr::QuadInt   allNegativeOnesInt(-1);

tr::ColorBuffer::ColorBuffer() :
	Buffer<Color>(),
	m_quadWidth(m_width),
	m_quadHeight(m_height),
	m_quadFloatWidth(m_floatWidth),
	m_quadFloatHeight(m_floatHeight)
{
}

tr::ColorBuffer::ColorBuffer(const size_t width, const size_t height) :
	Buffer<Color>(width, height),
	m_quadWidth(m_width),
	m_quadHeight(m_height),
	m_quadFloatWidth(m_floatWidth),
	m_quadFloatHeight(m_floatHeight)
{
}

tr::QuadColor tr::ColorBuffer::getAt(const QuadInt& x, const QuadInt& y, const QuadMask& mask) const
{
	return QuadColor(m_data.data(), y * m_quadWidth + x, mask);
}

tr::QuadColor tr::ColorBuffer::getAt(const QuadFloat& u, const QuadFloat& v, const QuadMask& mask) const
{
	QuadFloat tempU = u;
	QuadFloat tempV = v;

	tempU -= u.floor();
	tempV -= v.floor();

	tempU *= m_floatWidth;
	tempV *= m_floatHeight;

	return getAt(tempU.convertToQuadInt(), tempV.convertToQuadInt(), mask);
}

tr::QuadColor tr::ColorBuffer::getAt(QuadFloat u, QuadFloat v, const bool filter, const TextureWrappingMode textureWrappingMode, const QuadMask& mask) const
{
	if (textureWrappingMode == TextureWrappingMode::Clamp)
	{
		u = u.min(upperLimit).max(allZeroesFloat);
		v = v.min(upperLimit).max(allZeroesFloat);
	}
	else
	{
		u -= u.floor();
		v -= v.floor();
	}

	u *= m_quadFloatWidth;
	v *= m_quadFloatHeight;

	if (filter)
	{
		u -= pointFive;
		v -= pointFive;

		const QuadFloat uFloor    = u.floor();
		const QuadFloat vFloor    = v.floor();

		QuadInt         x0        = uFloor.convertToQuadInt();
		QuadInt         y0        = vFloor.convertToQuadInt();

		const QuadFloat uDiff     = u - uFloor;
		const QuadFloat vDiff     = v - vFloor;

		const QuadFloat uOpposite = allOnesFloat - uDiff;
		const QuadFloat vOpposite = allOnesFloat - vDiff;

		QuadInt         x1        = x0 + allOnesInt;
		QuadInt         y1        = y0 + allOnesInt;

		const QuadMask negativeOnesMaskX0 = x0.equal(allNegativeOnesInt);
		const QuadMask negativeOnesMaskY0 = y0.equal(allNegativeOnesInt);
		const QuadMask widthMaskX1        = x1.equal(m_width);
		const QuadMask heightMaskY1       = y1.equal(m_height);

		x0 = x0.maskedCopy(textureWrappingMode == TextureWrappingMode::Clamp ? allZeroesInt : m_quadWidth - allOnesInt,  negativeOnesMaskX0);
		x1 = x1.maskedCopy(textureWrappingMode == TextureWrappingMode::Clamp ? x0           : allZeroesInt,              widthMaskX1       );

		y0 = y0.maskedCopy(textureWrappingMode == TextureWrappingMode::Clamp ? allZeroesInt : m_quadHeight - allOnesInt, negativeOnesMaskY0);
		y1 = y1.maskedCopy(textureWrappingMode == TextureWrappingMode::Clamp ? y0           : allZeroesInt,              heightMaskY1);

		const QuadVec4 topLeft     = QuadColor(m_data.data(), y0 * m_width + x0, mask).toVector();
		const QuadVec4 topRight    = QuadColor(m_data.data(), y0 * m_width + x1, mask).toVector();
		const QuadVec4 bottomLeft  = QuadColor(m_data.data(), y1 * m_width + x0, mask).toVector();
		const QuadVec4 bottomRight = QuadColor(m_data.data(), y1 * m_width + x1, mask).toVector();

		return QuadColor((topLeft    * uOpposite + topRight    * uDiff) * vOpposite +
		                 (bottomLeft * uOpposite + bottomRight * uDiff) * vDiff);
	}
	else
	{
		return getAt(u.convertToQuadInt(), v.convertToQuadInt(), mask);
	}
}