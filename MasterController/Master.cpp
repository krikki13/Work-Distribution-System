// Master.cpp : Defines the entry point for the application.
//

#include "Master.h"

#define LOOP_INTERVAL 5000 //ms

using namespace std;
using namespace boost;


class MasterController {
	public:
		MasterController() {
			m_work.reset(new asio::io_service::work(m_ios));
		}

		void Start();

	private:
		unique_ptr<NodeAcceptorServer> acceptorServer;
		std::mutex workerNodeListGuard;
		unordered_map<string, WorkerNode*> workerNodes;

		void loop();
		std::unique_ptr<asio::io_service::work> m_work;
		asio::io_service m_ios;
	
};

void MasterController::Start() {
	cout << "Master initializing" << endl;

	unsigned short port_num = 13;
	acceptorServer.reset(new NodeAcceptorServer(m_ios, port_num,
		[this](WorkerNode* newWorker) {
			std::unique_lock<std::mutex> lock(workerNodeListGuard);
			if(workerNodes.find(newWorker->uid) != workerNodes.end()) {
				cout << "Worker node with UID " << newWorker->uid << " already exists in the worker list" << endl;
				lock.unlock();
				return false;
			}
			workerNodes[newWorker->uid] = newWorker;
			lock.unlock();
			return true;
		}));
	acceptorServer->Start();
	std::unique_ptr<std::thread> acceptorServerThread(
				new std::thread([this]() { m_ios.run(); }));

	loop();
}

void MasterController::loop() {
	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(LOOP_INTERVAL));

		if (!workerNodes.empty()) {
			cout << "Update " << endl;
			std::unique_lock<std::mutex> lock(workerNodeListGuard);
			for (auto& worker : workerNodes) {
				worker.second->update();
			}
			lock.unlock();
		}
	}
}

int main() {
	MasterController controller;
	controller.Start();

	return 0;
}