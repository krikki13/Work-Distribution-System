// Worker.cpp : Defines the entry point for the application.
//

#include "Worker.h"

using namespace std;
using boost::asio::ip::tcp;

string uid = boost::lexical_cast<std::string>(boost::uuids::random_generator()());

enum state { ready, working };
state workerState;

int main() {
	cout << "Worker initializing" << endl;
	cout << uid << endl;

	workerState = ready;

	boost::asio::io_context io_context;
	tcp::resolver resolver(io_context);
	tcp::resolver::results_type endpoints = resolver.resolve("localhost", "daytime");

	tcp::socket socket(io_context);
	boost::asio::connect(socket, endpoints);

	for (;;)
	{
		boost::array<char, 128> buf;
		boost::system::error_code error;

		size_t len = socket.read_some(boost::asio::buffer(buf), error);

			if (error == boost::asio::error::eof)
				break; // Connection closed cleanly by peer.
			else if (error)
				throw boost::system::system_error(error); // Some other error.

		std::cout.write(buf.data(), len);
	}

	return 0;
}
