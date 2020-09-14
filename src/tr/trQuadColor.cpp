#include "trQuadColor.hpp"
#include "trQuadInt.hpp"
#include "trQuadFloat.hpp"
#include <cassert>

// Tried static in function scope, but it's slower
// TODO: Find somewhere better to put this
const tr::QuadInt byteMask(0xFF);

tr::QuadColor::QuadColor(const Color* const baseAddress, const QuadInt& offsets, const QuadMask& mask) :
	m_r(0.0f), // TODO: Don't initialize?
	m_g(0.0f),
	m_b(0.0f),
	m_a(0.0f)
{
	assert(sizeof(tr::Color) == sizeof(uint32_t));

	const QuadInt colorsAsInts = offsets.gatherIntsAtOffsets(reinterpret_cast<const int32_t*>(baseAddress), mask);

	QuadInt rValues = (colorsAsInts >> 24);
	QuadInt gValues = (colorsAsInts >> 16) & byteMask;
	QuadInt bValues = (colorsAsInts >>  8) & byteMask;
	QuadInt aValues = (colorsAsInts      ) & byteMask;

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

	intR <<= 24;
	intG <<= 16;
	intB <<=  8;

	(intR | intG | intB | intA).write(intPointer, mask);
}