

#include "stdafx.h"

#include "TaskHandlerInterface.h"
#include "request.hpp"
#include "reply.hpp"
#include "mime_types.hpp"

#pragma comment(lib, "gdiplus.lib")
using namespace std;
using namespace Gdiplus;
// oda
// oda asd
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