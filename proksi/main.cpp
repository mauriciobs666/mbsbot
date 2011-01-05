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
namespace po = boost::program_options;

#ifdef _WIN32
    #define DEFAULT_SERIAL_DEVICE "COM1"
#else
	#define DEFAULT_SERIAL_DEVICE "/dev/ttyUSB0"
#endif

#define DEFAULT_SERIAL_BAUD 115200
#define DEFAULT_LISTEN_PORT 6661

std::string serialPortDevice(DEFAULT_SERIAL_DEVICE);
int baudRate = DEFAULT_SERIAL_BAUD;
int listenPort = DEFAULT_LISTEN_PORT;

using namespace std;
using namespace boost;

int main(int argc, char* argv[])
{
    cout << "TCP/IP to Serial port proxy" << endl;
    cout << "(c) GPL 2011 - Mauricio Bieze Stefani" << endl;

    // parse command line options

    try
    {
        po::options_description desc("Usage");

        desc.add_options()
            ("help,?", "This help message")
            ("device,d", po::value<string>(&serialPortDevice)->default_value(DEFAULT_SERIAL_DEVICE), "Serial port device")
            ("baud,b", po::value<int>(&baudRate)->default_value(DEFAULT_SERIAL_BAUD), "Baud rate")
            ("port,p", po::value<int>(&listenPort)->default_value(DEFAULT_LISTEN_PORT), "TCP port to listen")
            ;

        po::positional_options_description p;
        p.add("device", -1);

        po::variables_map vm;

        ifstream ifs("proksi.cfg");
        if (!ifs)
        {
            // error ?
        }
        else
        {
            store(parse_config_file(ifs, desc), vm);
            notify(vm);
        }

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
        exit(-1);
    }

    try
    {
        asio::io_service io;
        asio::serial_port serial(io, serialPortDevice);
        serial.set_option(asio::serial_port_base::baud_rate(baudRate));
        TRACE_INFO("Opened %s @ %d bps", serialPortDevice.c_str(), baudRate);

        char c;
        for(;;)
        {
            asio::read(serial, asio::buffer(&c,1));
            cout << c;
        }
    }
    catch(boost::system::system_error &e)
    {
        TRACE_ERROR("Error while trying to open %s: \"%s\"", serialPortDevice.c_str(), e.what());
    }

    return 0;
}
