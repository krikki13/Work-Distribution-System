#pragma once

#include <iostream>
#include "../common/TcpServer/TcpService.cpp"
#include "../common/WorkerState.cpp"

#define PING_INTERVAL 5000 //ms

using namespace std;

// Class that is used to communicate with a worker. If there has been no communication with worker for a while, this class will ping it, to verify worker is still working.
class WorkerNode : public TcpService {
	public:
		WorkerNode(std::shared_ptr<asio::ip::tcp::socket> socket) : TcpService(socket) {
			lastReply = std::chrono::high_resolution_clock::now();
			state = ready;
			pingSent = false;
			uid = boost::lexical_cast<std::string>(boost::uuids::random_generator()());
		};

		string uid;
		WorkerState state;

		void start();
		void update();

	private:
		void parseReceivedMessage(std::shared_ptr<string> message);
		void ping();

		bool pingSent;
		std::chrono::steady_clock::time_point pingSentAt;
		std::chrono::steady_clock::time_point lastReply;
};

void WorkerNode::start() {
	cout << "Listening to worker " << uid << endl;
	readAsyncContinuously([this](std::shared_ptr<string> message) { parseReceivedMessage(message); });
}

void WorkerNode::update() {
	if (stopped) return;
	try {
		auto timeNow = std::chrono::high_resolution_clock::now();

		if (pingSent) {
			double timeSincePing = std::chrono::duration<double, std::milli>(timeNow - lastReply).count();
			if (timeSincePing > PING_INTERVAL) {
				cout << "No reply from " << uid << " for too long. PANIC!!!" << endl; // TODO panic
			}
		} else {
			double timeSinceLastReply = std::chrono::duration<double, std::milli>(timeNow - lastReply).count();
			if (timeSinceLastReply > PING_INTERVAL) {
				ping();
			}
		}

		if(state == ready) {
			writeAsync(make_shared<string>("TASK 1"));
		}
	}catch(ServerException e) {
		cout << "Exception in worker update (" << uid << "): " << e.what() << endl;
	}
}

void WorkerNode::ping() {
	writeAsync(make_shared<string>("PING"));
	pingSent = true;
	pingSentAt = std::chrono::high_resolution_clock::now();
}

void WorkerNode::parseReceivedMessage(std::shared_ptr<string> message) {
	cout << "Received from worker " << uid << ": " << *message << endl;
	vector<string> msg;
	boost::split(msg, *message, boost::is_any_of("\\s "));
	if(msg[0] == "PONG") {
		pingSent = false;
		state = workerStateFromString(msg[1]);
	} else {
		cout << "Unknown command: " << *message << endl;
	}

	lastReply = std::chrono::high_resolution_clock::now();
}
