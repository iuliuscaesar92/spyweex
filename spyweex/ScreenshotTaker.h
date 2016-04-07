

#ifndef SCREENSHOT_TAKER_HPP
#define SCREENSHOT_TAKER_HPP

#include <iostream>
#include <string>

#include "TaskHandlerInterface.h"
#include "request.hpp"
#include "reply.hpp"
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
#undef min
#undef max

#pragma comment(lib, "gdiplus.lib")
using namespace std;
using namespace Gdiplus;

namespace http {
	namespace server {


		class ScreenshotTaker: public TaskHandlerInterface
		{
		public:
			static int GetEncoderClsid(WCHAR *format, CLSID *pClsid);

			static int SaveScreenshot(string filename, ULONG uQuality); // by Napalm

			static std::tuple<int, std::vector<char>> TakeScreenshot(ULONG uQuality);

			static std::string random_string(size_t length);
			
			bool execute(const request& req, reply& rep) override;
		};
	}
}
#endif