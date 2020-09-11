#include "trColorBuffer.hpp"

tr::ColorBuffer::ColorBuffer() :
	Buffer<Color>(),
	m_quadWidth(int32_t(m_width)),
	m_quadFloatWidth(m_floatWidth),
	m_quadFloatHeight(m_floatHeight),
	m_quadDataPointer(size_t(m_data.data()))
{
}

tr::ColorBuffer::ColorBuffer(const size_t width, const size_t height) :
	Buffer<Color>(width, height),
	m_quadWidth(int32_t(m_width)),
	m_quadFloatWidth(m_floatWidth),
	m_quadFloatHeight(m_floatHeight),
	m_quadDataPointer(size_t(m_data.data()))
{
}

tr::QuadColor tr::ColorBuffer::getAt(const QuadFloat& u, const QuadFloat& v) const
{
	QuadFloat tempU = u;
	QuadFloat tempV = v;

	tempU -= u.floor();
	tempV -= v.floor();

	tempU *= m_floatWidth;
	tempV *= m_floatHeight;

	const QuadInt x = tempU.toQuadInt();
	const QuadInt y = tempV.toQuadInt();

	static const QuadInt colorSize(4);

	return QuadColor(m_quadDataPointer + ((y * m_quadWidth + x) * colorSize).toQuadSizeT());
}
