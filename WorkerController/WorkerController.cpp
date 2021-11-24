// Worker.cpp : Defines the entry point for the application.
//

#include "WorkerController.h"

using namespace std;
using namespace boost;

string uid = boost::lexical_cast<std::string>(boost::uuids::random_generator()());

struct Session {
    Session(asio::io_service& ios,
        const std::string& raw_ip_address,
        unsigned short port_num) :
			m_sock(ios),
	        m_ep(asio::ip::address::from_string(raw_ip_address), port_num),
	        m_was_cancelled(false) {
    }

    asio::ip::tcp::socket m_sock; // Socket used for communication
    asio::ip::tcp::endpoint m_ep; // Remote endpoint.

    // Contains the description of an error if one occurs during
    // the request lifecycle.
    system::error_code m_ec;

    bool m_was_cancelled;
    std::mutex m_cancel_guard;
};

class TcpClient : public boost::noncopyable {
public:
    void initializeConnection(const std::string& raw_ip_address, unsigned short port_num);
    //void read();
    //void write(string message);
	
private:
    asio::io_service m_ios;
    std::map<int, std::shared_ptr<Session>> m_active_sessions;
    std::mutex m_active_sessions_guard;
    std::shared_ptr<Session> session_ptr;
};

void TcpClient::initializeConnection(const std::string& raw_ip_address, unsigned short port_num) {
    std::shared_ptr<Session> session = std::shared_ptr<Session>(
        new Session(m_ios,
	        raw_ip_address,
	        port_num));

    session->m_sock.open(session->m_ep.protocol());
    session_ptr = session;

    //std::unique_lock<std::mutex> lock(m_active_sessions_guard);
    //m_active_sessions[request_id] = session;
    //lock.unlock();

    session->m_sock.async_connect(session->m_ep,
        [this, session](const system::error_code& ec) {
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
        });
}

/*void TcpClient::read() {
    asio::async_read_until(session_ptr->m_sock,
       session_ptr->m_response_buf,
       '\n',
       [this, session_ptr](const boost::system::error_code& ec,
        std::size_t bytes_transferred) {
            //checks the error code
            string response;
            if (ec.value() != 0) {
                session_ptr->m_ec = ec;
            } else {
                std::istream strm(&session_ptr->m_response_buf);
                std::getline(strm, response);
            }

            cout << response << endl;
		});
}

void TcpClient::write() {
	
}*/

int main() {
	cout << "Worker initializing" << endl;
	cout << uid << endl;

    TcpClient client;
    client.initializeConnection("localhost", 13);


	return 0;
}
