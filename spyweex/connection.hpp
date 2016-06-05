//
// connection.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//


#ifndef HTTP_CONNECTION_HPP
#define HTTP_CONNECTION_HPP

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "reply.hpp"
#include "request.hpp"
#include "request_handler.hpp"
#include "request_parser.hpp"

namespace http {
namespace server {

class request_handler;

/// Represents a single connection from a client.
class connection : public boost::enable_shared_from_this<connection>,
	private boost::noncopyable
{
public:

  typedef boost::shared_ptr<request_handler> request_handler_ptr;

  /// Construct a connection with the given socket.
  explicit connection(boost::asio::ip::tcp::socket socket, boost::asio::io_service& io_ptr);

  /// Get the socket associated with the connection.
  boost::asio::ip::tcp::socket& socket();

  /// Start the first asynchronous operation for the connection.
  void start();

  /// Stop all asynchronous operations associated with the connection.
  void stop();

  void prepare_data_placeholders();

  void connection_dropped();

  void async_retry_connect(const boost::system::error_code& e, boost::shared_ptr<boost::asio::deadline_timer> timer);

  void async_retry_connect_handler(const boost::system::error_code& e);

  void on_strand_completed();

  void dispatch_task(std::shared_ptr<request> copy_of_request);

private:

  /// Perform an asynchronous read operation.
  void do_async_read();

  /// Handle completion of a read operation.
  void handle_read(const boost::system::error_code& e,
	  std::size_t bytes_transferred);

  /// Handle completion of a write operation.
  void handle_write(std::shared_ptr<request> req, std::shared_ptr<reply> rep, const boost::system::error_code& e, std::size_t bytes);

  /// Socket for the connection.
  boost::asio::ip::tcp::socket socket_;

  /// Buffer for incoming data.
  boost::array<char, 8192> buffer_;

  /// The incoming request.
  request request_;

  /// The parser for the incoming request.
  request_parser request_parser_;

  /// The reply to be sent back to the client.
  reply reply_;

  request_handler_ptr request_handler_;

  boost::asio::io_service& io_ptr_;

  boost::asio::ip::tcp::endpoint remote_ep;

  auto_ptr<boost::asio::io_service::work> work;

  strand request_handler_strand;

  typedef boost::shared_ptr<connection> connection_ptr;

};


} // namespace server
} // namespace http

#endif // HTTP_CONNECTION_HPP