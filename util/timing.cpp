#include "timing.h"

Timer::Timer(const std::string& name, std::ostream& out) : startPoint(std::chrono::high_resolution_clock::now()), funcName(name), logFile(out) 
{}

Timer::~Timer() {Stop();}

void Timer::Stop()
{
    auto endPoint = std::chrono::high_resolution_clock::now();

    auto start = std::chrono::time_point_cast<std::chrono::microseconds>(startPoint).time_since_epoch().count();
    auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endPoint).time_since_epoch().count();

    auto duration = end - start;

    logFile << funcName << ": " << duration << "μs" << std::endl;
}