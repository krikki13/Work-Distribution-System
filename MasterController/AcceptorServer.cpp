#include <iostream>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include "tcp_connection.cpp"

using namespace std;
using namespace boost::asio::ip;

class AcceptorServer {
	public:
        AcceptorServer(boost::asio::io_context& io_context) : io_context_(io_context),
            acceptor_(io_context, tcp::endpoint(tcp::v4(), 13)) {
            start_accept();
        }

	private:
		boost::asio::io_context& io_context_;
        tcp::acceptor acceptor_;

        void start_accept();
   
        void handle_accept(tcp_connection::pointer new_connection,
            const boost::system::error_code& error);
};


void AcceptorServer::start_accept() {
    tcp_connection::pointer new_connection = tcp_connection::create(io_context_);

    acceptor_.async_accept(new_connection->socket(),
        boost::bind(&AcceptorServer::handle_accept, this, new_connection,
        boost::asio::placeholders::error));
}

void AcceptorServer::handle_accept(tcp_connection::pointer new_connection,
    const boost::system::error_code& error) {
    if (!error) {
        new_connection->start();
    }

    start_accept();
}