// Master.cpp : Defines the entry point for the application.
//

#include "Master.h"

#define LOOP_INTERVAL 5000 //ms

using namespace std;
using namespace boost;


class MasterController {
	public:
		void Start();

	private:
		unique_ptr<NodeAcceptorServer> acceptorServer;
		std::mutex workerNodeListGuard;
		unordered_map<string, WorkerNode*> workerNodes;

		void loop();
		void stop();
		std::unique_ptr<asio::io_service::work> m_work;
		asio::io_service m_ios;
		std::unique_ptr<std::thread> acceptorServerThread;
};

void MasterController::Start() {
	cout << "Master initializing" << endl;

	try {
		m_work.reset(new asio::io_service::work(m_ios));

		unsigned short port_num = 13;
		acceptorServer.reset(new NodeAcceptorServer(m_ios, port_num,
			[this](WorkerNode* newWorker) {
				std::unique_lock<std::mutex> lock(workerNodeListGuard);
				if (workerNodes.find(newWorker->uid) != workerNodes.end()) {
					cout << "Worker node with UID " << newWorker->uid << " already exists in the worker list" << endl;
					lock.unlock();
					return false;
				}
				workerNodes[newWorker->uid] = newWorker;
				lock.unlock();
				return true;
			}));
		acceptorServer->Start();
		acceptorServerThread = make_unique<std::thread>([this]() { m_ios.run(); });

		loop();
	} catch(ServerException e) {
		stop();
		cout << e.what() << endl;
		cout << "TcpServer stopped. Exiting" << endl;
	}
}

void MasterController::loop() {
	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(LOOP_INTERVAL));

		if (!workerNodes.empty()) {
			cout << "Update " << endl;
			vector<string> toErase;
			std::unique_lock<std::mutex> lock(workerNodeListGuard);
			for (auto worker = workerNodes.begin(); worker != workerNodes.end(); ++worker) {
				if(worker->second->isStopped()) {
					cout << "Removing worker node " << worker->second->uid << " because it has stopped" << endl;
					//workerNodes.erase(worker);
					toErase.push_back(worker->second->uid);
				} else {
					worker->second->update();
				}
			}
			for(string& uid : toErase) {
				workerNodes.erase(uid);
			}
			lock.unlock();
		}
	}
}

void MasterController::stop() {
	acceptorServer->Stop();
	m_ios.stop();

	acceptorServerThread->join();
}

int main() {
	MasterController controller;
	controller.Start();

	return 0;
}