#pragma once

#include <iostream>
#include <string>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

using namespace boost;

class TcpService {
	public:
        //The class's constructor accepts a shared pointer to an object representing a socket connected to a particular client as an argument
    // and caches this pointer. This socket will be used later to communicate with the client application.
        TcpService(std::shared_ptr<asio::ip::tcp::socket> sock) : m_sock(sock) {
        }

        //This method starts handling the client by initiating the asynchronous reading operation
        //to read the request message from the client specifying the onRequestReceived() method as a callback.
        void StartHandling() {

            asio::async_read_until(*m_sock.get(),
                m_request,
                '\n',
                [this](
                const boost::system::error_code& ec,
                std::size_t bytes_transferred) {
                    //When the request reading completes, or an error occurs, the callback method onRequestReceived() is called.
                    onRequestReceived(ec,
                        bytes_transferred);
                });
        }

private:
    void onRequestReceived(const boost::system::error_code& ec,
        std::size_t bytes_transferred) {
        //This method first checks whether the reading succeeded by testing the ec argument that contains the operation completion status code.
        if (ec.value() != 0) {
            std::cout << "Error occured! Error code = "
                << ec.value()
                << ". Message: " << ec.message();
            //reading finished with an error, the corresponding message is output to the standard output stream
            //and then the onFinish() method is called.
            onFinish();
            return;
        }

        // Process the request.
        m_response = "Hello";

        // When the ProcessRequest() method completes and returns the string containing the response message,
        // the asynchronous writing operation is initiated to send this response message back to the client.
        asio::async_write(*m_sock.get(),
            asio::buffer(m_response),
            [this](
            const boost::system::error_code& ec,
            std::size_t bytes_transferred) {
                //The onResponseSent() method is specified as a callback.
                onResponseSent(ec, bytes_transferred);
            });
    }

    void onResponseSent(const boost::system::error_code& ec,
        std::size_t bytes_transferred) {
        // This method first checks whether the operation succeeded.
        if (ec.value() != 0) {
            // If the operation failed, the corresponding message is output to the standard output stream.
            std::cout << "Error occured! Error code = "
                << ec.value()
                << ". Message: " << ec.message();
        }

        //method is called to perform the cleanup.
        onFinish();
    }

    // Here we perform the cleanup.
    void onFinish() {
        delete this;
    }

	private:
	    std::shared_ptr<asio::ip::tcp::socket> m_sock;
	    std::string m_response;
	    asio::streambuf m_request;
};
