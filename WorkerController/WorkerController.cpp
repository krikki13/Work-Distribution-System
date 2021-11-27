// Worker.cpp : Defines the entry point for the application.
//

#include "WorkerController.h"

#define MASTER_HOSTNAME "127.0.0.1"
#define MASTER_PORT 13

enum State { initializing, ready, working };

class WorkerController {
public:
    WorkerController() : masterClient(MASTER_HOSTNAME, MASTER_PORT), currentState(initializing) {};

    string uid;
    TcpClient masterClient;

    void Start();

private:
    State currentState;
    void listenForCommands(std::shared_ptr<string> message);
    bool identifyWithMaster();
    string toString(State state);
};

void WorkerController::Start() {
	cout << "Worker initializing" << uid << endl;

    masterClient.start();
    if(!identifyWithMaster()) {
        throw "Failed to identify";
    }

    // How do you pass function by reference
    masterClient.readAsyncContinuously([this](std::shared_ptr<string> message) { listenForCommands(message); });

    currentState = ready;
    cout << "Ready to work :)" << endl;
}

bool WorkerController::identifyWithMaster() {
    for (int i = 0; i < 10; i++) {
        auto id = make_shared<string>("INIT WORKER");
        masterClient.write(id);
        std::shared_ptr<string> reply = masterClient.readOnce();
        cout << "Received " << *reply << endl;

        vector<string> msg;
        boost::split(msg, *reply, boost::is_any_of("\\s "));
        if (msg.size() == 3 && msg[0] == "INIT" && msg[1] == "OK") {
            uid = msg[2];
            cout << "Set UID to " << uid << endl;
            return true;
        } else if (!msg.empty()) {
            cout << "Failed to identify with Master: " << *reply << endl;
            return false;
        }
    }
    cout << "Failed to identify with Master after multiple attempts" << endl;
    return false;
}

void WorkerController::listenForCommands(std::shared_ptr<string> message) {
    vector<string> msg;
    boost::split(msg, *message, boost::is_any_of("\\s "));
    if(msg.size() == 0) {
        cout << "Received message with no content" << endl;
    }
	if(msg[0] == "PING") {
        auto reply = make_shared<string>("PONG " + toString(currentState));
        masterClient.writeAsync(reply);
	} else {
        cout << "Unknown command: " << *message << endl;
	}
}

string WorkerController::toString(State state) {
    if (state == initializing) {
        return "Initializing";
    } else if(state == ready) {
        return "Ready";
    } else if (state == working) {
        return "Working";
	}
    return "";
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
