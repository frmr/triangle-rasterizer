#include "trQuadColor.hpp"
#include <cassert>

// TODO: Might be needed elsewhere; consider moving into its own file
constexpr std::array<float, 256> tr::generateConversionTable()
{
	std::array<float, 256> floats{};

	for (uint16_t i = 0; i < 256; ++i)
	{
		floats[uint8_t(i)] = float(i);
	}

	return floats;
}

tr::QuadColor::QuadColor(const QuadSizeT& pointers) :
	m_r(0.0f), // TODO: Don't initialize
	m_g(0.0f),
	m_b(0.0f),
	m_a(0.0f)
{
	constexpr std::array<float, 256> floats = generateConversionTable();

	assert(sizeof(tr::Color) == sizeof(uint32_t));

	const std::array<size_t, 4> pointerArray = pointers.toArray();

	const std::array<uint32_t, 4> ints{
		*(reinterpret_cast<const uint32_t*>(pointerArray[0])),
		*(reinterpret_cast<const uint32_t*>(pointerArray[1])),
		*(reinterpret_cast<const uint32_t*>(pointerArray[2])),
		*(reinterpret_cast<const uint32_t*>(pointerArray[3]))
	};

	// TODO
	// Loads all reds into one QuadInt
	// Shift red >> 24
	// Gather floats by calculating address from floats[] + red byte

	m_r = QuadFloat(floats[ints[0] >> 24       ], floats[ints[1] >> 24       ], floats[ints[2] >> 24       ], floats[ints[3] >> 24       ]);
	m_g = QuadFloat(floats[ints[0] >> 16 & 0xFF], floats[ints[1] >> 16 & 0xFF], floats[ints[2] >> 16 & 0xFF], floats[ints[3] >> 16 & 0xFF]);
	m_b = QuadFloat(floats[ints[0] >>  8 & 0xFF], floats[ints[1] >>  8 & 0xFF], floats[ints[2] >>  8 & 0xFF], floats[ints[3] >>  8 & 0xFF]);
	m_a = QuadFloat(floats[ints[0] >>  0 & 0xFF], floats[ints[1] >>  0 & 0xFF], floats[ints[2] >>  0 & 0xFF], floats[ints[3] >>  0 & 0xFF]);
}

void tr::QuadColor::write(Color* const pointer, const QuadMask& mask) const
{
	int32_t* const  intPointer = reinterpret_cast<int32_t* const>(pointer);

	const QuadFloat roundedR = m_r.round();
	const QuadFloat roundedG = m_g.round();
	const QuadFloat roundedB = m_b.round();
	const QuadFloat roundedA = m_a.round();

	QuadInt intR = roundedR.toQuadInt();
	QuadInt intG = roundedG.toQuadInt();
	QuadInt intB = roundedB.toQuadInt();
	QuadInt intA = roundedA.toQuadInt();

	intR <<= 24;
	intG <<= 16;
	intB <<=  8;

	(intR | intG | intB | intA).write(intPointer, mask);
}