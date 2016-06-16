
#ifndef SOCKET_UTILS_HPP
#define SOCKET_UTILS_HPP
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/write.hpp>

namespace http
{
	namespace server
	{
		namespace socket_utils
		{
			void inline write_delimiter(boost::asio::ip::tcp::socket& socket_)
			{
				const char end_of_wxhtp_message[] = "end_wxhtp";
				boost::asio::write(socket_, 
					boost::asio::buffer(end_of_wxhtp_message, strlen(end_of_wxhtp_message) + sizeof(char)));
			}

		}
	}
}
#endif
