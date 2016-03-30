//
// server.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <string>
#include "connection.hpp"
#include "connection_manager.hpp"
#include "request_handler.hpp"

namespace http {
namespace server {

/// The top-level class of the HTTP server.
class server : private boost::noncopyable
{
public:
  /// Construct the server to listen on the specified TCP address and port, and
  /// serve up files from the given directory.
  explicit server(const std::string& address, const std::string& port,
      const std::string& doc_root);

  /// Run the server's io_service loop.
  void run();

private:

  /// Wait for a request to stop the server.
  void do_await_stop();

  /// Handle reverse tcp connection
  void handle_reverse_connection(const boost::system::error_code& err);

  /// The io_service used to perform asynchronous operations.
  boost::asio::io_service io_service_;

  /// The signal_set is used to register for process termination notifications.
  boost::asio::signal_set signals_;

  /// The next socket to be accepted.
  boost::asio::ip::tcp::socket socket_;

  /// The handler for all incoming requests.
  request_handler request_handler_;

  connection_ptr new_connection_;

};

} // namespace server
} // namespace http

#endif // HTTP_SERVER_HPP