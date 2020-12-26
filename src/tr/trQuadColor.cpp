#include "trQuadColor.hpp"
#include "trQuadInt.hpp"
#include "trQuadFloat.hpp"
#include <cassert>

// Tried static in function scope, but it's slower
// TODO: Find somewhere better to put this
const tr::QuadInt byteMask(0xFF);

tr::QuadColor::QuadColor(const QuadFloat& r, const QuadFloat& g, const QuadFloat& b, const QuadFloat& a) :
	m_r(r),
	m_g(g),
	m_b(b),
	m_a(a)
{
}

tr::QuadColor::QuadColor(const Color* const baseAddress, const QuadInt& offsets, const QuadMask& mask) :
	m_r(0.0f),
	m_g(0.0f),
	m_b(0.0f),
	m_a(0.0f)
{
	assert(sizeof(tr::Color) == sizeof(uint32_t));

	const QuadInt colorsAsInts = offsets.gatherIntsAtOffsets(reinterpret_cast<const int32_t*>(baseAddress), mask);

	QuadInt bValues = (colorsAsInts      ) & byteMask;
	QuadInt gValues = (colorsAsInts >>  8) & byteMask;
	QuadInt rValues = (colorsAsInts >> 16) & byteMask;
	QuadInt aValues = (colorsAsInts >> 24) & byteMask;

	m_r = rValues.convertToQuadFloat();
	m_g = gValues.convertToQuadFloat();
	m_b = bValues.convertToQuadFloat();
	m_a = aValues.convertToQuadFloat();
}

tr::QuadColor::QuadColor(const Color* const address, const QuadMask& mask) :
	m_r(0.0f),
	m_g(0.0f),
	m_b(0.0f),
	m_a(0.0f)
{
	assert(sizeof(tr::Color) == sizeof(uint32_t));

	const QuadInt colorsAsInts(reinterpret_cast<const int32_t*>(address), mask);

	QuadInt bValues = (colorsAsInts      ) & byteMask;
	QuadInt gValues = (colorsAsInts >>  8) & byteMask;
	QuadInt rValues = (colorsAsInts >> 16) & byteMask;
	QuadInt aValues = (colorsAsInts >> 24) & byteMask;

	m_r = rValues.convertToQuadFloat();
	m_g = gValues.convertToQuadFloat();
	m_b = bValues.convertToQuadFloat();
	m_a = aValues.convertToQuadFloat();
}

void tr::QuadColor::write(Color* const pointer, const QuadMask& mask) const
{
	int32_t* const  intPointer = reinterpret_cast<int32_t* const>(pointer);

	const QuadFloat roundedR = m_r.round();
	const QuadFloat roundedG = m_g.round();
	const QuadFloat roundedB = m_b.round();
	const QuadFloat roundedA = m_a.round();

	QuadInt intR = roundedR.convertToQuadInt();
	QuadInt intG = roundedG.convertToQuadInt();
	QuadInt intB = roundedB.convertToQuadInt();
	QuadInt intA = roundedA.convertToQuadInt();

	intG <<= 8;
	intR <<= 16;
	intA <<= 24;

	(intR | intG | intB | intA).write(intPointer, mask);
}

const tr::QuadVec3 tr::QuadColor::toVec3() const
{
	return QuadVec3(m_r, m_g, m_b);
}

const tr::QuadFloat& tr::QuadColor::getAlpha() const
{
	return m_a;
}

tr::QuadColor& tr::QuadColor::operator+=(const QuadColor& rhs)
{
	m_r += rhs.m_r;
	m_g += rhs.m_g;
	m_b += rhs.m_b;
	m_a += rhs.m_a;

	return *this;
}

tr::QuadColor& tr::QuadColor::operator*=(const QuadFloat& rhs)
{
	m_r *= rhs;
	m_g *= rhs;
	m_b *= rhs;
	m_a *= rhs;

	return *this;
}

tr::QuadColor tr::QuadColor::operator+(const QuadColor& rhs) const
{
	return QuadColor(
		m_r + rhs.m_r,
		m_g + rhs.m_g,
		m_b + rhs.m_b,
		m_a + rhs.m_a
	);
}

tr::QuadColor tr::QuadColor::operator*(const QuadFloat& rhs) const
{
	return QuadColor(
		m_r * rhs,
		m_g * rhs,
		m_b * rhs,
		m_a * rhs
	);
}