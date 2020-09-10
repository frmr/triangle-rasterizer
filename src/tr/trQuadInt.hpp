#pragma once

#include <array>
#include <cstdint>

namespace tr
{
	class QuadInt
	{
	public:
		                              QuadInt(const int32_t a);
		                              QuadInt(const int32_t a, const int32_t b, const int32_t c, const int32_t d);

		const std::array<int32_t, 4>& getData() const;

		QuadInt&                      operator+=(const QuadInt& rhs);
		QuadInt&                      operator*=(const QuadInt& rhs);

		QuadInt                       operator+(const QuadInt& rhs) const;
		QuadInt                       operator*(const QuadInt& rhs) const;

	private:
		std::array<int32_t, 4>        m_data;
	};
}