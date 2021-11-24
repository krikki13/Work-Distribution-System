#pragma once

#include <iostream>
#include "TcpService.cpp"

using namespace std;

class WorkerNode : public TcpService {
	public:
		WorkerNode(std::shared_ptr<asio::ip::tcp::socket> socket) : TcpService(socket) {};

		enum workerState { ready, working };
		workerState state;

		chrono::duration<double> timeSinceLastReply();
		void ping();

	private:
		std::time_t lastReply;

};

