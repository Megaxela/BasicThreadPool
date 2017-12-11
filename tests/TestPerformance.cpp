
#include <gtest/gtest.h>
#include <thread>
#include <ThreadPool.hpp>
#include <atomic>
#include "TestingExtend.hpp"

class PerformanceCounter
{
public:

    PerformanceCounter() :
        m_averageTicksPerSecond(0),
        m_totalCounter(0),
        m_lastAverageCorrection(std::clock()),
        m_ticksCumulative(0)
    {}

    uint64_t averageTicksPerSecond() const
    {
        return m_averageTicksPerSecond;
    }

    void tick()
    {
        if (std::clock() - m_lastAverageCorrection > CLOCKS_PER_SEC)
        {
            m_lastAverageCorrection = std::clock();

            if (m_totalCounter != 0)
            {
                ++m_totalCounter;
                m_averageTicksPerSecond =
                    ((m_totalCounter - 1) * m_averageTicksPerSecond + m_ticksCumulative) / m_totalCounter;
            }
            else
            {
                m_totalCounter = 1;
                m_averageTicksPerSecond = m_ticksCumulative;
            }

            m_ticksCumulative = 0;
        }

        ++m_ticksCumulative;
    }

private:
    uint64_t m_averageTicksPerSecond;
    uint64_t m_totalCounter;

    std::clock_t m_lastAverageCorrection;
    uint64_t m_ticksCumulative;
};

TEST(Performace, SingleJob)
{
    // Calculating single thread with single job
    // in while(true) cycle

    PerformanceCounter singleThreadCounter;
    PerformanceCounter poolCounter;

    auto startTime = std::chrono::steady_clock::now();

    {
        TEST_COUT << "Running single thread for 10 seconds...";

        startTime = std::chrono::steady_clock::now();
        std::thread thread(
            [&singleThreadCounter, startTime]()
            {
                while (std::chrono::steady_clock::now() - startTime < std::chrono::seconds(10))
                {
                    singleThreadCounter.tick();
                }
            }
        );

        thread.join();

        TEST_COUT
            << "Thread finished with " << singleThreadCounter.averageTicksPerSecond() << " cycles/sec";
    }

    {
        TEST_COUT << "Running thread pool for 10 seconds...";

        startTime = std::chrono::steady_clock::now();
        ThreadPool pool(1);

        auto jobIndex = pool.addInfiniteJob(
            Job(
                [&poolCounter]()
                {
                    poolCounter.tick();
                    return nullptr;
                }
            )
        );

        // Waiting for 5 seconds...
        while (std::chrono::steady_clock::now() - startTime < std::chrono::seconds(10))
        {
            std::this_thread::sleep_for(
                std::chrono::milliseconds(100)
            );
        }

        // Removing shitty job
        pool.removeJob(jobIndex);

        TEST_COUT
            << "Thread pool finished with " << poolCounter.averageTicksPerSecond() << " cycles/sec";
    }

    if (poolCounter.averageTicksPerSecond() < singleThreadCounter.averageTicksPerSecond())
    {
        TEST_COUT
            << "Pool is slower than single thread in "
            << std::setprecision(2) << singleThreadCounter.averageTicksPerSecond() / (float) poolCounter.averageTicksPerSecond()
            << " times.";
    }
    else
    {
        TEST_COUT
            << "Single thread is slower than pool in "
            << std::setprecision(2) << poolCounter.averageTicksPerSecond() / (float) singleThreadCounter.averageTicksPerSecond()
            << " times. Hmm, but that's impossible...";
    }
}

//TEST(Performance, MultipleJobs)
//{
//    // Calculating single thread with single job
//    // in while(true) cycle
//
//    std::atomic_int singleThreadTimes(0);
//
//    TEST_COUT << "Running single thread for 5 seconds...";
//
//    auto startTime = std::chrono::steady_clock::now();
//
//    std::thread thread(
//        [&singleThreadTimes, startTime]()
//        {
//            while (std::chrono::steady_clock::now() - startTime < std::chrono::seconds(5))
//            {
//                count_atomic(singleThreadTimes);
//            }
//        }
//    );
//
//    thread.join();
//
//    auto singleCPMS = singleThreadTimes.load() / (5000.0);
//
//    TEST_COUT
//        << "Thread finished with "
//        << singleThreadTimes
//        << " iterates. "
//        << singleCPMS
//        << " cycles/ms";
//
//    TEST_COUT << "Running thread pool for 5 seconds...";
//
//    std::atomic_int threadPoolTimes(0);
//
//    ThreadPool pool(std::max(1u, std::thread::hardware_concurrency() - 1));
//
//    startTime = std::chrono::steady_clock::now();
//
//    std::vector<Job::Index> jobs;
//    jobs.reserve(std::max(1u, std::thread::hardware_concurrency() - 1) + 5);
//
//    for (uint32_t i = 0; i < jobs.capacity(); ++i)
//    {
//        jobs.push_back(
//            pool.addInfiniteJob(
//                Job(
//                    [&threadPoolTimes]()
//                    {
//                        count_atomic(threadPoolTimes);
//                        return nullptr;
//                    }
//                )
//            )
//        );
//    }
//
//    // Waiting for 5 seconds...
//    while (std::chrono::steady_clock::now() - startTime < std::chrono::seconds(5))
//    {
//        std::this_thread::sleep_for(
//            std::chrono::milliseconds(100)
//        );
//    }
//
//    // Removing shitty job
//    for (auto&& el : jobs)
//    {
//        pool.removeJob(el);
//    }
//
//    auto poolCPMS = threadPoolTimes.load() / (5000.0);
//
//    TEST_COUT
//        << "Thread pool finished with "
//        << threadPoolTimes
//        << " iterates. "
//        << poolCPMS
//        << " cycles/ms";
//
//    if (poolCPMS < singleCPMS)
//    {
//        TEST_COUT
//            << "Pool is slower than single thread in "
//            << std::setprecision(2) << singleCPMS / poolCPMS
//            << " times.";
//    }
//    else
//    {
//        TEST_COUT
//            << "Single thread is slower than pool in "
//            << std::setprecision(2) << poolCPMS / singleCPMS
//            << " times. Hmm, but that's impossible...";
//    }
//}