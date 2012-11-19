#include <iostream>
#include <vector>
#include <deque>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/program_options.hpp>
#include <boost/asio.hpp>

namespace po = boost::program_options;

using boost::asio::ip::tcp;
using std::cout;
using std::endl;

namespace settings {
    std::string serial_device = "/dev/ttyUSB0";
    unsigned int speed = 115200;
    unsigned int tcp_port = 3009;
};

namespace buffers {
    std::vector<std::vector<char>> in, out;
}

class tcp_connection : public boost::enable_shared_from_this<tcp_connection> {
public:
    typedef boost::shared_ptr<tcp_connection> pointer;
    static pointer create(boost::asio::io_service& io_service) {
	return pointer(new tcp_connection(io_service));
    }

    tcp::socket& socket() { return socket_; }
    void start() {
	/**/
    }
private:
    tcp_connection(boost::asio::io_service& io_service) : socket_(io_service) { }
    void handle_write(const boost::system::error_code& /*error*/, size_t /*bytes_transferred*/) { }

    tcp::socket socket_;
};

class tcp_server {
public:
    tcp_server(boost::asio::io_service& io_service) : acceptor_(io_service, tcp::endpoint(tcp::v4(), settings::tcp_port)) {
	start_accept();
    }
private:
    void start_accept() {
	tcp_connection::pointer new_connection = tcp_connection::create(acceptor_.io_service());
	acceptor_.async_accept(new_connection->socket(), boost::bind(&tcp_server::handle_accept, this));
    }
    void handle_accept() {
    }

    tcp::acceptor acceptor_;
};

class serial_port {
public:
        serial_port(boost::asio::io_service& io_service, unsigned int baud, const std::string& device) : 
    		    active_(true), io_service_(io_service), serialPort(io_service, device) {
	    if (not serialPort.is_open()) {
        	cout << "Failed to open serial port" << endl;
		return;
	    }
	    boost::asio::serial_port_base::baud_rate baud_option(baud);
	    serialPort.set_option(baud_option); // set the baud rate after the port has been opened 
            read_start();
        }
	// pass the write data to the do_write function via the io service in the other thread
	void write(const char msg) { io_service_.post(boost::bind(&serial_port::do_write, this, msg)); }
	// call the do_close function via the io service in the other thread 	
        void close() { io_service_.post(boost::bind(&serial_port::do_close, this, boost::system::error_code())); }
	// return true if the socket is still active
	bool active() { return active_; }

private:
    static const int max_read_length = 512; // maximum amount of data to read in one operation

 // Start an asynchronous read and call read_complete when it completes or fails     
    void read_start(void) {
	serialPort.async_read_some(boost::asio::buffer(read_msg_, max_read_length), 
				    boost::bind(&serial_port::read_complete, 
						this,
						boost::asio::placeholders::error, 
						boost::asio::placeholders::bytes_transferred));
    }
    // the asynchronous read operation has now completed or failed and returned an error 
    void read_complete(const boost::system::error_code& error, size_t bytes_transferred) {
	// read completed, so process the data 
	if (!error) {
	    /* TODO: send data to all TCP threads */
	    cout.write(read_msg_, bytes_transferred); // echo to standard output
            read_start(); // start waiting for another asynchronous read again 
	} else do_close(error);
    }
    // callback to handle write call from outside this class 
    void do_write(const char msg) {
	bool write_in_progress = !write_msgs_.empty(); // is there anything currently being written? 
        write_msgs_.push_back(msg); // store in write buffer 
        if (!write_in_progress) // if nothing is currently being written, then start 
        write_start(); 
    }
    // Start an asynchronous write and call write_complete when it completes or fails 
    void write_start(void) {
	boost::asio::async_write(serialPort, 
				boost::asio::buffer(&write_msgs_.front(), 1), 
				boost::bind(&serial_port::write_complete, 
					    this, boost::asio::placeholders::error));
    }
    // the asynchronous read operation has now completed or failed and returned an error 
    void write_complete(const boost::system::error_code& error) { 
	// write completed, so send next write data 
	if (!error) { 
	    write_msgs_.pop_front(); // remove the completed data 
            if (!write_msgs_.empty()) // if there is anything left to be written 
                write_start(); // then start sending the next item in the buffer 
        } else do_close(error);
    }

    // something has gone wrong, so close the socket & make this object inactive 
    void do_close(const boost::system::error_code& error) { 
	// if this call is the result of a timer cancel()
        if (error == boost::asio::error::operation_aborted) 
    	    return; // ignore it because the connection cancelled the timer 
	if (error) cout << "Error: " << error.message() << endl; 
	else cout << "Error: Connection did not succeed.\n"; 
	serialPort.close(); 
	active_ = false; 
    }

    bool active_; // remains true while this object is still operating 
    boost::asio::io_service& io_service_; // the main IO service that runs this connection
    boost::asio::serial_port serialPort; // the serial port this instance is connected to
    char read_msg_[max_read_length]; // data read from the socket
    std::deque<char> write_msgs_; // buffered write data
};

int main(int argc, char** argv) {
    // Declare the supported options.
    try {
	/* Read settings */
	po::options_description desc("Allowed options");
	desc.add_options()("help", "produce help message")
			("serial", po::value<std::string>(), "set serial device (\"COM1,/dev/ttyS0,etc\")")
			("speed", po::value<int>(), "set serial speed (9600/115200)")
			("stopbits", po::value<int>(), "set stop bits")
			("parity", po::value<int>(), "set parity")
			("flowcontrol", po::value<int>(), "set parity")
			("host", po::value<int>(), "set TCP IP address")
			("port", po::value<int>(), "set TCP port");

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help")) {
	    cout << desc << endl;
    	    return 1;
	}
	if(vm.count("serial"))
	    settings::serial_device = vm["serial"].as<std::string>();

	if(vm.count("speed"))
	    settings::speed = vm["speed"].as<int>();

	boost::asio::io_service io;
	/* Open serial port */
	serial_port serial(io, settings::speed, settings::serial_device);
	/* Start TCP server */
	tcp_server server(io);
	/* Start everything */
	io.run();
    } catch (std::exception& e) {
    /* Errors */
	std::cerr << e.what() << std::endl;
    }

    return 0;
}