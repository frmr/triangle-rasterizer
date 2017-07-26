#include <exception>
#include <string>

namespace tr
{
	class Exception : public std::exception
	{
	public:
		Exception(const std::string& info) :
			m_info(info)
		{
		}

	public:
		const std::string m_info;
	};
}