#pragma once

#include <iostream>
#include "../common/TcpServer/TcpService.cpp"

using namespace std;

class WorkerNode : public TcpService {
	public:
		WorkerNode(std::shared_ptr<asio::ip::tcp::socket> socket) : TcpService(socket) {
			lastReply = std::time(0);
			state = ready;
			uid = boost::lexical_cast<std::string>(boost::uuids::random_generator()());
		};

		string uid;
		enum workerState { ready, working };
		workerState state;

		chrono::duration<double> timeSinceLastReply();
		void ping();
		void start();

	private:
		void parseReceivedMessage(std::shared_ptr<string> message);

		std::time_t lastReply;

};

void WorkerNode::ping() {
	writeAsync(make_shared<string>("PING"));
}

void WorkerNode::start() {
	cout << "Listening to worker " << uid << endl;
	//readAsyncContinuously(&parseReceivedMessage);
}

void WorkerNode::parseReceivedMessage(std::shared_ptr<string> message) {
	cout << "Received from worker " << uid << ": " << message << endl;
}