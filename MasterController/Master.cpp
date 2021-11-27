// Master.cpp : Defines the entry point for the application.
//

#include "Master.h"

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
		vector<WorkerNode*> workerNodes;
		
		std::unique_ptr<asio::io_service::work> m_work;
		asio::io_service m_ios;
	
};

void MasterController::Start() {
	cout << "Master initializing" << endl;

	unsigned short port_num = 13;
	acceptorServer.reset(new NodeAcceptorServer(m_ios, port_num,
		[this](WorkerNode* newWorker) {
			std::unique_lock<std::mutex> lock(workerNodeListGuard);
			workerNodes.push_back(newWorker);
			lock.unlock();
			return true;
		}));
	acceptorServer->Start();
	std::unique_ptr<std::thread> acceptorServerThread(
				new std::thread([this]() { m_ios.run(); }));

	while(true) {
		std::this_thread::sleep_for(std::chrono::seconds(5));

		if (!workerNodes.empty()) {
			cout << "Pinging" << endl;
			std::unique_lock<std::mutex> lock(workerNodeListGuard);
			for (auto worker : workerNodes) {
				worker->ping();
			}
			lock.unlock();
		}
		std::this_thread::sleep_for(std::chrono::seconds(500));
	}
}

int main() {
	MasterController controller;
	controller.Start();

	return 0;
}