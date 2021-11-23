// Master.cpp : Defines the entry point for the application.
//

#include "Master.h"

using namespace std;
using namespace boost::asio::ip;



int main() {
	cout << "Master initializing" << endl;

    try {
        boost::asio::io_context io_context;
        AcceptorServer server(io_context);

        io_context.run();
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

	return 0;
}