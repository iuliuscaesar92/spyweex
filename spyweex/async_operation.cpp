
#include "async_operation.hpp"

namespace http
{
	namespace server
	{
		
		void async_op::start()
		{
			{
				boost::recursive_mutex::scoped_lock lk(cs_);
				if (started_)
					return;
				started_ = true;
			}
			boost::thread t(boost::bind(&async_op::run, this));
		}

		void async_op::run()
		{
			while (true) {
				{
					boost::recursive_mutex::scoped_lock lk(cs_);
					if (!started_) break;
				}

				boost::this_thread::sleep(boost::posix_time::millisec(10));
				operation cur;
				{
					boost::recursive_mutex::scoped_lock lk(cs_);
					if (!ops_.empty())
					{
						cur = ops_[0];
						ops_.erase(ops_.begin());
					}
				}

				if (cur.service)
				{
					boost::system::error_code err = cur.op();
					cur.service->post(boost::bind(cur.completion, err));
				}
			}
			self_.reset();
		}

		void async_op::add(op_func op, completion_func completion, io_service & service)
		{
			self_ = shared_from_this();
			boost::recursive_mutex::scoped_lock lk(cs_);
			ops_.push_back(operation(service, op, completion));
			if (!started_) start();
		}

		void async_op::stop()
		{
			boost::recursive_mutex::scoped_lock lk(cs_);
			started_ = false;
			ops_.clear();
		}
	}
}