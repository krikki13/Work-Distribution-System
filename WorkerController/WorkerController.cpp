// Worker.cpp : Defines the entry point for the application.
//

#include "WorkerController.h"

#define MASTER_HOSTNAME "127.0.0.1"
#define MASTER_PORT 13


class WorkerController {
public:
    WorkerController() : masterClient(MASTER_HOSTNAME, MASTER_PORT) {};

    string uid;
    TcpClient masterClient;

    void Start();

private:
    bool identifyWithMaster();
};

void WorkerController::Start() {
	cout << "Worker initializing" << uid << endl;

    masterClient.initializeConnection();
    if(!identifyWithMaster()) {
        throw "Failed to identify";
    }
    cout << "SUCCESS :)" << endl;
}

bool WorkerController::identifyWithMaster() {
    for (int i = 0; i < 10; i++) {
        auto id = make_shared<string>("INIT WORKER");
        masterClient.write(id);
        std::shared_ptr<string> reply = masterClient.readOnce();

        vector<string> msg;
        boost::split(msg, reply, boost::is_any_of("\s"));
        if (msg.size() == 3 && msg[0] == "INIT" && msg[1] == "OK") {
            uid = msg[2];
            cout << "Set UID to " << uid;
            return true;
        } else if (msg.size() != 0) {
            cout << "Failed to identify with Master: " << *reply << endl;
            return false;
        }
    }
    cout << "Failed to identify with Master after multiple attempts" << endl;
    return false;
}


int main() {

    WorkerController controller;
    controller.Start();
    
    /*std::this_thread::sleep_for(std::chrono::seconds(3));
    auto s = make_shared<string>("How are ya\n");
    client.write(s);*/

    for (;;)
        std::this_thread::sleep_for(std::chrono::seconds(60));
    //client.stop();

	return 0;
}
