/**	Copyright (C) 2010-2011 - Mauricio Bieze Stefani
 *	This file is part of the MBSBOT project.
 *
 *	MBSBOT is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	MBSBOT is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with MBSBOT.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <MBSUtil.h>

#include <iostream>
#include <fstream>

#include <boost/asio.hpp>
#include <boost/program_options.hpp>
#include <boost/thread.hpp>

using namespace std;
using namespace boost;

#ifdef _WIN32
    #define DEFAULT_SERIAL_DEVICE "COM1"
#else
	#define DEFAULT_SERIAL_DEVICE "/dev/ttyUSB0"
#endif
std::string serialPortDevice(DEFAULT_SERIAL_DEVICE);

#define DEFAULT_SERIAL_BAUD 115200
int baudRate = DEFAULT_SERIAL_BAUD;

#define DEFAULT_LISTEN_PORT 6661
int listenPort = DEFAULT_LISTEN_PORT;

#define SERIAL_BUFFER_SIZE 32
char serialBufferRx[SERIAL_BUFFER_SIZE];
char serialBufferTx[SERIAL_BUFFER_SIZE];

asio::io_service io;
asio::serial_port serial(io);
asio::ip::tcp::socket tcpsocket(io);

void handleRx(const system::error_code& error, size_t n)
{
    cout << std::string(serialBufferRx,n);
    if(tcpsocket.is_open())
        asio::write(tcpsocket, asio::buffer(serialBufferRx,n));
    serial.async_read_some(asio::buffer(serialBufferRx,SERIAL_BUFFER_SIZE), &handleRx);
}

void handleTx(const system::error_code& error, size_t n)
{
    cout << std::string(serialBufferTx,n);
    asio::write(serial, asio::buffer(serialBufferTx,n));
    tcpsocket.async_read_some(asio::buffer(serialBufferTx,SERIAL_BUFFER_SIZE), &handleTx);
}

int main(int argc, char* argv[])
{
    cout << "TCP/IP to Serial port proxy" << endl;
    cout << "(c) GPL 2011 - Mauricio Bieze Stefani" << endl << endl;

    try // parse configuration
    {
        namespace po = boost::program_options;

        po::options_description desc("Usage");

        desc.add_options()
            ("help,?", "This help message")
            ("device,d", po::value<string>(&serialPortDevice)->default_value(DEFAULT_SERIAL_DEVICE), "Serial port device")
            ("baud,b", po::value<int>(&baudRate)->default_value(DEFAULT_SERIAL_BAUD), "Baud rate")
            ("port,p", po::value<int>(&listenPort)->default_value(DEFAULT_LISTEN_PORT), "TCP port to listen")
            ;

        po::variables_map vm;

        // use config file if available
        ifstream cfg_file("proksi.cfg");
        if (cfg_file.good())
        {
            TRACE_INFO("Using config file proksi.cfg");
            po::store(po::parse_config_file(cfg_file, desc), vm);
            po::notify(vm);
        }

        po::positional_options_description p;

        // use first alone argument as "device"
        p.add("device", 1);

        // use second alone argument as "port"
        p.add("port", 1);

        // parse command line
        po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
        po::notify(vm);

        if (vm.count("help"))
        {
            cout << desc << endl;
            exit(0);
        }
    }
    catch(...)
    {
        TRACE_ERROR("Unknown exception");
        exit(-1);
    }

    try // open serial port
    {
        serial.open(serialPortDevice);
        serial.set_option(asio::serial_port_base::baud_rate(baudRate));
        TRACE_INFO("Opened %s @ %d bps", serialPortDevice.c_str(), baudRate);
    }
    catch(boost::system::system_error &e)
    {
        TRACE_ERROR("Error while trying to open %s: \"%s\"", serialPortDevice.c_str(), e.what());
        exit(-2);
    }

    try // listen and accept connections
    {
        asio::ip::tcp::acceptor acceptor(io, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), listenPort));

        TRACE_INFO("Listening port %d for conections...", listenPort);

        for (;;)
        {
            acceptor.accept(tcpsocket);

            serial.async_read_some(asio::buffer(serialBufferRx,SERIAL_BUFFER_SIZE), &handleRx);
            tcpsocket.async_read_some(asio::buffer(serialBufferTx,SERIAL_BUFFER_SIZE), &handleTx);
            io.run();

            tcpsocket.close();
        }
    }
    catch(std::exception &e)
    {
        TRACE_ERROR("shit => %s", e.what());
    }

    return 0;
}
