
//#include <boost/process.hpp>

#include "command_prompt_executor.h"
#include "file_utils.h"
#include "string_utils.h"
#include <tchar.h>

#include <windows.h>

namespace http {
	namespace server {
		
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

			RegCloseKey(hKey);//Dont forget to cleanup!!!!
			return 1;
		}

		std::tuple<int, std::vector<char>> CommandPromptExecutor::run_command(std::string command)
		{
			//int iRes;
			LPCSTR szCmdline = command.c_str();
			std::string result = "";

			std::shared_ptr<FILE> pipe(_popen(szCmdline, "r"), _pclose);
			if (!pipe)
			{
				printf("failed to open a pipe for command result. err: %d\n", GetLastError());
				return std::make_tuple(-1, std::vector<char>());
			}

			char pipe_buf[128];
			while (!feof(pipe.get())) {
				if (fgets(pipe_buf, 128, pipe.get()) != NULL)
					result += pipe_buf;
			}

			std::vector<char> buffer;
			std::stringstream  ss(result);
			if (!ss)
			{
				printf("failed to open a stream for command result. err: %d\n", GetLastError());
				return std::make_tuple(-1, std::vector<char>());
			}

			char stream_buf[512];
			while (ss.read(stream_buf, sizeof(stream_buf)).gcount() > 0)
				buffer.insert(buffer.end(), stream_buf, stream_buf + ss.gcount());

			return std::make_tuple(0, buffer);
		}

		bool CommandPromptExecutor::execute(const request& req, reply& rep)
		{
			if(req.action_type.compare(wxhtpconstants::ACTION_TYPE::COMMAND_PROMPT))
			{
				return false;
			}
			int code; std::vector<char> buffer;
			std::string command = std::string(req.content.begin(), req.content.end());
			//std::vector<std::wstring> splitted_wcommand = string_utils::split<std::wstring>(wcommand, L"%%");
			//wcommand = string_utils::join(splitted_wcommand, L" ");

			std::tie(code, buffer) = CommandPromptExecutor::run_command(command);

			if (code != 0 && buffer.empty())
			{
				rep = reply::stock_reply(reply::internal_server_error);
				return true;
			}


			rep.status = reply::ok;

			std::string s(buffer.data(), buffer.size());
			rep.content.append(s);
			rep.headers.resize(3);
			rep.headers[0].name = "Tag";
			rep.headers[0].value = std::string(req.dictionary_headers.at("Tag"));
			rep.headers[1].name = "Content-Type";
			rep.headers[1].value = mime_types::extension_to_type("text/plain");
			rep.headers[2].name = "Content-Length";
			rep.headers[2].value = std::to_string(rep.content.size());
			std::string encoding;
			if(get_reg_oemcp(encoding))
			{
				rep.headers.resize(4);
				rep.headers[3].name = "Encoding";
				rep.headers[3].value = encoding;
			};

			return true;
		}
	}
}
