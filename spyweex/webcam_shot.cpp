

#include "webcam_shot.h"
#include "file_utils.h"
#include "string_utils.h"
#include <fstream>
#include <tchar.h>
#include <boost/lexical_cast.hpp>
#include "socket_utils.hpp"

namespace http {
	namespace server{

		WebcamShot::WebcamShot(boost::asio::ip::tcp::socket& sock, boost::asio::io_service& io_ref):
			TaskHandlerInterface(sock, io_ref)
		{
			operations_queue_ptr = async_op::new_();
		}

		int WebcamShot::GetEncoderClsid(WCHAR *format, CLSID *pClsid)
		{
			unsigned int num = 0, size = 0;
			Gdiplus::GetImageEncodersSize(&num, &size);
			if (size == 0) return -1;
			Gdiplus::ImageCodecInfo *pImageCodecInfo = (Gdiplus::ImageCodecInfo *)(malloc(size));
			if (pImageCodecInfo == NULL) return -1;
			Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

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

		int WebcamShot::setupESCAPI()
		{
			initCOM();
			return countCaptureDevices();
		}

		boost::system::error_code WebcamShot::take_picture(std::shared_ptr<vector<char>> buffer)
		{
			const int JPEG_QUALITY = 100;

			int devices = setupESCAPI();

			if (devices == 0)
			{
				cerr << "ESCAPI initialization failure or no devices found.\n" << GetLastError();
				return boost::system::error_code(boost::system::errc::make_error_code(boost::system::errc::io_error));
			}

			/* Set up capture parameters.
			* ESCAPI will scale the data received from the camera
			* (with point sampling) to whatever values you want.
			* Typically the native resolution is 320*240.
			*/
			struct SimpleCapParams capture;
			capture.mWidth = 640;
			capture.mHeight = 480;
			capture.mTargetBuf = new int[640 * 480];
			
			/* Initialize capture - only one capture may be active per device,
			* but several devices may be captured at the same time.
			*
			* 0 is the first device.
			*/

			if (initCapture(0, &capture) == 0)
			{
				cerr << "Capture failed - device may already be in use.\n" << GetLastError();
				return boost::system::error_code(boost::system::errc::make_error_code(boost::system::errc::io_error));
			}
			/* request a capture */
			doCapture(0);
			while (isCaptureDone(0) == 0)
			{
				/* Wait until capture is done. */
			}

			Sleep(1000);
			/***************************************mirror array********************************************/

			int* data = new int[480 * 640];

			for (int i = 640 * 480; i > 0; i--)
				data[640 * 480 - i] = capture.mTargetBuf[i - 1];

			//******************************************END CAPTURE******************************************
			deinitCapture(0);

			//**********************************WRITE TO A JPEG FILE*****************************************
			unsigned int width = 640;
			unsigned int height = 480;

			BITMAPINFO Info;
			memset(&Info, 0, sizeof(Info));

			Info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			Info.bmiHeader.biWidth = width;
			Info.bmiHeader.biHeight = height;
			Info.bmiHeader.biPlanes = 1;
			Info.bmiHeader.biBitCount = 32;
			Info.bmiHeader.biCompression = BI_RGB;
			Info.bmiHeader.biSizeImage = 0;  //(((32 * width + 31) & ~31) / 8) * height;

			Gdiplus::GdiplusStartupInput gdiplusStartupInput;
			ULONG_PTR gdiplusToken;
			Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

			Gdiplus::Bitmap* pScreenShot;
			pScreenShot = new Gdiplus::Bitmap(&Info, data);

			ULONG uQuality = 100;
			//CLSID imageCLSID = {0x557cf401, 0x1a04, 0x11d3, {0x9a, 0x73, 0x00, 0x00,0xf8,0x1e,0xf3, 0x2e}}; //this is for jpeg only
			CLSID imageCLSID;

			int iRes;
			// save the buffer to a file   
			Gdiplus::EncoderParameters encoderParams;
			encoderParams.Count = 1;
			encoderParams.Parameter[0].NumberOfValues = 1;
			encoderParams.Parameter[0].Guid = Gdiplus::EncoderQuality;
			encoderParams.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
			encoderParams.Parameter[0].Value = &uQuality;
			GetEncoderClsid(L"image/jpeg", &imageCLSID);

			WCHAR WCHAR_WindowsPath[MAX_PATH];
			GetWindowsDirectory(WCHAR_WindowsPath, MAX_PATH);
			std::wstring arr_w(WCHAR_WindowsPath);
			std::string WindowsPath(arr_w.begin(), arr_w.end());
			WindowsPath.append("\\Temp\\");

			std::string randomFileName = string_utils::random_string(12);
			randomFileName.append(".jpg");
			WindowsPath.append(randomFileName);
			randomFileName = WindowsPath;
			wchar_t *lpszFilename = new wchar_t[randomFileName.length() + 1];
			mbstowcs(lpszFilename, randomFileName.c_str(), randomFileName.length() + 1);

			iRes = (pScreenShot->Save(lpszFilename, &imageCLSID, &encoderParams) == 0);

			std::ifstream is(lpszFilename, std::ios::in | std::ios::binary);
			if (!is)
			{
				cerr << "failed to open a stream for webcam_snap. err: %d\n" << GetLastError();
				return boost::system::error_code(boost::system::errc::make_error_code(boost::system::errc::io_error));
			}

			char buf[512];
			while (is.read(buf, sizeof(buf)).gcount() > 0)
				buffer->insert(buffer->end(), buf, buf + is.gcount());
			is.close();
			delete pScreenShot;
			delete[] data;
			Gdiplus::GdiplusShutdown(gdiplusToken);
			DeleteFile(lpszFilename);
			return boost::system::error_code(boost::system::errc::make_error_code(boost::system::errc::success));
		}

		void WebcamShot::on_take_picture(std::shared_ptr<request> req, std::shared_ptr<reply> rep, 
			std::shared_ptr<vector<char>> buffer, boost::system::error_code& e)
		{
			if (buffer->empty())
			{
				std::shared_ptr<reply> temp_rep = reply::stock_reply(reply::internal_server_error);
				rep.swap(temp_rep);
			}
			else
			{
				std::string extension = "jpg";
				rep->status_line = http::server::status_strings::ok;
				rep->status_line.append(" ").append(req->action_type).append("\r\n");

				std::string s(buffer->data(), buffer->size());
				rep->content.append(s);

				rep->headers.resize(3);
				rep->headers[0].name = "Tag";
				rep->headers[0].value = std::string(req->dictionary_headers.at("Tag"));
				rep->headers[1].name = "Content-Type";
				rep->headers[1].value = mime_types::extension_to_type(extension);
				rep->headers[2].name = "Content-Length";
				rep->headers[2].value = std::to_string(rep->content.size());
			}
			
			OutputDebugString(_T("webcamshot made on thread id\n"));
			std::wstring threadId = boost::lexical_cast<std::wstring>(boost::this_thread::get_id());
			OutputDebugString(threadId.c_str());

			socket_write_mutex_.lock();
			write(socket_, rep->to_buffers());
			socket_utils::write_delimiter(socket_);
			rep.reset();
			socket_write_mutex_.unlock();
		}

		bool WebcamShot::execute(std::shared_ptr<request> req)
		{
			if (req->action_type.compare(wxhtpconstants::ACTION_TYPE::TAKE_WEBCAM_SCREEN))
			{
				return false;
			};
			std::shared_ptr<reply> rep(new reply());
			std::shared_ptr<std::vector<char>> buffer_ptr = std::make_shared<std::vector<char>>();

			operations_queue_ptr->add(
				boost::bind(&WebcamShot::take_picture, this, buffer_ptr),
				boost::bind(&WebcamShot::on_take_picture, this, req, rep, buffer_ptr, _1), 
				io_ref_);

			return true;
		}
	}
}
