#include <iostream>
#include <string>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

using namespace boost::asio::ip;

class tcp_connection : public boost::enable_shared_from_this<tcp_connection> {
	public:
	    typedef boost::shared_ptr<tcp_connection> pointer;

		tcp_connection(boost::asio::io_context& io_context)
			: socket_(io_context) {
		}

	    static pointer create(boost::asio::io_context& io_context) {
	        return pointer(new tcp_connection(io_context));
	    }

	    tcp::socket& socket() {
	        return socket_;
	    }

		void start();

		private:
			

			void handle_write(const boost::system::error_code& /*error*/,
				size_t /*bytes_transferred*/) {
			}

			tcp::socket socket_;
			std::string message_;
};

void tcp_connection::start() {

	message_ = "HELLO THERE";

	boost::asio::async_write(socket_, boost::asio::buffer(message_),
		boost::bind(&tcp_connection::handle_write, shared_from_this(),
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));

}