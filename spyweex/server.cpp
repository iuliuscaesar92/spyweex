//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "server.hpp"
#include <windows.h>
#include <signal.h>
//#include <utility>
#include <shellapi.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <mhook-dll.cpp>

using namespace boost::asio;
using boost::asio::ip::tcp;
namespace http {
namespace server {

server::server(const std::string& address, const std::string& port,
    const std::string& doc_root)
  : io_service_(),
    signals_(io_service_),
    socket_(io_service_),
    request_handler_(doc_root)
{
  privileges::require(); // on Constructor - we gain the admin privileges :)

  // Register to handle the signals that indicate when the server should exit.
  // It is safe to register for the same signal multiple times in a program,
  // provided all registration for the specified signal is made through Asio.
  signals_.add(SIGINT);
  signals_.add(SIGTERM);
  #if defined(SIGQUIT)
	signals_.add(SIGQUIT);
  #endif // defined(SIGQUIT)

  do_await_stop();
  ip::tcp::endpoint ep(ip::address::from_string(address), atoi(port.c_str()));
  socket_.async_connect( ep,
  	boost::bind(&server::handle_reverse_connection, this,
  		boost::asio::placeholders::error));
}

server::~server()
{
	// on Destructor we release the privileges
	privileges::release();
}

void server::run()
{
  // The io_service::run() call will block until all asynchronous operations
  // have finished.
	io_service_.run();
}

void server::handle_reverse_connection(const boost::system::error_code& err)
{
	if (!err)
	{	
		new_connection_.reset(new connection(std::move(socket_), request_handler_));
		new_connection_->start();
	}
	else
	{
		std::cout << "Error: " << err.message() << "\n";
	}
}

void server::do_await_stop()
{
  signals_.async_wait(
      [this](boost::system::error_code /*ec*/, int /*signo*/)
      {
        // The server is stopped by cancelling all outstanding asynchronous
        // operations. Once all operations have finished the io_service::run()
        // call will exit.
        // acceptor_.close();
        // connection_manager_.stop_all();
      });
}

} // namespace server
} // namespace http