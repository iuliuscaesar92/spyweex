
#ifndef THUMBNAIL_EXECUTOR_HPP
#define THUMBNAIL_EXECUTOR_HPP

#include <iostream>
#include <string>
#include "task_handler_interface.h"
#include "request.hpp"
#include "reply.hpp"
#include "async_operation.hpp"
#include "mime_types.hpp"


#include <windows.h>
#ifndef min
#define min(x,y) ((x) < (y) ? (x) : (y))
#endif
#ifndef max
#define max(x,y) ((x) > (y) ? (x) : (y))
#endif;
#include <Unknwn.h>  
#include <gdiplus.h>
#include "async_operation.hpp"
#undef min
#undef max

#pragma comment(lib, "gdiplus.lib")
using namespace std;
using namespace Gdiplus;
namespace http
{
	namespace server
	{
		class ThumbnailTaker: public TaskHandlerInterface, private boost::noncopyable
		{
			explicit ThumbnailTaker(boost::asio::ip::tcp::socket& sock, boost::asio::io_service& io_ref);

			static int GetEncoderClsid(WCHAR *format, CLSID *pClsid);

			boost::system::error_code ThumbnailTaker::take_screenshot(std::shared_ptr<std::vector<char>> buffer, ULONG uQuality);

			void on_take_screenshot(std::shared_ptr<request> req, std::shared_ptr<reply> rep, std::shared_ptr<std::vector<char>> buffer, boost::system::error_code& e);

			bool execute(std::shared_ptr<request> req) override;

			private:

				//deadline_timer thumbnail_report_timer;

				//std::shared_ptr<reply>	 _reply_buffer_ptr;
				//std::shared_ptr<request> _request_buffer_ptr;
		};
	}
}

#endif
