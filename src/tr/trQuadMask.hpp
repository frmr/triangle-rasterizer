#pragma once

#include <array>

namespace tr
{
	class QuadMask
	{
	public:
		                    QuadMask(const bool a);
		                    QuadMask(const bool a, const bool b, const bool c, const bool d);

		QuadMask&           operator&=(const QuadMask& rhs);
		QuadMask            operator|(const QuadMask& rhs) const;

		bool                get(const size_t index) const;
		bool                moveMask() const;

	private:
		std::array<bool, 4> m_data;
	};
}