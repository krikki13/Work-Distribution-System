#pragma once

#include <iostream>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include "TcpService.cpp"

using namespace std;
using namespace boost::asio::ip;

//responsible for accepting the connection requests arriving from clients and instantiating the objects of the Service class,
// which will provide the service to connected clients.
class AcceptorServer {
public:
    AcceptorServer(asio::io_service& ios, unsigned short port_num) : m_ios(ios),
        //The object of this class contains an instance of the asio::ip::tcp::acceptor class as its member named m_acceptor,
        //which is constructed in the AcceptorServer class's constructor.
        m_acceptor(m_ios,
        asio::ip::tcp::endpoint(
        asio::ip::address_v4::any(),
        port_num)),
        m_isStopped(false) {
    }

    //The Start() method is intended to instruct an object of the AcceptorServer class to start listening and accepting incoming connection requests.
    void Start() {
        //It puts the m_acceptor acceptor socket into listening mode
        m_acceptor.listen();
        cout << "Listening for connections" << endl;
        InitAccept();
    }

    // Stop accepting incoming connection requests.
    void Stop() {
        m_isStopped.store(true);
    }

    virtual void onAccept(std::shared_ptr<asio::ip::tcp::socket> socket);

private:
    void InitAccept() {
        //constructs an active socket object and initiates the asynchronous accept operation
        std::shared_ptr<asio::ip::tcp::socket> sock(new asio::ip::tcp::socket(m_ios));

        cout << "Waiting for a new client " << endl;
        //calling the async_accept() method on the acceptor socket object
        // and passing the object representing an active socket to it as an argument.
        m_acceptor.async_accept(*sock.get(),
            [this, sock](
            const boost::system::error_code& error) {
                try {
                    // When the connection request is accepted, the callback method onAccept() is called.
                    if (error.value() == 0) {
                        cout << "We have got a new client" << endl;
                        onAccept(sock);
                    } else {
                        //the corresponding message is output to the standard output stream.
                        std::cout << "AcceptorServer#onAccept: Error occured! Error code = "
                            << error.value() << ". Message: " << error.message();
                    }

                    // Init next async accept operation if
                    // acceptor has not been stopped yet.
                    if (!m_isStopped.load()) {
                        InitAccept();
                    } else {
                        // Stop accepting incoming connections
                        // and free allocated resources.
                        m_acceptor.close();
                    }
                }catch(std::exception& e) {
                    cout << "Exception in async_accept: " << e.what() << endl;
                }catch(...){}
            });
    }

    asio::io_service& m_ios;
    //used to asynchronously accept the incoming connection requests.
    asio::ip::tcp::acceptor m_acceptor;
    std::atomic<bool> m_isStopped;
};

void AcceptorServer::onAccept(std::shared_ptr<asio::ip::tcp::socket> socket) {
    TcpService* service = new TcpService(socket);
    service->readAsyncContinuously(
        [this](std::shared_ptr<string> message) {
            cout << "Received: " << *message << endl;
        });
}