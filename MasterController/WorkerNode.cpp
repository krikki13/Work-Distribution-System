#include <iostream>

using namespace std;

class WorkerNode {
	public: 
		enum workerState { ready, working };
		workerState state;

		chrono::duration<double> timeSinceLastReply();
		void ping();

	private:
		chrono:time_t lastReply;
		string hostname;

};

