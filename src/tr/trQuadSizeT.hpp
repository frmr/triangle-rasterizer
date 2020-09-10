#pragma once

#include <array>

namespace tr
{
	class QuadSizeT
	{
	public:
		           QuadSizeT(const size_t a);
		           QuadSizeT(const size_t a, const size_t b, const size_t c, const size_t d);

		const std::array<size_t, 4>& getData() const;

		QuadSizeT& operator+=(const QuadSizeT& rhs);
		QuadSizeT& operator*=(const QuadSizeT& rhs);

		QuadSizeT  operator+(const QuadSizeT& rhs) const;
		QuadSizeT  operator*(const QuadSizeT& rhs) const;

		size_t get(const size_t index) const;

	private:
		std::array<size_t, 4> m_data;
	};
}