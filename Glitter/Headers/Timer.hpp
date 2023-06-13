#pragma once

/// <summary>
/// TimeData used by the Timer class
/// </summary>
struct TimeData
{
	float DeltaTime;
};

/// <summary>
/// Timer class that manages scene timekeeping
/// </summary>
class Timer
{
public:
	Timer();

	/// <summary>
	/// Tick the timekeeping, updating internal TimeData
	/// </summary>
	void Tick();

	/// <summary>
	/// Get the TimeData for this timer
	/// 
	/// XXX: inlined here because this is a small, often called function
	/// When this is in a CPP file, the linker screams :(
	/// </summary>
	/// <returns></returns>
	inline TimeData Timer::GetData()
	{
		return m_timeData;
	}

private:
	float m_prevTime;
	float m_currentTime;
	TimeData m_timeData;
};
