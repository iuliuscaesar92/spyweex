
#include "stdafx.h"


namespace http {
	namespace server {
		int ScreenshootTaker::GetEncoderClsid(WCHAR *format, CLSID *pClsid)
		{
			unsigned int num = 0, size = 0;
			GetImageEncodersSize(&num, &size);
			if (size == 0) return -1;
			ImageCodecInfo *pImageCodecInfo = (ImageCodecInfo *)(malloc(size));
			if (pImageCodecInfo == NULL) return -1;
			GetImageEncoders(num, size, pImageCodecInfo);

			for (unsigned int j = 0; j < num; ++j) {
				if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
					*pClsid = pImageCodecInfo[j].Clsid;
					free(pImageCodecInfo);
					return j;
				}
			}
			free(pImageCodecInfo);
			return -1;
		}

		std::tuple<int, std::vector<char>> ScreenshootTaker::TakeScreenshoot(ULONG uQuality)
		{
			ULONG_PTR gdiplusToken;
			GdiplusStartupInput gdiplusStartupInput;
			GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
			HWND hMyWnd = GetDesktopWindow();
			RECT r;
			int w, h;
			HDC dc, hdcCapture;
			int nBPP, nCapture, iRes;
			LPBYTE lpCapture;
			CLSID imageCLSID;
			Bitmap *pScreenShot;

			TCHAR NPath[MAX_PATH];
			GetCurrentDirectory(MAX_PATH, NPath);
			// get the area of my application's window     
			GetWindowRect(hMyWnd, &r);
			dc = GetWindowDC(hMyWnd);   // GetDC(hMyWnd) ;
			w = r.right - r.left;
			h = r.bottom - r.top;
			nBPP = GetDeviceCaps(dc, BITSPIXEL);
			hdcCapture = CreateCompatibleDC(dc);

			// create the buffer for the screenshot
			BITMAPINFO bmiCapture = { sizeof(BITMAPINFOHEADER), w, -h, 1, nBPP, BI_RGB, 0, 0, 0, 0, 0, };

			// create a container and take the screenshot
			HBITMAP hbmCapture = CreateDIBSection(dc, &bmiCapture, DIB_PAL_COLORS, (LPVOID *)&lpCapture, NULL, 0);

			// failed to take it
			if (!hbmCapture) {
				DeleteDC(hdcCapture);
				DeleteDC(dc);
				GdiplusShutdown(gdiplusToken);
				printf("failed to take the screenshot. err: %d\n", GetLastError());
				return std::make_tuple(-1, std::vector<char>());
			}

			// copy the screenshot buffer
			nCapture = SaveDC(hdcCapture);
			SelectObject(hdcCapture, hbmCapture);
			BitBlt(hdcCapture, 0, 0, w, h, dc, 0, 0, SRCCOPY);
			RestoreDC(hdcCapture, nCapture);
			DeleteDC(hdcCapture);
			DeleteDC(dc);

			// save the buffer to a file   
			pScreenShot = new Bitmap(hbmCapture, (HPALETTE)NULL);
			EncoderParameters encoderParams;
			encoderParams.Count = 1;
			encoderParams.Parameter[0].NumberOfValues = 1;
			encoderParams.Parameter[0].Guid = EncoderQuality;
			encoderParams.Parameter[0].Type = EncoderParameterValueTypeLong;
			encoderParams.Parameter[0].Value = &uQuality;
			GetEncoderClsid(L"image/jpeg", &imageCLSID);

			std::string randomFileName = random_string(12);
			wchar_t *lpszFilename = new wchar_t[randomFileName.length() + 1];
			mbstowcs(lpszFilename, randomFileName.c_str(), randomFileName.length() + 1);

			iRes = (pScreenShot->Save(lpszFilename, &imageCLSID, &encoderParams) == Ok);

			std::vector<char> buffer;
			std::ifstream is(lpszFilename, std::ios::in | std::ios::binary);
			if (!is)
			{
				DeleteDC(hdcCapture);
				DeleteDC(dc);
				GdiplusShutdown(gdiplusToken);
				printf("failed to open a stream for screenshoot. err: %d\n", GetLastError());
				return std::make_tuple(-1, std::vector<char>());
			}

			char buf[512];
			while (is.read(buf, sizeof(buf)).gcount() > 0)
				buffer.insert(buffer.end(), buf, buf + is.gcount() );
			
			delete pScreenShot;
			DeleteObject(hbmCapture);
			GdiplusShutdown(gdiplusToken);
			return std::make_tuple(iRes, buffer);
		}

