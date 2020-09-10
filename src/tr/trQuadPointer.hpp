#pragma once

#include "immintrin.h"
#include <array>
#include "trQuadSizeT.hpp"

namespace tr
{
	template<typename T>
	class QuadPointer
	{
	public:
		QuadPointer(const T* const a) :
			m_data{ a, a, a, a }
		{
		}

		QuadPointer(const T* const a, const T* const b, const T* const c, const T* const d) :
			m_data{ a, b, c, d }
		{
		}

		QuadPointer& operator+=(const QuadSizeT& rhs)
		{
			for (size_t i = 0; i < m_data.size(); ++i)
			{
				m_data[i] += rhs.getData()[i];
			}

			return *this;
		}

		QuadPointer operator+(const QuadSizeT& rhs)
		{
			QuadPointer<T> result = *this;

			for (size_t i = 0; i < m_data.size(); ++i)
			{
				result.m_data[i] += rhs.getData()[i];
			}

			return result;
		}

	private:
		std::array<const T*, 4> m_data;
	};
}