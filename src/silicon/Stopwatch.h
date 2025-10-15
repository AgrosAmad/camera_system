/****************************************************************************
 *
 * ACTIVE SILICON LIMITED
 *
 * File name   : stopwatch.h
 * Function    : Stopwatch class.
 *
 * Copyright (c) 2019 Active Silicon Ltd.
 ****************************************************************************
 * Comments:
 * --------
 * To use this class, follow this pattern:
 *    1. Create an instance of the class nearby the code you want to measure.
 *    2. Call Start() method immediately before the code to measure.
 *    3. Call Stop() method immediately after the code to measure.
 *    4. Call ElapsedTimeSec() or ElapsedTimeMillisec() methods to get the 
 *       elapsed time (in seconds or milliseconds, respectively).
 ****************************************************************************
 */

#pragma once

class CStopwatch
{
public:
    // Does some initialization to get consistent results for all tests.
    CStopwatch()
    : m_qiStartCount(0), m_fdElapsedTimeSec(0.0)
    {
        // Confine the test to run on a single processor,
        // to get consistent results for all tests.
        SetThreadAffinityMask(GetCurrentThread(), 1);
        SetThreadIdealProcessor(GetCurrentThread(), 0);
        Sleep(1);
    }

    // Starts measuring performance
    // (to be called before the block of code to measure).
    void Start()
    {
        // Clear total elapsed time 
        // (it is a spurious value until Stop() is called)
        m_fdElapsedTimeSec = 0.0;
        // Start ticking
        m_qiStartCount = Counter();
    }

    // Stops measuring performance
    // (to be called after the block of code to measure).
    void Stop()
    {
        // Stop ticking
        LONGLONG qiStopCount = Counter();
        // Calculate total elapsed time since Start() was called;
        // time is measured in seconds
        m_fdElapsedTimeSec = (qiStopCount - m_qiStartCount) * 1.0 / Frequency();
        // Clear start count (it is spurious information)
        m_qiStartCount = 0;
    }

    // Returns total elapsed time (in seconds) in Start-Stop interval.
    double ElapsedTimeSec() const
    {
        // Total elapsed time was calculated in Stop() method.
        return m_fdElapsedTimeSec;
    }

    // Returns total elapsed time (in milliseconds) in Start-Stop interval.
    double ElapsedTimeMilliSec() const
    {
        // Total elapsed time was calculated in Stop() method.
        return m_fdElapsedTimeSec * 1000.0;
    }

    // Returns current value of high-resolution performance counter.
    LONGLONG Counter() const
    {
        LARGE_INTEGER counter;
        QueryPerformanceCounter(&counter);
        return counter.QuadPart;
    }

    // Returns the frequency (in counts per second) of the 
    // high-resolution performance counter.
    LONGLONG Frequency() const
    {
        LARGE_INTEGER frequency;
        QueryPerformanceFrequency(&frequency);
        return frequency.QuadPart;
    }

private:
    // The value of counter on start ticking
    LONGLONG m_qiStartCount;

    // The time (in seconds) elapsed in Start-Stop interval
    double m_fdElapsedTimeSec;

private:
    CStopwatch(const CStopwatch &);
    CStopwatch & operator=(const CStopwatch &);
};
