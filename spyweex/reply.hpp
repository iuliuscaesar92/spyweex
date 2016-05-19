//
// reply.hpp
// ~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_REPLY_HPP
#define HTTP_REPLY_HPP

#include <string>
#include <vector>
#include <boost/asio.hpp>
#include "header.hpp"


namespace http {
namespace server {

/// A reply to be sent to a client.
struct reply
{
  /// The status of the reply.
  enum status_type
  {
    ok = 200,
    created = 201,
    accepted = 202,
    no_content = 204,
    multiple_choices = 300,
    moved_permanently = 301,
    moved_temporarily = 302,
    not_modified = 304,
    bad_request = 400,
    unauthorized = 401,
    forbidden = 403,
    not_found = 404,
    internal_server_error = 500,
    not_implemented = 501,
    bad_gateway = 502,
    service_unavailable = 503
  } status;
  
  std::string status_line;

  /// The headers to be included in the reply.
  std::vector<header> headers;

  /// The content to be sent in the reply.
  std::string content;

  /// Convert the reply into a vector of buffers. The buffers do not own the
  /// underlying memory blocks, therefore the reply object must remain valid and
  /// not be changed until the write operation has completed.
  std::vector<boost::asio::const_buffer> to_buffers();

  /// Get a stock reply.
  static std::shared_ptr<reply> stock_reply(status_type status);

  void clear_fields()
  {
	  this->status_line.clear();
	  this->headers.clear();
	  this->content.clear();
  }

  reply()
  {
	  this->status = status_type::ok;
  }
  reply(const reply& rep)
  {
	  this->status = rep.status;
	  this->status_line = rep.status_line;
	  this->headers = rep.headers;
	  this->content = rep.content;
  }

  reply& operator=(const reply& obj) {
	  if(this == &obj)
	  {
		  return *this;
	  }
	  this->status = obj.status;
	  this->status_line = obj.status_line;
	  this->headers = obj.headers;
	  this->content = obj.content;

	  return *this;

  }
};

namespace status_strings {

	const std::string ok =
		"WXHTP/1.0 200 OK";
	const std::string created =
		"HTTP/1.0 201 Created\r\n";
	const std::string accepted =
		"HTTP/1.0 202 Accepted\r\n";
	const std::string no_content =
		"HTTP/1.0 204 No Content\r\n";
	const std::string multiple_choices =
		"HTTP/1.0 300 Multiple Choices\r\n";
	const std::string moved_permanently =
		"HTTP/1.0 301 Moved Permanently\r\n";
	const std::string moved_temporarily =
		"HTTP/1.0 302 Moved Temporarily\r\n";
	const std::string not_modified =
		"HTTP/1.0 304 Not Modified\r\n";
	const std::string bad_request =
		"HTTP/1.0 400 Bad Request\r\n";
	const std::string unauthorized =
		"HTTP/1.0 401 Unauthorized\r\n";
	const std::string forbidden =
		"HTTP/1.0 403 Forbidden\r\n";
	const std::string not_found =
		"HTTP/1.0 404 Not Found\r\n";
	const std::string internal_server_error =
		"HTTP/1.0 500 Internal Server Error\r\n";
	const std::string not_implemented =
		"HTTP/1.0 501 Not Implemented\r\n";
	const std::string bad_gateway =
		"HTTP/1.0 502 Bad Gateway\r\n";
	const std::string service_unavailable =
		"HTTP/1.0 503 Service Unavailable\r\n";

	boost::asio::const_buffer to_buffer(reply::status_type status);

} // namespace status_strings



} // namespace server
} // namespace http

#endif // HTTP_REPLY_HPP