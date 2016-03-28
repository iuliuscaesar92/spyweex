#pragma once

#include "stdafx.h"


#pragma comment(lib, "gdiplus.lib")
using namespace std;
using namespace Gdiplus;
// oda
// oda asd
namespace http {
	namespace server {


		class ScreenshootTaker
		{
		public:
			static int GetEncoderClsid(WCHAR *format, CLSID *pClsid);

			static int SaveScreenshot(string filename, ULONG uQuality); // by Napalm

			static std::tuple<int, std::vector<char>> TakeScreenshoot(ULONG uQuality);

			static std::string random_string(size_t length);
		};
	}
}