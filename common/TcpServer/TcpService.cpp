#pragma once

#include <iostream>
#include <string>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

#include "ServerException.cpp"

using namespace std;
using namespace boost;

class TcpService : public boost::noncopyable {
	public:
        //The class's constructor accepts a shared pointer to an object representing a socket connected to a particular client as an argument
    // and caches this pointer. This socket will be used later to communicate with the client application.
        TcpService(std::shared_ptr<asio::ip::tcp::socket> socket) : socket(socket) {
            persistent = false;
            stopped = false;
        }

        std::shared_ptr<string> readOnce();
        void readAsyncContinuously(std::function<void(std::shared_ptr<string>)> callback);
        void write(std::shared_ptr<string> message);
        void writeAsync(std::shared_ptr<string> message);

        void setPersistent(bool value) {
            persistent = value;
        }

        void stop() {
			// What is the proper way to stop
            stopped = true;

            socket->close();
            if(!persistent) {
				delete this;
            }
        }

		bool isStopped() {
            return stopped;
        }

        system::error_code& getErrorCode() {
	        return ec;
        }

protected:
    std::shared_ptr<asio::ip::tcp::socket> socket;
    system::error_code ec;

    // if false the instance will destroy itself when fatal error is encountered
    // if true the instance will stop managing network request when fatal error is encountered, but it will not destroy itself 
    bool persistent;
    bool stopped;
};

std::shared_ptr<string> TcpService::readOnce() {
    if (stopped) throw ServerException("TCP Service has already stopped. Cannot execute readOnce()");

    asio::streambuf* buf = new asio::streambuf;
    boost::system::error_code error;
    asio::read_until(*socket, *buf, '\n', error);
    if (ec.value() != 0) {
        this->ec = error;
        cout << "Read error: " << error.message();
        if (ec.value() == 10054) {
            stop();
        }
        return make_shared<string>();
    }

    auto received = make_shared<string>("");
    std::istream is(buf);
    std::getline(is, *received);
    delete buf;
    return received;
}

void TcpService::readAsyncContinuously(std::function<void(std::shared_ptr<string>)> callback) {
    if (stopped) throw ServerException("TCP Service has already stopped. Cannot execute readAsyncContinuously()");

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
                if(ec.value() == 10054 ) {
	                stop();
                }
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
    if (stopped) throw ServerException("TCP Service has stopped. Cannot execute write()");

    if (message->back() != '\n') {
        message->push_back('\n');
    }
    cout << "Sending: " << *message;
    boost::system::error_code error;
    asio::write(*socket, asio::buffer(*message), ec);

    if (ec.value() != 0) {
        cout << "Write error: " << ec.message();
        if (ec.value() == 10054) {
            stop();
        }
    }
}

void TcpService::writeAsync(std::shared_ptr<string> message) {
    if (stopped)  throw ServerException("TCP Service has stopped. Cannot execute writeAsync()");

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
                if (ec.value() == 10054) {
                    stop();
                }
                return;
            }
        });
}