#pragma once
#include <chrono>
class CTime
{
private:
	std::chrono::duration<double> m_dElapsedTime;
	std::chrono::time_point<std::chrono::system_clock> m_tpCurrent, m_tpPrevious;

public:
	CTime();
	~CTime() = default;
	void Tick();
	double Delta(){ return m_dElapsedTime.count(); }
};