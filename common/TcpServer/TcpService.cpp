#pragma once

#include <iostream>
#include <string>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

using namespace std;
using namespace boost;

class TcpService {
	public:
        //The class's constructor accepts a shared pointer to an object representing a socket connected to a particular client as an argument
    // and caches this pointer. This socket will be used later to communicate with the client application.
        TcpService(std::shared_ptr<asio::ip::tcp::socket> socket) : socket(socket) {
        }

        std::shared_ptr<string> readOnce();
        void readAsyncContinuously(std::function<void(std::shared_ptr<string>)> callback);
        void write(std::shared_ptr<string> message);
        void writeAsync(std::shared_ptr<string> message);

        // Here we perform the cleanup.
        void stop() {
            delete this;
        }

protected:
    std::shared_ptr<asio::ip::tcp::socket> socket;

    system::error_code ec;
};

std::shared_ptr<string> TcpService::readOnce() {
    asio::streambuf* buf = new asio::streambuf;
    boost::system::error_code error;
    asio::read_until(*socket, *buf, '\n', error);
    if (ec.value() != 0) {
        this->ec = error;
        cout << "Read error: " << error.message();
        return make_shared<string>();
    }

    auto received = make_shared<string>("");
    std::istream is(buf);
    std::getline(is, *received);
    delete buf;
    return received;
}

void TcpService::readAsyncContinuously(std::function<void(std::shared_ptr<string>)> callback) {
    cout << "Listening" << endl;
    asio::streambuf* buf = new asio::streambuf();
    asio::async_read_until(*socket,
       *buf,
       '\n',
       [this, buf, callback](const boost::system::error_code& ec,
        std::size_t bytes_transferred) {
            //checks the error code
            auto response = make_shared<string>("");
            if (ec.value() != 0) {
                this->ec = ec;
                cout << "Async read error: " << ec.message();
            } else {
                std::istream is(buf);
                std::getline(is, *response);
            }
            delete buf;
            readAsyncContinuously(callback);
            callback(response);
        });
}

void TcpService::write(std::shared_ptr<string> message) {
    if (message->back() != '\n') {
        message->push_back('\n');
    }
    cout << "Sending: " << *message;
    asio::write(*socket, asio::buffer(*message));
}

void TcpService::writeAsync(std::shared_ptr<string> message) {
    if (message->back() != '\n') {
        message->push_back('\n');
    }
    cout << "Sending: " << *message;
    asio::async_write(*socket,
        asio::buffer(*message),
        [this, message](const boost::system::error_code& ec, // message must be in a capture list to keep it in scope until async_write is done
        std::size_t bytes_transferred) {
            // check the error code
            if (ec.value() != 0) {
                //session->m_ec = ec;
                cout << "Async write error: " << ec.message();
                return;
            }
        });
}