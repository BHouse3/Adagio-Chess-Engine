#pragma once

#include <chrono>
#include <iostream>
#include <fstream>

#ifdef ENABLE_TIMING
#define Timing(name, logFile) Timer timer(name, logFile);
#else
#define Timing(name)
#endif

class Timer
{
    public:
        Timer(const std::string& name, std::ostream& out = std::cout);

        ~Timer();

        void Stop();
    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> startPoint;
        std::string funcName;
        std::ostream& logFile;
};