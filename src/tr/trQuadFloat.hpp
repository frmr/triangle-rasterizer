#pragma once

#include "trQuadMask.hpp"
#include "trQuadSizeT.hpp"
#include <array>

namespace tr
{
	class QuadFloat
	{
	public:
		                     QuadFloat(const float a);
		                     QuadFloat(const float a, const float b, const float c, const float d);
		                     QuadFloat(const float* const pointer);

		QuadFloat&           operator+=(const QuadFloat& rhs);
		QuadFloat&           operator-=(const QuadFloat& rhs);
		QuadFloat&           operator*=(const QuadFloat& rhs);
		QuadFloat&           operator/=(const QuadFloat& rhs);
		QuadFloat            operator+(const QuadFloat& rhs) const;
		QuadFloat            operator-(const QuadFloat& rhs) const;
		QuadFloat            operator*(const QuadFloat& rhs) const;
		QuadFloat            operator/(const QuadFloat& rhs) const;

		QuadMask             greaterThan(const QuadFloat& rhs) const;
		QuadMask             lessThan(const QuadFloat& rhs) const;

		const QuadFloat&     abs(); // TODO: Return void
		QuadFloat            floor() const;

		QuadSizeT            toQuadSizeT() const;

		float                get(const size_t index) const;

		void                 write(float* const pointer) const;

	private:
		std::array<float, 4> m_data;
	};
}