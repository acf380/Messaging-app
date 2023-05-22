#include "Logger.h"


void LogMessage(const char* message, char logLevel)
{
    time_t currentTime;
    struct tm* localTime;

    time( &currentTime );               // Get the current time
    localTime = localtime( &currentTime );  // Convert the current time to the local time

    int Day    = localTime->tm_mday;
    int Month  = localTime->tm_mon + 1;
    int Year   = localTime->tm_year + 1900;
    int Hour   = localTime->tm_hour;
    int Min    = localTime->tm_min;
    int Sec    = localTime->tm_sec;

    std::string logLevelStr;
    switch (logLevel)
    {
        case 'i':
            logLevelStr = "[INFO]";
            break;
        case 'w':
            logLevelStr = "[WARN]";
            break;
        case 'e':
            logLevelStr = "[ERROR]";
            break;
        default:
            logLevelStr = "[LOG]";
    }

    std::cout << Year << "-" << Month << "-" << Day << " " << Hour << ":" << Min << ":" << Sec << " " << logLevelStr << " " << message << std::endl;
}
