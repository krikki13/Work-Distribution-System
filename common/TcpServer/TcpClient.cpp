#include <atomic>
#include <iostream>
#include <thread>
#include <string>

using namespace std;
using namespace boost;


class TcpClient : public boost::noncopyable {
public:
    TcpClient(const string& hostName, const unsigned short port) : socket(m_ios),
        ep(asio::ip::address::from_string(hostName), port), isInitialized(false) {
        //instantiates an object of the asio::io_service::work class
        // passing an instance of the asio::io_service class named m_ios to its constructor
        m_work.reset(new boost::asio::io_service::work(m_ios));

        for (unsigned char i = 1; i <= 1; i++) {
            //spawns a thread that calls the run() method of the m_ios object.
            std::unique_ptr<std::thread> th(
                new std::thread([this]() { m_ios.run(); }));

            m_threads.push_back(std::move(th));
        }
        cout << "Constructor done" << endl;
    }

    void initializeConnection();
    std::shared_ptr<string> readOnce();
    void readAsyncContinuously();
    void write(std::shared_ptr<string> message);
    void writeAsync(std::shared_ptr<string> message);
    void stop();

private:
    std::atomic<bool> isInitialized;

    asio::io_service m_ios;
    std::unique_ptr<boost::asio::io_service::work> m_work;
    std::list<std::unique_ptr<std::thread>> m_threads;

    asio::ip::tcp::socket socket;
    asio::ip::tcp::endpoint ep; // Remote endpoint.

    // Contains the description of an error if one occurs during
    // the request lifecycle.
    system::error_code ec;
};

void TcpClient::initializeConnection() {
    if(isInitialized) {
        cout << "Connection is already initialized" << endl;
        return;
    }
    isInitialized = true;
    cout << "InitializeConnection" << endl;

    socket.open(ep.protocol());
    socket.connect(ep);

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

std::shared_ptr<string> TcpClient::readOnce() {
    asio::streambuf buf;
    boost::system::error_code error;
    asio::read_until(socket,buf, '\n', error);

    std::shared_ptr<string> received;
    std::istream is(&buf);
    std::getline(is, *received);
    return received;
}

void TcpClient::readAsyncContinuously() {
    cout << "Listening" << endl;
    asio::streambuf* m_response_buf = new asio::streambuf();
    asio::async_read_until(socket,
       *m_response_buf,
       '\n',
       [this, m_response_buf](const boost::system::error_code& ec,
        std::size_t bytes_transferred) {
            //checks the error code
            string response;
            if (ec.value() != 0) {
                this->ec = ec;
            } else {
                std::istream strm(m_response_buf);
                std::getline(strm, response);
            }

            cout << "Response: " << response << endl;
            delete m_response_buf;
            readAsyncContinuously();
        });
}

void TcpClient::write(std::shared_ptr<string> message) {
    asio::write(socket, asio::buffer(*message));
}

void TcpClient::writeAsync(std::shared_ptr<string> message) {
    asio::async_write(socket,
        asio::buffer(*message),
        [this, message](const boost::system::error_code& ec, // message must be in a capture list to keep it in scope until async_write is done
        std::size_t bytes_transferred) {
            // check the error code
            if (ec.value() != 0) {
                //session->m_ec = ec;
                cout << "Async write error" << endl;
                return;
            }
            cout << "Async write" << endl;
        });
}

void TcpClient::stop() {
    // Destroy work object. This allows the I/O threads to
    // exit the event loop when there are no more pending
    // asynchronous operations.
    m_work.reset(NULL);

    // Waiting for the I/O threads to exit.
    for (auto& thread : m_threads) {
        thread->join();
    }
}