#include "Time.h"
CTime::CTime()
{
	m_tpCurrent = m_tpPrevious = std::chrono::system_clock::now();
	Tick();
}

void CTime::Tick()
{
	m_tpCurrent = std::chrono::system_clock::now();
	m_dElapsedTime = m_tpCurrent - m_tpPrevious;
	m_tpPrevious = m_tpCurrent;
}