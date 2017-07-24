#pragma once

#include <cmath>
#include <iostream>

namespace tr
{
	template<typename T>
	class Vec4
	{
	public:
		T* const                data;
		T&                      x;
		T&                      y;
		T&                      z;
		T&                      w;
		static constexpr T      epsilon = (T) 0.000000;
		static constexpr size_t size = 4;

	public:
		T& At(const size_t i)
		{
			return data[i];
		}

		T GetAt(const size_t i) const
		{
			return data[i];
		}

		double Length() const
		{
			return sqrt(x*x + y*y + z*z);
		}

		void Print() const
		{
			std::cout << x << "\t" << y << "\t" << z << "\t" << w << std::endl << std::endl;
		}

		void SetZero()
		{
			x = y = z = w = (T) 0;
		}

	public:
		T& operator[](const size_t i)
		{
			return data[i];
		}

		template<typename U>
		Vec4<T> operator+(const Vec4<U>& rhs) const
		{
			return tr::Vec4<T>(x + (T) rhs.x, y + (T) rhs.y, z + (T) rhs.z, w);
		}

		template<typename U>
		Vec4<T> operator-(const Vec4<U>& rhs) const
		{
			return tr::Vec4<T>(x - (T) rhs.x, y - (T) rhs.y, z - (T) rhs.z, w);
		}

		template<typename U>
		Vec4<T> operator*(const U& rhs) const
		{
			return tr::Vec4<T>(x * (T) rhs, y * (T) rhs, z * (T) rhs, w);
		}

		template<typename U>
		Vec4<T> operator/(const U& rhs) const
		{
			return tr::Vec4<T>(x / (T) rhs, y / (T) rhs, z / (T) rhs, w);
		}

		template<typename U>
		Vec4<T>& operator+=(const Vec4<U>& rhs)
		{
			x += (T) rhs.x;
			y += (T) rhs.y;
			z += (T) rhs.z;
			return *this;
		}

		template<typename U>
		Vec4<T>& operator-=(const Vec4<U>& rhs)
		{
			x -= (T) rhs.x;
			y -= (T) rhs.y;
			z -= (T) rhs.z;
			return *this;
		}

		template<typename U>
		Vec4<T>& operator*=(const U& rhs)
		{
			x *= (T) rhs;
			y *= (T) rhs;
			z *= (T) rhs;
			w *= (T) rhs;
			return *this;
		}

		template<typename U>
		Vec4<T>& operator/=(const U& rhs)
		{
			x /= (T) rhs;
			y /= (T) rhs;
			z /= (T) rhs;
			w /= (T) rhs;
			return *this;
		}

		template<typename U>
		bool operator==(const U& rhs) const
		{
			return (x <= (T) rhs.x + epsilon &&
					x >= (T) rhs.x - epsilon &&
					y <= (T) rhs.y + epsilon &&
					y >= (T) rhs.y - epsilon &&
					z <= (T) rhs.z + epsilon &&
					z >= (T) rhs.z - epsilon &&
					w <= (T) rhs.w + epsilon &&
					w >= (T) rhs.w - epsilon);
		}

	public:
		Vec4() :
			data(new T[size]),
			x(data[0]),
			y(data[1]),
			z(data[2]),
			w(data[3])
		{
			SetZero();
		}

		Vec4(const tr::Vec4<T>& vecA) :
			data(new T[size]),
			x(data[0]),
			y(data[1]),
			z(data[2]),
			w(data[3])
		{
			x = vecA.x;
			y = vecA.y;
			z = vecA.z;
			w = vecA.w;
		}

		Vec4(const T& xInit, const T& yInit, const T& zInit, const T& wInit) :
			data(new T[size]),
			x(data[0]),
			y(data[1]),
			z(data[2]),
			w(data[3])
		{
			x = xInit;
			y = yInit;
			z = zInit;
			w = wInit;
		}

		~Vec4()
		{
			delete[] data;
		}
	};

	typedef	Vec4<float>     Vec4f;
	typedef	Vec4<double>    Vec4d;
	typedef Vec4<int>       Vec4i;
}
