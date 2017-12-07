
#include <gtest/gtest.h>
#include <thread>
#include <ThreadPool.hpp>
#include <atomic>
#include "TestingExtend.hpp"

void count_something(uint64_t& times)
{
    times += 1;
}

TEST(Performace, SingleJob)
{
    // Calculating single thread with single job
    // in while(true) cycle

    uint64_t singleThreadTimes = 0;

    TEST_COUT << "Running single thread for 5 seconds...";

    auto startTime = std::chrono::steady_clock::now();

    std::thread thread(
        [&singleThreadTimes, startTime]()
        {
            while (std::chrono::steady_clock::now() - startTime < std::chrono::seconds(5))
            {
                count_something(singleThreadTimes);
            }
        }
    );

    thread.join();

    auto singleCPMS = singleThreadTimes / (5000.0);

    TEST_COUT
        << "Thread finished with "
        << singleThreadTimes
        << " iterates. "
        << singleCPMS
        << " cycles/ms";

    TEST_COUT << "Running thread pool for 5 seconds...";

    uint64_t threadPoolTimes = 0;

    ThreadPool pool(std::max(1u, std::thread::hardware_concurrency() - 1));

    startTime = std::chrono::steady_clock::now();
    auto jobIndex = pool.addInfiniteJob(
        Job(
            [&threadPoolTimes]()
            {
                count_something(threadPoolTimes);
                return nullptr;
            }
        )
    );

    // Waiting for 5 seconds...
    while (std::chrono::steady_clock::now() - startTime < std::chrono::seconds(5))
    {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(100)
        );
    }

    // Removing shitty job
    pool.removeJob(jobIndex);

    auto poolCPMS = threadPoolTimes / (5000.0);

    TEST_COUT
        << "Thread pool finished with "
        << threadPoolTimes
        << " iterates. "
        << poolCPMS
        << " cycles/ms";

    if (poolCPMS < singleCPMS)
    {
        TEST_COUT
            << "Pool is slower than single thread in "
            << std::setprecision(2) << singleCPMS / poolCPMS
            << " times.";
    }
    else
    {
        TEST_COUT
            << "Single thread is slower than pool in "
            << std::setprecision(2) << poolCPMS / singleCPMS
            << " times. Hmm, but that's impossible...";
    }
}

void count_atomic(std::atomic_int& counter)
{
    ++counter;
}

TEST(Performance, MultipleJobs)
{
    // Calculating single thread with single job
    // in while(true) cycle

    std::atomic_int singleThreadTimes(0);

    TEST_COUT << "Running single thread for 5 seconds...";

    auto startTime = std::chrono::steady_clock::now();

    std::thread thread(
        [&singleThreadTimes, startTime]()
        {
            while (std::chrono::steady_clock::now() - startTime < std::chrono::seconds(5))
            {
                count_atomic(singleThreadTimes);
            }
        }
    );

    thread.join();

    auto singleCPMS = singleThreadTimes.load() / (5000.0);

    TEST_COUT
        << "Thread finished with "
        << singleThreadTimes
        << " iterates. "
        << singleCPMS
        << " cycles/ms";

    TEST_COUT << "Running thread pool for 5 seconds...";

    std::atomic_int threadPoolTimes(0);

    ThreadPool pool(std::max(1u, std::thread::hardware_concurrency() - 1));

    startTime = std::chrono::steady_clock::now();

    std::vector<Job::Index> jobs;
    jobs.reserve(std::max(1u, std::thread::hardware_concurrency() - 1) + 5);

    for (uint32_t i = 0; i < jobs.capacity(); ++i)
    {
        jobs.push_back(
            pool.addInfiniteJob(
                Job(
                    [&threadPoolTimes]()
                    {
                        count_atomic(threadPoolTimes);
                        return nullptr;
                    }
                )
            )
        );
    }

    // Waiting for 5 seconds...
    while (std::chrono::steady_clock::now() - startTime < std::chrono::seconds(5))
    {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(100)
        );
    }

    // Removing shitty job
    for (auto&& el : jobs)
    {
        pool.removeJob(el);
    }

    auto poolCPMS = threadPoolTimes.load() / (5000.0);

    TEST_COUT
        << "Thread pool finished with "
        << threadPoolTimes
        << " iterates. "
        << poolCPMS
        << " cycles/ms";

    if (poolCPMS < singleCPMS)
    {
        TEST_COUT
            << "Pool is slower than single thread in "
            << std::setprecision(2) << singleCPMS / poolCPMS
            << " times.";
    }
    else
    {
        TEST_COUT
            << "Single thread is slower than pool in "
            << std::setprecision(2) << poolCPMS / singleCPMS
            << " times. Hmm, but that's impossible...";
    }
}