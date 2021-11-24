// Master.cpp : Defines the entry point for the application.
//

#include "Master.h"

using namespace std;
using namespace boost;



class MasterController {
	public:
		void Start();

	private:
		unique_ptr<AcceptorServer> acceptorServer;
		vector<WorkerNode> workerNodes;
	
};

void MasterController::Start() {
	cout << "Master initializing" << endl;

	asio::io_service io;
	acceptorServer.reset(new AcceptorServer(io));
	
	acceptorServer->setOnWorkerNodeAdded([](WorkerNode* worker)
	{
			cout << "Added";
	});
	acceptorServer->Start();

	for(;;)
		std::this_thread::sleep_for(std::chrono::seconds(60));
}

int main() {
	MasterController controller;
	controller.Start();

	return 0;
}