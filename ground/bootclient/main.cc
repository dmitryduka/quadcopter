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

#include "crc16.hpp"
#include "serial.hpp"
#include "radio/message_types.h"

namespace po = boost::program_options;

using boost::shared_ptr;
using boost::asio::ip::tcp;
using std::cout;
using std::endl;

namespace settings {
    std::string input;
    std::string serial_device = "/dev/ttyACM0";
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

struct bootloader_client {
    boost::shared_ptr<io> output;
    size_t max_packet_size;
    char * code_starts;

    bootloader_client(boost::shared_ptr<io> o) : output(o) {
        if(!ping()) throw std::runtime_error("Bootloader doesn't respond");
        cout << "Bootloader responded." << endl;
        max_packet_size = get_max_packet_size();
        get_code_starts();
    }

    bool ping() {
        output->write('?');
        unsigned char ch;
        if(output->read(ch))
            if(ch == '!') return true;
        return false;
    }

    size_t get_max_packet_size() { return 32; }

    void set_address(char * a) {
        output->write('%');
        unsigned char data[6];
        unsigned int x = reinterpret_cast<unsigned int>(a);
        data[0] = x & 0xFF;
        data[1] = (x >> 8) & 0xFF;
        data[2] = (x >> 16) & 0xFF;
        data[3] = (x >> 24) & 0xFF;
        unsigned short int crc = crc16(data, 4);
        data[4] = crc & 0xFF;
        data[5] = crc >> 8;
        for(int i = 0; i < 6; ++i)
            output->write(data[i]);
        unsigned char ch;
        if(!output->read(ch))
            throw std::runtime_error("Error reading response after setting the start address");
        if(ch != '%')
            throw std::runtime_error("Bootloader response is invalid (start address)");
        if(!output->read(ch))
            throw std::runtime_error("Error reading response after setting the start address");
        if(ch != '!')
            throw std::runtime_error("Bootloader responded the start address is invalid");
        cout << "Set address to " << "0x" << std::setw(8) << std::setfill('0') << std::hex << x << std::dec << endl;
    }

    void output_section(const std::vector<unsigned char>& c, char* address) {
        set_address(address);
        size_t chunks = c.size() / max_packet_size;
        size_t left = c.size() - chunks * max_packet_size;
        std::vector<unsigned char> data = {0, static_cast<unsigned char>(max_packet_size & 0xFF),
                                              static_cast<unsigned char>((max_packet_size >> 8) & 0xFF) };
        for(int i = 0; i < chunks; ++i) {
            data.resize(3);
            for(int k = 0; k < max_packet_size; ++k)
                data.push_back(c[max_packet_size * i + k]);
            unsigned short int crc = crc16(&data[0], data.size());
            data.push_back(crc & 0xFF);
            data.push_back(crc >> 8);
            for(int k = 0; k < data.size(); ++k)
                output->write(data[k]);
            /* Entire message is written, read response now */
            unsigned char ch;
            if(!output->read(ch))
                throw std::runtime_error("Error reading response after the write message");
            if(ch != 0 /*Write*/)
                throw std::runtime_error("Bootloader response after the write message is invalid");
            unsigned char d[2];
            for(int k = 0; k < 2; ++k)
                if(!output->read(d[k]))
                    throw std::runtime_error("Error reading response after the write message");
            size_t writtenBytes = d[0] | (d[1] << 8);
            crc = crc16(d, 2);
            for(int k = 0; k < 2; ++k)
                if(!output->read(d[k]))
                    throw std::runtime_error("Error reading response after the write message");
            unsigned short crc_b = d[0] | (d[1] << 8);
            if(crc != crc_b)
                throw std::runtime_error("Error reading response after the write message: CRC is wrong");
        }
    }

    char* get_code_starts() {
        output->write('^');
        unsigned char data[6];
        unsigned char ch;
        if(output->read(ch)) {
            if(ch == '^') {
                /* Read address and crc */
                for(int i = 0; i < 6; ++i)
                    if(!output->read(data[i]))
                        throw std::runtime_error("Error while trying to read CODE_STARTS symbol from the bootloader");
                /* Check crc */
                if(crc16(data, 4) != (data[4] | data[5] << 8))
                    throw std::runtime_error("Error while trying to read CODE_STARTS symbol from the bootloader");
                unsigned int cs = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
                code_starts = reinterpret_cast<char*>(cs);
                cout << "Code starts at " << "0x" << std::setw(8) << std::setfill('0') << std::hex << cs << std::dec << endl;
            }
        }
    }

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

        boost::asio::io_service ioserv;
        boost::shared_ptr<io> output(new serial_port(ioserv, settings::speed, settings::serial_device));
        bootloader_client bc(output);

        /*bfd_holder abfd(settings::input);
        std::vector<unsigned char> init = abfd.get_section(".init");
        std::vector<unsigned char> boot = abfd.get_section(".boot");
        std::vector<unsigned char> tramp = abfd.get_section(".tramp");
        std::vector<unsigned char> text = abfd.get_section(".text");
        std::vector<unsigned char> data = abfd.get_section(".data");*/
        /* Open serial port */
    } catch (std::exception& e) {
        /* Errors */
        std::cerr << e.what() << std::endl;
    }

    cout << "Finishing" << endl;
    return 0;
}