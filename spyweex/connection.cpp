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
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/move/move.hpp>
#include "request_handler.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "spyweex.h"

namespace http {
namespace server {

connection::connection(boost::asio::ip::tcp::socket sock, boost::asio::io_service& io_ptr)
  : socket_(boost::move(sock)),
	io_ptr_(io_ptr),
	remote_ep(socket_.remote_endpoint()),
	work(new boost::asio::io_service::work(io_ptr)),
	request_handler_strand(io_ptr)
{
	request_handler_.reset(new request_handler(socket_, io_ptr_));
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

ip::tcp::socket& connection::socket()
{
	return socket_;
}

void connection::connection_dropped()
{
	const boost::shared_ptr<boost::asio::deadline_timer> t(new boost::asio::deadline_timer(io_ptr_));
	t->expires_from_now(boost::posix_time::seconds(2));
	t->async_wait(
		boost::bind(
			&connection::async_retry_connect,
			this,
			boost::asio::placeholders::error,
			t));
}

void connection::async_retry_connect(const boost::system::error_code& e, boost::shared_ptr<boost::asio::deadline_timer> timer)
{
	if (!e)
	{
		timer.reset();
		if (socket_.is_open())
		{
			stop(); // closes all asyncronous operations asociated with socket
		}
		boost::system::error_code ec;
		socket_.open(boost::asio::ip::tcp::v4(), ec);
		if (!ec)
			socket_.async_connect(remote_ep,
				boost::bind(
					&connection::async_retry_connect_handler,
					this,
					boost::asio::placeholders::error
					));
	}
}

void connection::async_retry_connect_handler(const boost::system::error_code& e)
{
	if (!e)
	{
		this->prepare_data_placeholders();
		this->start();
	}
	else
	{
		this->connection_dropped();
	}
}

void connection::prepare_data_placeholders()
{
	request_parser_.reset();
	request_.clear_fields();
	reply_.clear_fields();
}

void connection::do_async_read()
{
	prepare_data_placeholders();
	socket_.async_read_some(boost::asio::buffer(buffer_),
		boost::bind(&connection::handle_read, shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

void connection::on_strand_completed()
{
	
}

void connection::dispatch_task(std::shared_ptr<request> copy_of_request)
{
	request_handler_->handle_request(copy_of_request);
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
			std::shared_ptr<request> copy_of_request(new request(request_));
			request_handler_->handle_request(copy_of_request);
			do_async_read();
			//request_handler_strand.post(
			//	boost::bind(&connection::dispatch_task, this, copy_of_request));
			//request_handler_strand.post(boost::bind(&connection::do_async_read, this));
		}
		else if (result == request_parser::bad)
		{
			std::shared_ptr<request> copy_of_request(new request(request_));
			std::shared_ptr<reply> copy_of_reply(new reply(reply_));
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
	else if (e == boost::asio::error::eof)
	{
		connection_dropped();
	}
}

void connection::handle_write(std::shared_ptr<request> req, std::shared_ptr<reply> rep, const boost::system::error_code& e, std::size_t bytes)
{
	req.reset();
	rep.reset();

	//	//Initiate graceful connection closure.
	//	boost::system::error_code ignored_ec;
	//	socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
}


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
//#pragma optimize("", on)

} // namespace server
} // namespace http