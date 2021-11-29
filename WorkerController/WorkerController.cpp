// Worker.cpp : Defines the entry point for the application.
//

#include "WorkerController.h"

#define MASTER_HOSTNAME "127.0.0.1"
#define MASTER_PORT 13

struct Task {
    string taskId;
	std::thread* workerThread;
    std::chrono::steady_clock::time_point startedAt;

    Task() {}
    Task(string taskId, std::chrono::steady_clock::time_point startedAt) : taskId(taskId), startedAt(startedAt) {}
    ~Task() {
	    if(workerThread != nullptr) {
            workerThread->join();
            delete workerThread;
	    }
    }
};

class WorkerController {
public:
    WorkerController() : masterClient(MASTER_HOSTNAME, MASTER_PORT), currentState(initializing) {};

    string uid;
    TcpClient masterClient;

    void Start();

private:
    WorkerState currentState;
    unique_ptr<Task> latestTask;

    void listenForCommands(std::shared_ptr<string> message);
    bool identifyWithMaster();

    void startWorking(string& taskId);
    void taskFinished();
};

void WorkerController::Start() {
    cout << "Worker initializing" << uid << endl;
    try {

        if (!identifyWithMaster()) {
            throw "Failed to identify";
        }

        masterClient.readAsyncContinuously([this](std::shared_ptr<string> message) { listenForCommands(message); });
        currentState = ready;
        cout << "Ready to work :)" << endl;

        while (true) {
            if (masterClient.isStopped()) {
                cout << "TcpClient stopped. Exiting" << endl;
                return;
            }
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
    } catch (ServerException e) {
        cout << "Exception: " << e.what() << endl;
        cout << "TcpClient stopped. Exiting" << endl;
    }
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
        auto reply = make_shared<string>("PONG " + workerStateToString(currentState));
        masterClient.writeAsync(reply);
    } else if (msg.size() == 2 && msg[0] == "TASK") { // TASK <task_id>
        if (currentState != ready) {
            masterClient.writeAsync(make_shared<string>(*message + " REJECTED NOT_READY"));
            return;
	    }
        startWorking(msg[1]);
    } else {
        cout << "Unknown command: " << *message << endl;
	}
}

void WorkerController::startWorking(string& taskId) {
    masterClient.writeAsync(make_shared<string>("TASK " + taskId + " ACCEPTED"));
    currentState = working;

    latestTask.reset(new Task(taskId, std::chrono::high_resolution_clock::now()));
    latestTask->workerThread = new std::thread([this]() {
        cout << "Working hard!" << endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
        cout << "Done" << endl;
        this->taskFinished();
    });
}

void WorkerController::taskFinished() {
    double elapsedTime = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - latestTask->startedAt).count();
    int seconds = (int)elapsedTime / 1000;
    masterClient.write(make_shared<string>("TASK " + latestTask->taskId + " FINISHED TIME " + std::to_string(seconds)));
    currentState = ready;
}

int main() {
    WorkerController controller;
    controller.Start();

	return 0;
}
