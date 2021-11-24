#pragma once

#include <iostream>
#include "../common/TcpServer/TcpService.cpp"

using namespace std;

class WorkerNode : public TcpService {
	public:
		WorkerNode(std::shared_ptr<asio::ip::tcp::socket> socket) : TcpService(socket) {
			lastReply = std::time(0);
			state = ready;
		};

		enum workerState { ready, working };
		workerState state;

		chrono::duration<double> timeSinceLastReply();
		void ping();

	private:
		std::time_t lastReply;

};

