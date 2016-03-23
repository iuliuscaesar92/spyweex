//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include "stdafx.h"
//#include "server.hpp"
#include <signal.h>
#include <utility>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>


using namespace boost::asio;
using boost::asio::ip::tcp;
namespace http {
namespace server {

server::server(const std::string& address, const std::string& port,
    const std::string& doc_root)
  : io_service_(),
    signals_(io_service_),
    acceptor_(io_service_),
    connection_manager_(),
    socket_(io_service_),
    request_handler_(doc_root)
{
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
  //boost::asio::ip::tcp::resolver resolver(io_service_);
  //boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(
	 // ip::tcp::endpoint(
		//  ip::address::from_string(address),
		//  atoi(port.c_str())
		//  )
	 // );

  //// Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
  //boost::asio::ip::tcp::resolver resolver(io_service_);
  //boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(
	 // ip::tcp::endpoint(
		//ip::address::from_string(address),
		//	atoi(port.c_str())
	 // )
  //);
  //acceptor_.open(endpoint.protocol());
  //acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  //acceptor_.bind(endpoint);
  //acceptor_.listen();

  //do_accept();
}

void server::run()
{
  // The io_service::run() call will block until all asynchronous operations
  // have finished. While the server is running, there is always at least one
  // asynchronous operation outstanding: the asynchronous accept call waiting
  // for new incoming connections.
  io_service_.run();
}

void server::handle_reverse_connection(const boost::system::error_code& err)
{
	//if (!err)
	//{
	//	// The connection was successful. Send the request.
	//	boost::asio::async_write(socket_, request_,
	//		boost::bind(&server::handle_write_request, this,
	//			boost::asio::placeholders::error));
	//}
	//else
	//{
	//	std::cout << "Error: " << err.message() << "\n";
	//}
}


//void server::do_accept()
//{
//  acceptor_.async_accept(socket_,
//      [this](boost::system::error_code ec)
//      {
//        // Check whether the server was stopped by a signal before this
//        // completion handler had a chance to run.
//        if (!acceptor_.is_open())
//        {
//          return;
//        }
//
//        if (!ec)
//        {
//          connection_manager_.start(std::make_shared<connection>(
//              std::move(socket_), connection_manager_, request_handler_));
//        }
//
//        do_accept();
//      });
//}

void server::do_await_stop()
{
  signals_.async_wait(
      [this](boost::system::error_code /*ec*/, int /*signo*/)
      {
        // The server is stopped by cancelling all outstanding asynchronous
        // operations. Once all operations have finished the io_service::run()
        // call will exit.
        acceptor_.close();
        connection_manager_.stop_all();
      });
}

} // namespace server
} // namespace http