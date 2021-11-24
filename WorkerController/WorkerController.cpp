// Worker.cpp : Defines the entry point for the application.
//

#include "WorkerController.h"

string uid = boost::lexical_cast<std::string>(boost::uuids::random_generator()());

int main() {
	cout << "Worker initializing" << endl;
	cout << uid << endl;

    TcpClient client(1);
    client.initializeConnection("127.0.0.1", 13);
    client.read();
    std::this_thread::sleep_for(std::chrono::seconds(3));
    auto s = make_shared<string>("How are ya\n");
    client.write(s);

    for (;;)
        std::this_thread::sleep_for(std::chrono::seconds(60));
    client.stop();

	return 0;
}
