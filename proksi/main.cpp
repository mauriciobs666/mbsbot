#include <MBSUtil.h>
#include <iostream>
#include <boost/asio.hpp>

#ifdef _WIN32
    #define DEFAULT_SERIAL_DEVICE "COM1"
#else
	#define DEFAULT_SERIAL_DEVICE "/dev/ttyUSB0"
#endif

std::string serialPortDevice(DEFAULT_SERIAL_DEVICE);
int baudRate = 115200;
unsigned short listenPort = 6661;

int main(int argc, char* argv[])
{
    for(int a=1; a<argc; a++)
    {
        if( argv[a][0] == '-' )
        {
            switch(argv[a][1])
            {
                case '?':
                    std::cout << "Usage:" << std::endl << "proksi <device> <tcp port>" << std::endl;
                break;
            }
        }
        else if( isalpha(argv[a][0]) || (argv[a][0] == '/') )
        {
            serialPortDevice = argv[a];
        }
        else if( isdigit(argv[a][0]) )
        {
            listenPort = atoi(argv[a]);
        }
        else
        {
            TRACE_ERROR("WTF %s", argv[a]);
        }
    }

    boost::asio::io_service io;

    try
    {
        boost::asio::serial_port serial(io, serialPortDevice);
        serial.set_option(boost::asio::serial_port_base::baud_rate(baudRate));
        TRACE_INFO("Opened %s @ %d bps", serialPortDevice.c_str(), baudRate);
        char c;
        std::string result;
        for(;;)
        {
            boost::asio::read(serial, boost::asio::buffer(&c,1));
            switch(c)
            {
                case '\r':
                    break;
                case '\n':
                    std::cout << result;
                    result.clear();
                default:
                    result+=c;
            }
        }
    }
    catch(boost::system::system_error &e)
    {
        TRACE_ERROR("Error while trying to open %s: \"%s\"", serialPortDevice.c_str(), e.what());
    }

    return 0;
}
