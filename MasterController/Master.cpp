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
		unique_ptr<AcceptorServer> acceptorServer;
		vector<WorkerNode> workerNodes;
		std::unique_ptr<asio::io_service::work> m_work;
		asio::io_service m_ios;
	
};

void MasterController::Start() {
	cout << "Master initializing" << endl;

	unsigned short port_num = 13;
	acceptorServer.reset(new AcceptorServer(m_ios, port_num));
	
	acceptorServer->setOnWorkerNodeAdded([](WorkerNode* worker)
	{
			cout << "Added in MasterController" << endl;
	});
	acceptorServer->Start();
	m_ios.run();

	cout << "m_ios.run(); blocks thread" << endl;
	for(;;)
		std::this_thread::sleep_for(std::chrono::seconds(60));
}

int main() {
	MasterController controller;
	controller.Start();

	return 0;
}