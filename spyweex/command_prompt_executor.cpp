
//#include <boost/process.hpp>

#include "command_prompt_executor.h"
#include "file_utils.h"
#include "string_utils.h"
#include "reply.hpp"

#include <tchar.h>
#include <fstream>

#include <windows.h>
#include <shellapi.h>
#include <comdef.h>

namespace http {
	namespace server {

		CommandPromptExecutor::CommandPromptExecutor(boost::asio::ip::tcp::socket& sock, boost::asio::io_service& io_ref):
			TaskHandlerInterface(sock, io_ref)
		{
			operations_queue_ptr = async_op::new_();
		}

		int CommandPromptExecutor::get_reg_oemcp(std::string& result)
		{
			//Size of buffer
			const int cBuffSize = 255;	    
			TCHAR szBuff[cBuffSize] =
				//Actual buffer
				{ 0 },
				//Path of key
				*szPath = _T("SYSTEM\\CurrentControlSet\\Control\\Nls\\CodePage");

			//Key used to hget value
			HKEY hKey = NULL;
			//To tell buff size and recieve actual size
			DWORD dwSize = cBuffSize; 


			//First open a key to allow you to read the registry
			if (RegOpenKeyEx(
				//Main key to browse
				HKEY_LOCAL_MACHINE,
				//sub key
				szPath,
				0,
				//access rights - we want to read
				KEY_READ,
				//Recieve the key we want to use
				&hKey)
				!= ERROR_SUCCESS) 
			{
				//MessageBox(HWND_DESKTOP, _T("Could not open key"), NULL, MB_OK);
				std::cerr << "Could not open Registry CodePage key";
				return 0;
			}

			if (RegQueryValueEx(
				//From previous call
				hKey,
				//value we want to look at
				_T("OEMCP"),
				0,
				//not needed,we know its a string
				NULL,
				//Put info here
				(UCHAR*)szBuff,
				//How big is the buffer?
				&dwSize)
				!= ERROR_SUCCESS) 
			{
				std::cerr << "Could not get the value for OEMCP";
				return 0;
			}

			result = std::string(szBuff, szBuff + _tcslen(szBuff));

			RegCloseKey(hKey); //Dont forget to cleanup!!!!
			return 1;
		}

		boost::system::error_code CommandPromptExecutor::run_command(std::shared_ptr<std::vector<char>> buffer, std::string command)
		{
			std::wstring wscommand(command.begin(), command.end());

			wchar_t *lpszFilename = file_utils::make_temp_file_path();
			HANDLE file_handle = file_utils::CreateFileWithAllPermissions(lpszFilename);
			CloseHandle(file_handle);

			std::wstring cmd = L"/C " + wscommand + L" > " + lpszFilename;

			SHELLEXECUTEINFO ShExecInfo;
			ZeroMemory(&ShExecInfo, sizeof(ShExecInfo));
			ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
			ShExecInfo.fMask = NULL;
			ShExecInfo.hwnd = NULL;
			ShExecInfo.lpVerb = L"open";
			ShExecInfo.lpFile = L"cmd.exe";
			ShExecInfo.lpParameters = cmd.c_str();
			ShExecInfo.lpDirectory = NULL;
			ShExecInfo.nShow = SW_HIDE;
			ShExecInfo.hInstApp = NULL;

			if ((INT)ShellExecuteExW(&ShExecInfo) <= 32)
			{
				printf("Executing command...\n");
			}

			Sleep(1000);
			std::ifstream file(lpszFilename, std::ios::in);
			std::string result;

			if (file) {
				while (!file.eof()) 
					result.push_back(file.get());
				file.close();
			}			

			std::stringstream  ss(result);
			if (!ss)
			{
				printf("failed to open a stream for command result. err: %d\n", GetLastError());
				return boost::system::error_code(boost::system::errc::make_error_code(boost::system::errc::io_error));
			}

			char stream_buf[512];
			while (ss.read(stream_buf, sizeof(stream_buf)).gcount() > 0)
				buffer->insert(buffer->end(), stream_buf, stream_buf + ss.gcount());

			_bstr_t b(lpszFilename);
			const char *mbs_path = b;
			remove(mbs_path);
			return boost::system::error_code(boost::system::errc::make_error_code(boost::system::errc::success));
		}

		void CommandPromptExecutor::on_run_command(std::shared_ptr<request> req, std::shared_ptr<reply> rep, std::shared_ptr<std::vector<char>> buffer, boost::system::error_code& e)
		{
			if (buffer->empty())
			{
				std::shared_ptr<reply> temp_rep = reply::stock_reply(reply::internal_server_error);
				rep.swap(temp_rep);
			}
			else
			{
				rep->status_line = http::server::status_strings::ok;
				rep->status_line.append(" ").append(req->action_type).append("\r\n");

				std::string s(buffer->data(), buffer->size());
				rep->content.append(s);
				rep->headers.resize(3);
				rep->headers[0].name = "Tag";
				rep->headers[0].value = std::string(req->dictionary_headers.at("Tag"));
				rep->headers[1].name = "Content-Type";
				rep->headers[1].value = mime_types::extension_to_type("text/plain");
				rep->headers[2].name = "Content-Length";
				rep->headers[2].value = std::to_string(rep->content.size());
				std::string encoding;
				if (CommandPromptExecutor::get_reg_oemcp(encoding))
				{
					rep->headers.resize(4);
					rep->headers[3].name = "Encoding";
					rep->headers[3].value = encoding;
				};
			}

			async_write_mutex.lock();
			async_write(socket_,
				rep->to_buffers(),
				boost::bind(&CommandPromptExecutor::handle_write,
					this, rep,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred)
				);
		}

		void CommandPromptExecutor::handle_write(std::shared_ptr<reply> rep, const boost::system::error_code& e, std::size_t bytes)
		{
			async_write_mutex.unlock();
			rep.reset();
		}

		bool CommandPromptExecutor::execute(std::shared_ptr<request> req)
		{
			if(req->action_type.compare(wxhtpconstants::ACTION_TYPE::COMMAND_PROMPT))
			{
				return false;
			}
			std::shared_ptr<reply> rep(new reply());
			std::shared_ptr<std::vector<char>> buffer_ptr = std::make_shared<std::vector<char>>();

			std::string command = std::string(req->content.begin(), req->content.end());
			//std::vector<std::wstring> splitted_wcommand = string_utils::split<std::wstring>(wcommand, L"%%");
			//wcommand = string_utils::join(splitted_wcommand, L" ");

			operations_queue_ptr->add(
				boost::bind(&CommandPromptExecutor::run_command, this, buffer_ptr, command),
				boost::bind(&CommandPromptExecutor::on_run_command, this, req, rep, buffer_ptr, _1), io_ref_);


			return true;
		}
	}
}
