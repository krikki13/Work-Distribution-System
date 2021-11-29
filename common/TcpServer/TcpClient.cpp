#include <atomic>
#include <iostream>
#include <thread>
#include <string>
#include "TcpService.cpp"

using namespace std;
using namespace boost;


class TcpClient : public TcpService {
public:
    // Public constructor calling private one is a workaround, because I could not find a way to initialize io_service
	// before initializing socket in TcpService constructor
    TcpClient(const string& hostName, const unsigned short port) : TcpClient(hostName, port, new asio::io_service) {
        persistent = true;

        start();
    }

    ~TcpClient() {
        stop();
    }

    void stop();

private:
    TcpClient(const string& hostName, const unsigned short port, asio::io_service* io_service1) : io_service(io_service1),
        TcpService(make_shared<asio::ip::tcp::socket>(asio::ip::tcp::socket(*io_service1))),
        ep(asio::ip::address::from_string(hostName), port), isInitialized(false) {

        m_work.reset(new boost::asio::io_service::work(*io_service));

        io_service_thread = make_unique<std::thread>([this]() { io_service->run(); });
    }
    void start();

    std::atomic<bool> isInitialized;
    std::unique_ptr<std::thread> io_service_thread;

    asio::io_service* io_service;
    std::unique_ptr<boost::asio::io_service::work> m_work;

    asio::ip::tcp::endpoint ep; // Remote endpoint.

};

void TcpClient::start() {
    if(isInitialized) {
        cout << "Connection is already initialized" << endl;
        return;
    }
    isInitialized = true;
    cout << "Initializing connection" << endl;

    socket->open(ep.protocol());
    socket->connect(ep);
}


void TcpClient::stop() {
    if (stopped) return;

    stopped = true;
    m_work.reset(NULL);

    io_service_thread->join();
    delete io_service;
}