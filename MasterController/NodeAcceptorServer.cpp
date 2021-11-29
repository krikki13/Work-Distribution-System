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

//responsible for accepting or rejecting the connection requests arriving from clients and instantiating the objects of the Service class,
// which will provide the service to connected clients.
class NodeAcceptorServer : public AcceptorServer {
public:
	NodeAcceptorServer(asio::io_service& ios, unsigned short port_num, std::function<bool(WorkerNode*)> onWorkerNodeAdded) :
		AcceptorServer(ios, port_num), onWorkerNodeAdded(onWorkerNodeAdded) {};

	// Gets called when new connection is established. 
	void onAccept(std::shared_ptr<asio::ip::tcp::socket> socket) override {
		cout << "Waiting for client's identification details" << endl;
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

					identifyClient(socket, response);
				}

				delete m_response_buf;
			});
	}

private:
	void identifyClient(std::shared_ptr<asio::ip::tcp::socket> socket, const string& initialMessage);
	std::function<bool(WorkerNode*)> onWorkerNodeAdded;
};

void NodeAcceptorServer::identifyClient(std::shared_ptr<asio::ip::tcp::socket> socket, const string& initialMessage) {
	cout << "Received identification details: " << initialMessage << endl;

	vector<string> msg;
	boost::split(msg, initialMessage, boost::is_any_of("\\s "));
	if (msg.size() < 2) {
		return;
	}
	if (msg.size() == 2 && msg[0] == "INIT" && msg[1] == "WORKER") {
		auto* client = new WorkerNode(socket);
		if(onWorkerNodeAdded(client)) {
			client->setPersistent(true);
			client->writeAsync(make_shared<string>("INIT OK " + client->uid));
			client->start();
		}
	} else {
		cout << "else";
		return;
	}
	return;
}