		int ScreenshootTaker::SaveScreenshot(string filename, ULONG uQuality) // by Napalm
		{
			ULONG_PTR gdiplusToken;
			GdiplusStartupInput gdiplusStartupInput;
			GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
			HWND hMyWnd = GetDesktopWindow();
			RECT r;
			int w, h;
			HDC dc, hdcCapture;
			int nBPP, nCapture, iRes;
			LPBYTE lpCapture;
			CLSID imageCLSID;
			Bitmap *pScreenShot;

			// get the area of my application's window     
			GetWindowRect(hMyWnd, &r);
			dc = GetWindowDC(hMyWnd);   // GetDC(hMyWnd) ;
			w = r.right - r.left;
			h = r.bottom - r.top;
			nBPP = GetDeviceCaps(dc, BITSPIXEL);
			hdcCapture = CreateCompatibleDC(dc);

			// create the buffer for the screenshot
			BITMAPINFO bmiCapture = { sizeof(BITMAPINFOHEADER), w, -h, 1, nBPP, BI_RGB, 0, 0, 0, 0, 0, };

			// create a container and take the screenshot
			HBITMAP hbmCapture = CreateDIBSection(dc, &bmiCapture, DIB_PAL_COLORS, (LPVOID *)&lpCapture, NULL, 0);

			// failed to take it
			if (!hbmCapture) {
				DeleteDC(hdcCapture);
				DeleteDC(dc);
				GdiplusShutdown(gdiplusToken);
				printf("failed to take the screenshot. err: %d\n", GetLastError());
				return 0;
			}

			// copy the screenshot buffer
			nCapture = SaveDC(hdcCapture);
			SelectObject(hdcCapture, hbmCapture);
			BitBlt(hdcCapture, 0, 0, w, h, dc, 0, 0, SRCCOPY);
			RestoreDC(hdcCapture, nCapture);
			DeleteDC(hdcCapture);
			DeleteDC(dc);

			// save the buffer to a file   
			pScreenShot = new Bitmap(hbmCapture, (HPALETTE)NULL);
			EncoderParameters encoderParams;
			encoderParams.Count = 1;
			encoderParams.Parameter[0].NumberOfValues = 1;
			encoderParams.Parameter[0].Guid = EncoderQuality;
			encoderParams.Parameter[0].Type = EncoderParameterValueTypeLong;
			encoderParams.Parameter[0].Value = &uQuality;
			GetEncoderClsid(L"image/jpeg", &imageCLSID);

			wchar_t *lpszFilename = new wchar_t[filename.length() + 1];
			mbstowcs(lpszFilename, filename.c_str(), filename.length() + 1);

			iRes = (pScreenShot->Save(lpszFilename, &imageCLSID, &encoderParams) == Ok);
			delete pScreenShot;
			DeleteObject(hbmCapture);
			GdiplusShutdown(gdiplusToken);
			return iRes;
		}

		std::string ScreenshootTaker::random_string(size_t length)
		{
			srand(time(NULL));
			auto randchar = []() -> char
			{
				const char charset[] =
					"0123456789"
					"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
					"abcdefghijklmnopqrstuvwxyz";
				const size_t max_index = (sizeof(charset) - 1);
				return charset[rand() % max_index];
			};
			std::string str(length, 0);
			std::generate_n(str.begin(), length, randchar);
			return str;
		}

		//int main() {
		//	string path = "screenshot.jpg";
		//	ULONG quality = 100;
		//	SaveScreenshot(path, quality);

		//	return 0;
		//}
	}
}