

#include "webcam_shot.h"
#include "file_utils.h"
#include "string_utils.h"
#include <fstream>

namespace http {
	namespace server{

		std::tuple<int, std::vector<char>> WebcamShot::take_picture()
		{
			int iRes;
			const int JPEG_QUALITY = 100;
			vector<int> params;
			params.push_back(CV_IMWRITE_JPEG_QUALITY);
			params.push_back(JPEG_QUALITY);
			VideoCapture stream1(0);   //0 is the id of video device.0 if you have only one camera.
			if (!stream1.isOpened()) { //check if video device has been initialised
				cerr << "cannot open camera" << GetLastError();
				return std::make_tuple(-1, std::vector<char>());
			}
			Mat cameraFrame;
			stream1.read(cameraFrame);

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

			//HANDLE fHandle = file_utils::CreateFileWithAllPermissions(lpszFilename);
			//CloseHandle(fHandle);

			iRes = imwrite(randomFileName, cameraFrame, params);
			/*stream1.release();*/

			std::vector<char> buffer;
			std::ifstream is(lpszFilename, std::ios::in | std::ios::binary);
			if (!is)
			{
				cerr << "failed to open a stream for screenshoot. err: %d\n" << GetLastError();
				return std::make_tuple(-1, std::vector<char>());
			}

			char buf[512];
			while (is.read(buf, sizeof(buf)).gcount() > 0)
				buffer.insert(buffer.end(), buf, buf + is.gcount());
			is.close();

			DeleteFile(lpszFilename);
			return std::make_tuple(iRes, buffer);
		}

		bool WebcamShot::execute(std::shared_ptr<request> req, std::shared_ptr<reply> rep)
		{
			if (req->action_type.compare(wxhtpconstants::ACTION_TYPE::TAKE_WEBCAM_SCREEN))
			{
				return false;
			};
			std::string extension = "jpg";
			int code; std::vector<char> buffer;

			std::tie(code, buffer) = WebcamShot::take_picture();

			if (buffer.empty())
			{
				std::shared_ptr<reply> temp_rep = reply::stock_reply(reply::internal_server_error);
				rep.swap(temp_rep);
				return true;
			}

			rep->status_line = http::server::status_strings::ok;
			rep->status_line.append(" ").append(req->action_type).append("\r\n");

			std::string s(buffer.data(), buffer.size());
			rep->content.append(s);

			//char buf[512];
			//while (is.read(buf, sizeof(buf)).gcount() > 0)
			//  rep.content.append(buf, is.gcount());

			rep->headers.resize(3);
			rep->headers[0].name = "Tag";
			rep->headers[0].value = std::string(req->dictionary_headers.at("Tag"));
			rep->headers[1].name = "Content-Type";
			rep->headers[1].value = mime_types::extension_to_type(extension);
			rep->headers[2].name = "Content-Length";
			rep->headers[2].value = std::to_string(rep->content.size());
			return true;
		}
	}
}
