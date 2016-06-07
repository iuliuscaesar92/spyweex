

#ifndef WEBCAM_SHOT_HPP
#define WEBCAM_SHOT_HPP

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
//
//#pragma comment(lib, "vfw32.lib")
//#pragma comment( lib, "comctl32.lib" )


#include <iostream>
#include <string>

#include "task_handler_interface.h"
#include "request.hpp"
#include "reply.hpp"
#include "mime_types.hpp"

using namespace cv;
using namespace std;

namespace http {
	namespace server {

		class WebcamShot: public TaskHandlerInterface, private boost::noncopyable
		{
		public:
			explicit WebcamShot(boost::asio::ip::tcp::socket& sock, boost::asio::io_service& io_ref);

			boost::system::error_code take_picture(std::shared_ptr<vector<char>> buffer);

			void on_take_picture(std::shared_ptr<request> req, std::shared_ptr<reply> rep, std::shared_ptr<vector<char>> buffer, boost::system::error_code& e);

			bool execute(std::shared_ptr<request> req) override;
		};

	}
}
#endif