
#ifndef APP_ERROR_CATEGORY_HPP
#define APP_ERROR_CATEGORY_HPP

#include <boost/system/error_code.hpp>
#include <string>
#include <iostream>

namespace http {
	namespace server {
		//namespace my_errc
		//{
		//	enum my_errc_t
		//	{
		//		success = 0,
		//		unknown_error = 100,
		//		bad_message = 101							
		//	};
		//}
		//class application_category : public boost::system::error_category
		//{
		//public:
		//	const char *name() const { return "my app error category"; }
		//	std::string message(int ev) const { return "My error message goes here"; }
		//};
	}
}

#endif APP_ERROR_CATEGORY_HPP