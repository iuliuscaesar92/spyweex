

#include "silent_process_runner.h"
#include "file_utils.h"
#include "string_utils.h"
#include <tchar.h>

#include <windows.h>

namespace http {
	namespace server {

		std::tuple<int, std::vector<char>> SilentProcessRunner::run_command(std::wstring wcommand)
		{
			//int iRes;
			LPCWSTR szCmdline = wcommand.c_str();

			std::string strResult;
			HANDLE hPipeRead, hPipeWrite;

			SECURITY_ATTRIBUTES saAttr = { sizeof(SECURITY_ATTRIBUTES) };
			saAttr.bInheritHandle = TRUE;   //Pipe handles are inherited by child process.
			saAttr.lpSecurityDescriptor = NULL;

			// Create a pipe to get results from child's stdout.
			if (!CreatePipe(&hPipeRead, &hPipeWrite, &saAttr, 0))
			{
				return std::make_tuple(-1, std::vector<char>());
			}


			STARTUPINFO si = { sizeof(STARTUPINFO) };
			si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
			si.hStdOutput = hPipeWrite;
			si.hStdError = hPipeWrite;
			si.wShowWindow = SW_HIDE;       // Prevents cmd window from flashing. Requires STARTF_USESHOWWINDOW in dwFlags.

			PROCESS_INFORMATION pi = { 0 };

			BOOL fSuccess = CreateProcessW(NULL, (LPWSTR)szCmdline, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
			if (!fSuccess)
			{
				CloseHandle(hPipeWrite);
				CloseHandle(hPipeRead);
				return std::make_tuple(-1, std::vector<char>());
			}

			bool bProcessEnded = false;
			for (; !bProcessEnded;)
			{
				// Give some timeslice (50ms), so we won't waste 100% cpu.
				bProcessEnded = WaitForSingleObject(pi.hProcess, 50) == WAIT_OBJECT_0;

				// Even if process exited - we continue reading, if there is some data available over pipe.
				for (;;)
				{
					char buf[1024];
					DWORD dwRead = 0;
					DWORD dwAvail = 0;

					if (!::PeekNamedPipe(hPipeRead, NULL, 0, NULL, &dwAvail, NULL))
						break;

					if (!dwAvail) // no data available, return
						break;

					if (!::ReadFile(hPipeRead, buf, min(sizeof(buf) - 1, dwAvail), &dwRead, NULL) || !dwRead)
						// error, the child process might ended
						break;

					buf[dwRead] = 0;
					strResult += buf;
				}
			} //for

			CloseHandle(hPipeWrite);
			CloseHandle(hPipeRead);
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);

			std::wstring wide_result = std::wstring(strResult.begin(), strResult.end());
			std::vector<char> buffer;
			std::stringstream  ss(strResult);
			if (!ss)
			{
				printf("failed to open a stream for command result. err: %d\n", GetLastError());
				return std::make_tuple(-1, std::vector<char>());
			}

			char buf[512];
			while (ss.read(buf, sizeof(buf)).gcount() > 0)
				buffer.insert(buffer.end(), buf, buf + ss.gcount());

			return std::make_tuple(0, buffer);

		}

		bool SilentProcessRunner::execute(std::shared_ptr<request> req, std::shared_ptr<reply> rep)
		{
			if (req->action_type.compare(wxhtpconstants::ACTION_TYPE::COMMAND_PROMPT))
			{
				return false;
			}
			int code; std::vector<char> buffer;
			std::wstring wcommand = std::wstring(req->content.begin(), req->content.end());
			//std::vector<std::wstring> splitted_wcommand = string_utils::split<std::wstring>(wcommand, L"%%");
			//wcommand = string_utils::join(splitted_wcommand, L" ");

			std::tie(code, buffer) = SilentProcessRunner::run_command(wcommand);

			if (buffer.empty())
			{
				rep = reply::stock_reply(reply::internal_server_error);
				return true;
			}

			rep->status = reply::ok;

			std::string s(buffer.data(), buffer.size());
			rep->content.append(s);
			rep->headers.resize(3);
			rep->headers[0].name = "Tag";
			rep->headers[0].value = std::string(req->dictionary_headers.at("Tag"));
			rep->headers[1].name = "Content-Type";
			rep->headers[1].value = mime_types::extension_to_type("text/plain");
			rep->headers[2].name = "Content-Length";
			rep->headers[2].value = std::to_string(rep->content.size());

			return true;
		}
	}
}
