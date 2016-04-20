//
// connection.cpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "connection.hpp"
//#include <vector>
#include <boost/bind.hpp>
#include "request_handler.hpp"


namespace http {
namespace server {

connection::connection(boost::asio::ip::tcp::socket socket, request_handler& handler)
  : socket_(std::move(socket)),
    request_handler_(handler)
{
	//boost::asio::socket_base::keep_alive option(true);
	//socket_.set_option(option);
}

void connection::start()
{
  boost::asio::ip::tcp::socket::keep_alive kl(true);
  boost::asio::ip::tcp::socket::enable_connection_aborted eca(true);
  socket_.set_option(eca);
  socket_.set_option(kl);
  socket_.async_read_some(boost::asio::buffer(buffer_),
	 boost::bind(&connection::handle_read, shared_from_this(),
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
}

void connection::stop()
{
  socket_.close();
}

void connection::do_async_read()
{
	request_parser_.reset();
	request_.clear_fields();
	reply_.clear_fields();
	socket_.async_read_some(boost::asio::buffer(buffer_),
		boost::bind(&connection::handle_read, shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

void connection::handle_read(const boost::system::error_code& e,
	std::size_t bytes_transferred)
{
	if (!e)
	{
		request_parser::result_type result;
		std::tie(result, std::ignore) = request_parser_.parse(
			request_, buffer_.data(), buffer_.data() + bytes_transferred);
		if (result == request_parser::good)
		{
			// make copy of request and reply
			std::shared_ptr<request> copy_of_request = std::make_shared<request>(request_);
			std::shared_ptr<reply> copy_of_reply = std::make_shared<reply>(reply_);
			do_async_read();

			request_handler_.handle_request(copy_of_request, copy_of_reply);
			boost::asio::async_write(socket_, 
				copy_of_reply->to_buffers(),
				boost::bind(&connection::handle_write, shared_from_this(), 
					copy_of_request, copy_of_reply,
					boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
				);
		}
		else if (result == request_parser::bad)
		{
			std::shared_ptr<request> copy_of_request = std::make_shared<request>(request_);
			std::shared_ptr<reply> copy_of_reply = std::make_shared<reply>(reply_);
			do_async_read();
			reply_ = *(reply::stock_reply(reply::bad_request).get());
			boost::asio::async_write(socket_, copy_of_reply->to_buffers(),
				boost::bind(&connection::handle_write, shared_from_this(),
					copy_of_request, copy_of_reply,
					boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
				);
		}
		else
		{
			socket_.async_read_some(boost::asio::buffer(buffer_),
				boost::bind(&connection::handle_read, shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
		}
	}
	else if (e != boost::asio::error::operation_aborted)
	{
		//connection_manager_.stop(shared_from_this());
	}
	// If an error occurs then no new asynchronous operations are started. This
	// means that all shared_ptr references to the connection object will
	// disappear and the object will be destroyed automatically after this
	// handler returns. The connection class's destructor closes the socket.
}

void connection::handle_write(std::shared_ptr<request> req, std::shared_ptr<reply> rep, const boost::system::error_code& e, std::size_t bytes)
{
	req.reset();
	rep.reset();
	//std::string s(this->reply_.to_buffers(), this->reply_.to_buffers().data());
	//int a = this->reply_.to_buffers().size();
	//if(this->reply_.to_buffers().size() == bytes)
	//{
	


	//socket_.async_read_some(boost::asio::buffer(buffer_),
	//	boost::bind(&connection::handle_read, shared_from_this(),
	//		boost::asio::placeholders::error,
	//		boost::asio::placeholders::bytes_transferred));




	//}
	//if(e == boost::asio::error::no_data)
	//{
	//	socket_.async_read_some(boost::asio::buffer(buffer_),
	//		boost::bind(&connection::handle_read, shared_from_this(),
	//			boost::asio::placeholders::error,
	//			boost::asio::placeholders::bytes_transferred));
	//}
	//if (!e)
	//{
	//	std::cerr << "Error type:" << e;
	//	//socket_.async_read_some(boost::asio::buffer(buffer_),
	//	//	boost::bind(&connection::handle_read, shared_from_this(),
	//	//		boost::asio::placeholders::error,
	//	//		boost::asio::placeholders::bytes_transferred));
	//	//Initiate graceful connection closure.
	//	boost::system::error_code ignored_ec;
	//	socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);

	//}
	//else if (e != boost::asio::error::operation_aborted)
	//{
	//	this->stop();
	//}
	// No new asynchronous operations are started. This means that all shared_ptr
	// references to the connection object will disappear and the object will be
	// destroyed automatically after this handler returns. The connection class's
	// destructor closes the socket.
}

//void connection::do_read()
//{
//  auto self(shared_from_this());
//  socket_.async_read_some(boost::asio::buffer(buffer_),
//      [this, self](boost::system::error_code ec, std::size_t bytes_transferred)
//      {
//        if (!ec)
//        {
//          request_parser::result_type result;
//          std::tie(result, std::ignore) = request_parser_.parse(
//              request_, buffer_.data(), buffer_.data() + bytes_transferred);
//
//          if (result == request_parser::good)
//          {
//            request_handler_.handle_request(request_, reply_);
//            do_write();
//          }
//          else if (result == request_parser::bad)
//          {
//            reply_ = reply::stock_reply(reply::bad_request);
//            do_write();
//          }
//          else
//          {
//            do_read();
//          }
//        }
//   //     else if (ec != boost::asio::error::operation_aborted)
//   //     {
//			//std::cout << "Error: " << ec.message() << "\n";
//			//this->stop();
//   //     }
//      });
//}
//#pragma optimize( "", off )
//void connection::do_write()
//{
//  auto self(shared_from_this());
//
//  boost::asio::async_write(socket_, reply_.to_buffers(),
//  [this, self](boost::system::error_code ec, std::size_t)
//  {
//	  if (!ec)
//	  {
//		  
//		  //boost::system::error_code ignored_ec;
//
//		  //// Initiate graceful connection closure. Don't need it here.
//		  //socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both,
//		  //  ignored_ec);
//	  }
//	  //if (ec != boost::asio::error::operation_aborted)
//	  //{
//	  //  this->stop();
//	  //}
//
//  });
//
//
//  //boost::asio::async_write(socket_, reply_.to_buffers(),
//  //    [this, self](boost::system::error_code ec, std::size_t)
//  //    {
//		////do_read();
//  //      if (!ec)
//  //      {
//
//
//  //        //boost::system::error_code ignored_ec;
//
//		//  // Initiate graceful connection closure. Don't need it here.
//  //        //socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both,
//  //        //  ignored_ec);
//  //      }
//  //      //if (ec != boost::asio::error::operation_aborted)
//  //      //{
//  //      //  this->stop();
//  //      //}
//  //    });
//}
//#pragma optimize("", on)
} // namespace server
} // namespace http