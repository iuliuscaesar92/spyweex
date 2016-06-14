
#include "thumbnail_executor.h"

#include <fstream>

#include <stdlib.h>
#include "file_utils.h"
#include "string_utils.h"
#include "reply.hpp"

#include <tchar.h>
#include <boost/lexical_cast.hpp>

namespace http {
	namespace server {

		ThumbnailTaker::ThumbnailTaker(boost::asio::ip::tcp::socket& sock, boost::asio::io_service& io_ref) :
			TaskHandlerInterface(sock, io_ref), thumbnail_report_timer(io_ref)
		{
			operations_queue_ptr = async_op::new_();
		}

		int ThumbnailTaker::GetEncoderClsid(WCHAR *format, CLSID *pClsid)
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

		boost::system::error_code ThumbnailTaker::take_screenshot(std::shared_ptr<vector<char>> buffer, ULONG uQuality)
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
				return boost::system::error_code(boost::system::errc::make_error_code(boost::system::errc::io_error));
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

			WCHAR WCHAR_WindowsPath[MAX_PATH];
			GetWindowsDirectory(WCHAR_WindowsPath, MAX_PATH);
			std::wstring arr_w(WCHAR_WindowsPath);
			std::string WindowsPath(arr_w.begin(), arr_w.end());
			WindowsPath.append("\\Temp\\");

			std::string randomFileName = string_utils::random_string(12);
			WindowsPath.append(randomFileName);
			randomFileName = WindowsPath;
			wchar_t *lpszFilename = new wchar_t[randomFileName.length() + 1];
			mbstowcs(lpszFilename, randomFileName.c_str(), randomFileName.length() + 1);

			HANDLE fHandle = file_utils::CreateFileWithAllPermissions(lpszFilename);
			CloseHandle(fHandle);

			iRes = (pScreenShot->Save(lpszFilename, &imageCLSID, &encoderParams) == Ok);
			std::ifstream is(lpszFilename, std::ios::in | std::ios::binary);
			if (!is)
			{
				DeleteDC(hdcCapture);
				DeleteDC(dc);
				GdiplusShutdown(gdiplusToken);
				printf("failed to open a stream for screenshoot. err: %d\n", GetLastError());
				return boost::system::error_code(boost::system::errc::make_error_code(boost::system::errc::io_error));
			}

			char buf[512];
			while (is.read(buf, sizeof(buf)).gcount() > 0)
				buffer->insert(buffer->end(), buf, buf + is.gcount());
			is.close();
			delete pScreenShot;
			delete []lpszFilename;
			DeleteObject(hbmCapture);
			GdiplusShutdown(gdiplusToken);

			DeleteFile(lpszFilename);

			return boost::system::error_code(boost::system::errc::make_error_code(boost::system::errc::success));
		}

		void ThumbnailTaker::on_take_screenshot(std::shared_ptr<vector<char>> buffer)
		{
			std::shared_ptr<reply> rep(new reply());
			if (buffer->empty())
			{
				std::shared_ptr<reply> temp_rep = reply::stock_reply(reply::internal_server_error);
				rep.swap(temp_rep);
				isWorking = false;
			}
			else
			{
				std::string extension = "jpg";

				// Fill out the reply to be sent to the client.
				rep->status_line = http::server::status_strings::ok;
				if(isWorking)
					rep->status_line.append(" ").append(wxhtpconstants::ACTION_TYPE::THUMBNAIL_SCREEN_REPORT).append("\r\n");
				else
					rep->status_line.append(" ").append(wxhtpconstants::ACTION_TYPE::THUMBNAIL_SCREEN_STOP).append("\r\n");

				std::string s(buffer->data(), buffer->size());
				rep->content.append(s);

				rep->headers.resize(3);
				rep->headers[0].name = "Tag";
				rep->headers[0].value = std::string(_request_buffer_ptr->dictionary_headers.at("Tag"));
				rep->headers[1].name = "Content-Type";
				rep->headers[1].value = mime_types::extension_to_type(extension);
				rep->headers[2].name = "Content-Length";
				rep->headers[2].value = std::to_string(rep->content.size());
			}

			OutputDebugString(_T("thumbnail screenshot made on thread id\n"));
			std::wstring threadId = boost::lexical_cast<std::wstring>(boost::this_thread::get_id());
			OutputDebugString(threadId.c_str());

			socket_write_mutex_.lock();
			try
			{
				// if connection is dropped, then we should catch the exception
				write(socket_, rep->to_buffers());
			}
			catch(exception ex)
			{
				isWorking = false;
				OutputDebugString(_T("\nException in socket. Can't write data for thumbnail\n"));
			}
			socket_write_mutex_.unlock();
			rep.reset();

			if(isWorking) screenshot_retention();
		}

		void ThumbnailTaker::screenshot_retention()
		{
			thumbnail_report_timer.expires_from_now(boost::posix_time::seconds(4));
			thumbnail_report_timer.async_wait(boost::bind(&ThumbnailTaker::on_retention_timer_expires, this, _1));
		}

		void ThumbnailTaker::on_retention_timer_expires(boost::system::error_code e)
		{
			if (!e)
			{
				add_new_thumbnail_task();
			}
			else if(e == boost::asio::error::operation_aborted)
			{
				isWorking = false;
				add_new_thumbnail_task(); // do the last screen
			}
			else
			{
				isWorking = false;
			}
			
		}

		void ThumbnailTaker::add_new_thumbnail_task()
		{
			std::shared_ptr<std::vector<char>> buffer_ptr = std::make_shared<std::vector<char>>();
			take_screenshot(buffer_ptr, 100),
			on_take_screenshot(buffer_ptr);
		}

		void ThumbnailTaker::stop_thumbnail_task()
		{
			thumbnail_report_timer.cancel();
		}

		bool ThumbnailTaker::execute(std::shared_ptr<request> req)
		{

			if (req->action_type.compare(wxhtpconstants::ACTION_TYPE::THUMBNAIL_SCREEN_START))  // if compare result is something but not zero (zero is equality)
				if (req->action_type.compare(wxhtpconstants::ACTION_TYPE::THUMBNAIL_SCREEN_STOP))
					return false;

			_request_buffer_ptr.swap(req);
			if (_request_buffer_ptr->action_type.compare(wxhtpconstants::ACTION_TYPE::THUMBNAIL_SCREEN_START) == 0)
			{
				if (!isWorking)
				{
					isWorking = true;
					add_new_thumbnail_task();
				}
			}
			else if (_request_buffer_ptr->action_type.compare(wxhtpconstants::ACTION_TYPE::THUMBNAIL_SCREEN_STOP) == 0)
			{
				if (isWorking)
				{
					isWorking = false;
					stop_thumbnail_task();
				}
			}			

			// to be deleted.
			//boost::asio::deadline_timer dt(io_ref_);
			//dt.expires_from_now(boost::posix_time::seconds(10));
			//dt.wait();
			//take_screenshot(buffer_ptr, 100);
			//boost::system::error_code ec;
			//on_take_screenshot(req, rep, buffer_ptr, ec);
			//to be deleted

			return true;
		}

	}
}