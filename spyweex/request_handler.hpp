//
// request_handler.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_REQUEST_HANDLER_HPP
#define HTTP_REQUEST_HANDLER_HPP

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/move/move.hpp>

#include "task_handler_interface.h"
#include "screenshot_taker.h"
#include <string>
#include "reply.hpp"
#include "request.hpp"

namespace http {
namespace server {

struct reply;
struct request;

/// The common handler for all incoming requests.
class request_handler : private boost::noncopyable
{
public:

  explicit request_handler(boost::asio::ip::tcp::socket& sock);

  /// Handle a request and produce a reply.
  //void handle_request(const request& req, reply& rep);
  void handle_request(std::shared_ptr<request> req, std::shared_ptr<reply> rep) const;

private:

  std::unique_ptr<TaskHandlerInterface> rootHandler;

  /// Socket for the connection.
  boost::asio::ip::tcp::socket &socket_;

};

} // namespace server
} // namespace http

#endif // HTTP_REQUEST_HANDLER_HPP