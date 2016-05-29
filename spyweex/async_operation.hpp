
#ifndef HTTP_ASYNC_OPERATION_HPP
#define HTTP_ASYNC_OPERATION_HPP

#include <string>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>

using namespace boost::asio;

namespace http {
	namespace server {

		struct async_op : boost::enable_shared_from_this<async_op>, private boost::noncopyable
		{
			typedef boost::shared_ptr<async_op> ptr;

			static ptr new_() {
				return ptr(new async_op);
			}

			private:
				typedef boost::function<void(boost::system::error_code)> completion_func;

				typedef boost::function<boost::system::error_code()> op_func;

				async_op() : started_(false) {}

				struct operation {
					operation(io_service & service, op_func op, completion_func completion)
						: service(&service), op(op), completion(completion)
						//, work(new io_service::work(service))
					{}
					operation() : service(0) {}
					public:
						boost::asio::io_service * service;
						op_func op;
						completion_func completion;

					//typedef boost::shared_ptr<io_service::work> work_ptr;
					//work_ptr work;
				};

				typedef std::vector<operation> array_ops;

				void start();

				void run();

			public:
				void add(op_func op, completion_func completion, io_service& service);

				void stop();

			private:
				array_ops ops_;

				bool started_;

				boost::recursive_mutex cs_;

				ptr self_;
		};

	}
}

#endif // HTTP_ASYNC_OPERATION_HPP