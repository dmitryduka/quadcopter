#include <bfd.h>
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <vector>
#include <deque>
#include <list>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/program_options.hpp>
#include <boost/asio.hpp>

#include "radio/message_types.h"

namespace po = boost::program_options;

using boost::shared_ptr;
using boost::asio::ip::tcp;
using std::cout;
using std::endl;

class serial_port;

namespace settings {
    std::string serial_device = "/dev/ttyUSB0";
    unsigned int speed = 115200;
    unsigned int tcp_port = 3009;
};

/* Interfaces */
class listener_interface {
public:
    virtual bool listen(const char*, size_t) = 0;
};

/* Implementations */

class serial_port {
public:
    serial_port(boost::asio::io_service& io_service, unsigned int baud, const std::string& device) : 
		    io_service_(io_service), port(io_service, device), current_offset(0) {
   		if (!port.is_open()) {
    		cout << "Failed to open serial port" << endl;
			return;
    	}
    	boost::asio::serial_port_base::baud_rate baud_option(baud);
    	port.set_option(baud_option); // set the baud rate after the port has been opened 
        read_start();
    }
	// pass the write data to the do_write function via the io service in the other thread
	void write(const std::vector<char>& msg) { do_write(msg); }
	// call the do_close function via the io service in the other thread 	
	void close() { io_service_.post(boost::bind(&serial_port::do_close, this, boost::system::error_code())); }
	// add listener to analyze buffer content
	void add_listener(const shared_ptr<listener_interface> l) { listeners_.push_back(l); }
	// add listener to analyze buffer content
	void remove_listener(const shared_ptr<listener_interface> l) { listeners_.remove(l); }
private:
    static const int max_read_length = 256; // maximum amount of data to read in one operation

	// Start an asynchronous read and call read_complete when it completes or fails     
    void read_start(void) {
		port.async_read_some(boost::asio::buffer(read_msg_ + current_offset, 1), 
				        boost::bind(&serial_port::read_complete, 
						this,
						boost::asio::placeholders::error, 
						boost::asio::placeholders::bytes_transferred));
    }
    // the asynchronous read operation has now completed or failed and returned an error 
    void read_complete(const boost::system::error_code& error, size_t bytes_transferred) {
        cout << read_msg_[current_offset];
        cout.flush();
		// read completed, so process the data
		if (!error) {
			bool message_accepted = false;
			if(bytes_transferred > 0) {
			    current_offset += bytes_transferred;
		        /* notify listeners */
                for(std::list<shared_ptr<listener_interface>>::iterator it = listeners_.begin(); it != listeners_.end(); ++it)
				if ((*it)->listen(read_msg_, current_offset))
					message_accepted = true;
			    /*  start receiving data from the start of the buffer in case 
		    	any listener accepts the data or we've exceeded the buffer size */		
		        if(message_accepted || (current_offset >= max_read_length)) current_offset = 0;
		    }

		    read_start(); // start waiting for another asynchronous read again 
		} else do_close(error);
    }
    // callback to handle write call from outside this class 
    void do_write(const std::vector<char>& msg) {
		//bool write_in_progress = !write_msgs_.empty(); // is there anything currently being written? 
        write_msgs_.push_back(msg); // store in write buffer 
        //if (!write_in_progress) // if nothing is currently being written, then start 
        write_start();
    }
    // Start an asynchronous write and call write_complete when it completes or fails 
    void write_start(void) {
        cout << "UART:" << std::string(write_msgs_.front().begin(), write_msgs_.front().end());
		port.async_write_some(
				boost::asio::buffer(write_msgs_.front(), write_msgs_.front().size()), 
				boost::bind(&serial_port::write_complete, 
					    this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }
    // the asynchronous read operation has now completed or failed and returned an error 
    void write_complete(const boost::system::error_code& error, size_t bytes_transferred) { 
	// write completed, so send next write data 
		if (!error) { 
		    cout << bytes_transferred << " sent" << endl;
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
		port.close();
    }

    boost::asio::io_service& io_service_; // the main IO service that runs this connection
    boost::asio::serial_port port; // the serial port this instance is connected to
    char read_msg_[max_read_length]; // data read from the socket
    std::list<std::vector<char>> write_msgs_; // buffered write data
    std::list<shared_ptr<listener_interface>> listeners_;
    size_t current_offset;
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
				("flowcontrol", po::value<int>(), "set flow control")
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

		if(vm.count("port"))
		    settings::tcp_port = vm["port"].as<int>();

		bfd_init();
		bfd* abfd = bfd_openr("./main", NULL);
		cout << "Opened main : " ;
		if(bfd_check_format(abfd, bfd_object)) cout << "ok" << endl;
		else {
			cout << "fail" << endl;
			return 1;
		}
		if(abfd) {
			{
				asection* text = bfd_get_section_by_name(abfd, ".init");
				if(!text) {
					cout << "Can't access section .text" << endl;
					return 0;
				}
				std::vector<unsigned char> contents(text->size);
				bfd_get_section_contents(abfd, text, &contents[0], 0, text->size);
				for(int i = 0; i < contents.size(); ++i) {
					if(i % 4 == 0) cout << endl;
					cout << std::setw(2) << std::setfill('0') << std::hex << (unsigned int)contents[i];
				}
			}
			{
				asection* text = bfd_get_section_by_name(abfd, ".boot");
				if(!text) {
					cout << "Can't access section .text" << endl;
					return 0;
				}
				std::vector<unsigned char> contents(text->size);
				bfd_get_section_contents(abfd, text, &contents[0], 0, text->size);
				for(int i = 0; i < contents.size(); ++i) {
					if(i % 4 == 0) cout << endl;
					cout << std::setw(2) << std::setfill('0') << std::hex << (unsigned int)contents[i];
				}
			}
			{
				asection* text = bfd_get_section_by_name(abfd, ".tramp");
				if(!text) {
					cout << "Can't access section .text" << endl;
					return 0;
				}
				std::vector<unsigned char> contents(text->size);
				bfd_get_section_contents(abfd, text, &contents[0], 0, text->size);
				for(int i = 0; i < contents.size(); ++i) {
					if(i % 4 == 0) cout << endl;
					cout << std::setw(2) << std::setfill('0') << std::hex << (unsigned int)contents[i];
				}
			}
			{
				asection* text = bfd_get_section_by_name(abfd, ".text");
				if(!text) {
					cout << "Can't access section .text" << endl;
					return 0;
				}
				std::vector<unsigned char> contents(text->size);
				bfd_get_section_contents(abfd, text, &contents[0], 0, text->size);
				for(int i = 0; i < contents.size(); ++i) {
					if(i % 4 == 0) cout << endl;
					cout << std::setw(2) << std::setfill('0') << std::hex << (unsigned int)contents[i];
				}
			}
		} else {                                     
			cout << "Can't open file" << endl;
		}
		//boost::asio::io_service io;
		/* Open serial port */
		//serial_port serial(io, settings::speed, settings::serial_device);
    } catch (std::exception& e) {
	    /* Errors */
		std::cerr << e.what() << std::endl;
    }

    return 0;
}