#pragma once

#include <iostream>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include "WorkerNode.cpp"
#include "../common/TcpServer/AcceptorServer.cpp"

using namespace std;
using namespace boost::asio::ip;

//responsible for accepting the connection requests arriving from clients and instantiating the objects of the Service class,
// which will provide the service to connected clients.
class NodeAcceptorServer : public AcceptorServer {
public:
    NodeAcceptorServer(asio::io_service& ios, unsigned short port_num, std::function<void(WorkerNode*)> onWorkerNodeAdded) :
		AcceptorServer(ios, port_num), onWorkerNodeAdded(onWorkerNodeAdded) {};

    void onAccept(std::shared_ptr<asio::ip::tcp::socket> socket) override {
	    auto m_response_buf = new asio::streambuf();
        asio::async_read_until(*socket,
           *m_response_buf,
           '\n',
           [this, m_response_buf, socket](const boost::system::error_code& ec,
            std::size_t bytes_transferred) {
	           if (ec.value() != 0) {
                    cout << "Error";
                } else {
		           string response;
		           std::istream is(m_response_buf);
                    std::getline(is, response);

                    TcpService* newClient = identifyClient(socket, response);
                    if (newClient != NULL) {
                        newClient->StartHandling();
                    } // else socket will be forgotten
                }

                delete m_response_buf;
            });
    }

private:
    TcpService* identifyClient(std::shared_ptr<asio::ip::tcp::socket> socket, const string& initialMessage);
    std::function<void(WorkerNode*)> onWorkerNodeAdded;
};

TcpService* NodeAcceptorServer::identifyClient(std::shared_ptr<asio::ip::tcp::socket> socket, const string& initialMessage) {
    cout << "Got " << initialMessage << "-" << endl;

    vector<string> msg;
    boost::split(msg, initialMessage, boost::is_any_of("\s"));
    if(msg.size() < 2) {
        return nullptr;
    }
    if (msg.size() == 3 && msg[0] == "INIT" && msg[1] == "WORKER") {
        auto* client = new WorkerNode(socket);
        onWorkerNodeAdded(client);
        return client;
    } else {
        cout << "else";
        return NULL;
    }
}

