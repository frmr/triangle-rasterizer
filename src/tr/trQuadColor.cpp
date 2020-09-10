#include "trQuadColor.hpp"
#include <cassert>

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
	m_r(0.0f),
	m_g(0.0f),
	m_b(0.0f),
	m_a(0.0f)
{
	constexpr std::array<float, 256> floats = generateConversionTable();

	assert(sizeof(tr::Color) == sizeof(uint32_t));

	std::array<uint32_t, 4> ints{
		*(reinterpret_cast<const uint32_t*>(pointers.get(0))),
		*(reinterpret_cast<const uint32_t*>(pointers.get(1))),
		*(reinterpret_cast<const uint32_t*>(pointers.get(2))),
		*(reinterpret_cast<const uint32_t*>(pointers.get(3)))
	};

	m_r = QuadFloat(floats[ints[0] >> 24       ], floats[ints[1] >> 24       ], floats[ints[2] >> 24       ], floats[ints[3] >> 24       ]);
	m_g = QuadFloat(floats[ints[0] >> 16 & 0xFF], floats[ints[1] >> 16 & 0xFF], floats[ints[2] >> 16 & 0xFF], floats[ints[3] >> 16 & 0xFF]);
	m_b = QuadFloat(floats[ints[0] >>  8 & 0xFF], floats[ints[1] >>  8 & 0xFF], floats[ints[2] >>  8 & 0xFF], floats[ints[3] >>  8 & 0xFF]);
	m_a = QuadFloat(floats[ints[0] >>  0 & 0xFF], floats[ints[1] >>  0 & 0xFF], floats[ints[2] >>  0 & 0xFF], floats[ints[3] >>  0 & 0xFF]);
}

void tr::QuadColor::write(Color* const pointer, const QuadMask& mask) const
{
	std::array<uint32_t, 4> ints;

	for (size_t i = 0; i < ints.size(); ++i)
	{
		const float r = m_r.get(i);
		const float g = m_g.get(i);
		const float b = m_b.get(i);
		const float a = m_a.get(i);

		ints[i] = uint32_t(std::lround(r) << 24 | std::lround(g) << 16 | std::lround(b) << 8 | std::lround(a));
	}

	uint32_t* const intPointer = reinterpret_cast<uint32_t* const>(pointer);

	for (size_t i = 0; i < ints.size(); ++i)
	{
		if (mask.get(i))
		{
			*(intPointer + i) = ints[i];
		}
	}
}