#pragma once

#include <iostream>
#include "../common/TcpServer/TcpService.cpp"

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
		enum workerState { ready, working };
		workerState state;

		void start();
		void update();

	private:
		void parseReceivedMessage(std::shared_ptr<string> message);
		void ping();

		bool pingSent;
		std::chrono::steady_clock::time_point pingSentAt;
		std::chrono::steady_clock::time_point lastReply;

		workerState parseWorkerState(string& state) {
			if(state == "READY") {
				return ready;
			} else if(state == "WORKING") {
				return working;
			}
			throw "Invalid worker state";
		}
};

void WorkerNode::start() {
	cout << "Listening to worker " << uid << endl;
	readAsyncContinuously([this](std::shared_ptr<string> message) { parseReceivedMessage(message); });
}

void WorkerNode::update() {
	auto timeNow = std::chrono::high_resolution_clock::now();

	if(pingSent) {
		double timeSincePing = std::chrono::duration<double, std::milli>(lastReply - timeNow).count();
		if (timeSincePing > PING_INTERVAL) {
			cout << "No reply for too long"; // TODO panic
		}
	} else {
		double timeSinceLastReply = std::chrono::duration<double, std::milli>(lastReply - timeNow).count();
		if (timeSinceLastReply > PING_INTERVAL) {
			ping();
		}
	}
}

void WorkerNode::ping() {
	writeAsync(make_shared<string>("PING"));
	pingSent = true;
	pingSentAt = std::chrono::high_resolution_clock::now();
}

void WorkerNode::parseReceivedMessage(std::shared_ptr<string> message) {
	cout << "Received from worker " << uid << ": " << message << endl;
	vector<string> msg;
	boost::split(msg, *message, boost::is_any_of("\\s "));
	if(msg[0] == "PONG") {
		pingSent = false;
		state = parseWorkerState(msg[1]);
	} else {
		cout << "Unknown command: " << *message << endl;
	}

	lastReply = std::chrono::high_resolution_clock::now();
}
