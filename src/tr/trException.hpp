#include <exception>
#include <string>

namespace tr
{
	class Exception : public std::exception
	{
	public:
		Exception(const std::string& info) :
			info(info)
		{
		}

	public:
		const std::string info;
	};
}