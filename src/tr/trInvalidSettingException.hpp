#pragma once

#include <stdexcept>
#include "tfString.hpp"

namespace tr
{
	class InvalidSettingException : public std::runtime_error
	{
	public:
		InvalidSettingException(const tf::String& what) :
			std::runtime_error("InvalidSettingException: " + what)
		{
		}
	};
}