#pragma once

#include <iostream>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include "WorkerNode.cpp"
#include "../common/TcpServer/AcceptorServer.cpp"

using namespace std;
using namespace boost::asio::ip;

//responsible for accepting the connection requests arriving from clients and instantiating the objects of the Service class,
// which will provide the service to connected clients.
class NodeAcceptorServer : public AcceptorServer {
public:
    NodeAcceptorServer(asio::io_service& ios, unsigned short port_num) : AcceptorServer(ios, port_num) {};

    TcpService* onConnectionRequest(std::shared_ptr<asio::ip::tcp::socket> socket, const string& initialMessage) {
        cout << "Got " << initialMessage << "-" << endl;
        if (initialMessage == "Hello there") {
            cout << "if";
            WorkerNode* client = new WorkerNode(socket);
            return client;
        } else {
            cout << "else";
            return NULL;
        }
    }
};