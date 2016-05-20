

#ifndef WEBCAM_SHOT_HPP
#define WEBCAM_SHOT_HPP

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

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

		class WebcamShot: public TaskHandlerInterface
		{
		public:

			static std::tuple<int, std::vector<char>> take_picture();

			bool execute(std::shared_ptr<request> req, std::shared_ptr<reply> rep) override;
		};

	}
}
#endif