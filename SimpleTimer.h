#pragma once
#include<chrono>
class SimpleTimer
{
public:
	SimpleTimer();
	~SimpleTimer();

private:
	std::chrono::time_point<std::chrono::steady_clock> start, end;
};

