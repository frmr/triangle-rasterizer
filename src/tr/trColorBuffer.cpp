#include "trColorBuffer.hpp"

tr::ColorBuffer::ColorBuffer() :
	Buffer<Color>(),
	m_quadWidth(int32_t(m_width)),
	m_quadFloatWidth(m_floatWidth),
	m_quadFloatHeight(m_floatHeight)
{
}

tr::ColorBuffer::ColorBuffer(const size_t width, const size_t height) :
	Buffer<Color>(width, height),
	m_quadWidth(int32_t(m_width)),
	m_quadFloatWidth(m_floatWidth),
	m_quadFloatHeight(m_floatHeight)
{
}

tr::QuadColor tr::ColorBuffer::getAt(const QuadFloat& u, const QuadFloat& v, const QuadMask& mask) const
{
	QuadFloat tempU = u;
	QuadFloat tempV = v;

	tempU -= u.floor();
	tempV -= v.floor();

	tempU *= m_floatWidth;
	tempV *= m_floatHeight;

	const QuadInt x = tempU.convertToQuadInt();
	const QuadInt y = tempV.convertToQuadInt();

	return QuadColor(m_data.data(), y * m_quadWidth + x, mask);
}
