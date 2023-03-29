#pragma once

#include <stdexcept>

namespace tr
{
	class InvalidSettingException : public std::runtime_error
	{
	public:
		InvalidSettingException(const std::string& what) :
			std::runtime_error("InvalidSettingException: " + what)
		{
		}
	};
}