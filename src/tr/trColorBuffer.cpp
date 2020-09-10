#include "trColorBuffer.hpp"

tr::ColorBuffer::ColorBuffer() :
	Buffer<Color>(),
	m_quadWidth(m_width),
	m_quadFloatWidth(m_floatWidth),
	m_quadFloatHeight(m_floatHeight),
	m_quadDataPointer(size_t(m_data.data()))
{
}

tr::ColorBuffer::ColorBuffer(const size_t width, const size_t height) :
	Buffer<Color>(width, height),
	m_quadWidth(m_width),
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

	// Will need to convert from float to int32, then to int64 for the maths

	const QuadSizeT x = tempU.toQuadSizeT();
	const QuadSizeT y = tempV.toQuadSizeT();

	return QuadColor(m_quadDataPointer + (y * m_quadWidth + x) * sizeof(tr::Color));
}
