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

#include "serial.hpp"
#include "radio/message_types.h"

namespace po = boost::program_options;

using boost::shared_ptr;
using boost::asio::ip::tcp;
using std::cout;
using std::endl;

namespace settings {
    std::string input;
    std::string serial_device = "/dev/ttyUSB0";
    unsigned int speed = 115200;
    unsigned int tcp_port = 3009;
};

struct bfd_holder {
    bfd* abfd;

    bfd_holder(const std::string& file) {
        bfd_init();
        abfd = bfd_openr(file.c_str(), NULL);
        if(!bfd_check_format(abfd, bfd_object)) throw std::runtime_error("Bad input file");
    }

    ~bfd_holder() {
        bfd_close(abfd);
    }

    operator bfd*() {
        return abfd;
    }

    std::vector<unsigned char> get_section(const std::string& s) {
        asection* text = bfd_get_section_by_name(abfd, s.c_str());
        if(!text) throw std::runtime_error(std::string("Can'access section ") + s);
        std::vector<unsigned char> contents(text->size);
        bfd_get_section_contents(abfd, text, &contents[0], 0, text->size);
        return contents;
    }
};

void print_section(const std::vector<unsigned char>& c) {
    for(int i = 0; i < c.size(); ++i) {
        if(i % 4 == 0) cout << endl;
        cout << std::setw(2) << std::setfill('0') << std::hex << (unsigned int)c[i];
    }
    cout << std::dec << std::setw(1);
}

void output_section(const std::vector<unsigned char>& c) {
}

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
        ("port", po::value<int>(), "set TCP port")
        ("input", po::value<std::string>(), "set input file");

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

        if(vm.count("input"))
            settings::input = vm["input"].as<std::string>();
        else throw std::runtime_error("You should specify input file");

        if(vm.count("serial") && (vm.count("host") || vm.count("port"))) {
            throw std::runtime_error("You should specify either serial device or TCP host/port, not together");
        }

        if(!vm.count("serial") && !vm.count("host"))
            throw std::runtime_error("You should specify the means of output - either serial device or TCP host/port");

        boost::asio::io_service io;
        boost::shared_ptr<io> output(new serial_port(io, settings::speed, settings::serial_device));

        bfd_holder abfd(settings::input);
        std::vector<unsigned char> init = abfd.get_section(".init");
        std::vector<unsigned char> boot = abfd.get_section(".boot");
        std::vector<unsigned char> tramp = abfd.get_section(".tramp");
        std::vector<unsigned char> text = abfd.get_section(".text");
        std::vector<unsigned char> data = abfd.get_section(".data");
        /* Open serial port */

        
    } catch (std::exception& e) {
        /* Errors */
        std::cerr << e.what() << std::endl;
    }

    return 0;
}