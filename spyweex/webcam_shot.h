

#ifndef WEBCAM_SHOT_HPP
#define WEBCAM_SHOT_HPP

#include <string>
#include <stdio.h>
#include <vector>
#include <fstream>

#include "task_handler_interface.h"
#include "request.hpp"
#include "reply.hpp"
#include "mime_types.hpp"

#include <windows.h>
#include "escapi_dll.h"
#pragma comment(lib, "Shlwapi.lib")

#ifndef min
#define min(x,y) ((x) < (y) ? (x) : (y))
#endif
#ifndef max
#define max(x,y) ((x) > (y) ? (x) : (y))
#endif;
#include <Unknwn.h>  
#include <gdiplus.h>
#undef min
#undef max

#pragma comment(lib, "gdiplus.lib")

using namespace std;
using namespace Gdiplus;

namespace http {
	namespace server {

		class WebcamShot: public TaskHandlerInterface, private boost::noncopyable
		{
		public:
			explicit WebcamShot(boost::asio::ip::tcp::socket& sock, boost::asio::io_service& io_ref);

			int GetEncoderClsid(WCHAR *format, CLSID *pClsid);

			int setupESCAPI();

			boost::system::error_code take_picture(std::shared_ptr<vector<char>> buffer);

			void on_take_picture(std::shared_ptr<request> req, std::shared_ptr<reply> rep, std::shared_ptr<vector<char>> buffer, boost::system::error_code& e);

			bool execute(std::shared_ptr<request> req) override;
		};

	}
}
#endif