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
    TcpClient(const string& hostName, const unsigned short port) : TcpClient(hostName, port, new asio::io_service) {}

    ~TcpClient() {
        stop();
        delete io_service;
    }

    void start();
    void stop();

private:
    TcpClient(const string& hostName, const unsigned short port, asio::io_service* io_service1) : io_service(io_service1),
        TcpService(make_shared<asio::ip::tcp::socket>(asio::ip::tcp::socket(*io_service1))),
        ep(asio::ip::address::from_string(hostName), port), isInitialized(false) {

        m_work.reset(new boost::asio::io_service::work(*io_service));

        io_service_thread = make_unique<std::thread>([this]() { io_service->run(); });
    }

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

    /*socket.async_connect(ep,
        [this](const system::error_code& ec) {
            //checking the error code passed to it as the ec argument
            if (ec.value() != 0) {
                //we store the ec value in the corresponding Session object,
                session->m_ec = ec;
                //call the class's onRequestComplete() private method passing the Session object to it as an argument

                cout << "Async connection error" << endl;
                //then return.
                return;
            }
            cout << "Async connected" << endl;

            auto s = make_shared<string>("Hello there\n");
            write(s);
        });*/
}


void TcpClient::stop() {
    m_work.reset(NULL);
    isInitialized = false;

    io_service_thread->join();
}