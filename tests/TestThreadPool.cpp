//
// Created by megaxela on 11/20/17.
//

#include "gtest/gtest.h"
#include <thread>
#include <ThreadPool.hpp>
#include <chrono>

static Job::Result counter1()
{
    return std::make_shared<int>(100);
}

static Job::Result counter2()
{
    return std::make_shared<int>(200);
}

static Job::Result counter3()
{
    return std::make_shared<int>(300);
}

static Job::Result counter4()
{
    return std::make_shared<int>(400);
}

static Job::Result counter5()
{
    return std::make_shared<int>(500);
}
static Job::Result counter6()
{
    return std::make_shared<int>(600);
}

TEST(ThreadPool, JobsBasic)
{
    // Creating thread pool
    ThreadPool pool(std::thread::hardware_concurrency());

    // Pushing calculation threads
    auto result1 = pool.addJob(Job(counter1));
    auto result2 = pool.addJob(Job(counter2));
    auto result3 = pool.addJob(Job(counter3));
    auto result4 = pool.addJob(Job(counter4));
    auto result5 = pool.addJob(Job(counter5));
    auto result6 = pool.addJob(Job(counter6));

    // Checking results
    ASSERT_EQ(result1.get<int>(), 100);
    ASSERT_EQ(result2.get<int>(), 200);
    ASSERT_EQ(result3.get<int>(), 300);
    ASSERT_EQ(result4.get<int>(), 400);
    ASSERT_EQ(result5.get<int>(), 500);
    ASSERT_EQ(result6.get<int>(), 600);
}

static Job::Result resultLessCounter(int& counter)
{
    std::this_thread::sleep_for(
        std::chrono::milliseconds(100)
    );

    counter += 100;
}

TEST(ThreadPool, InfiniteJob)
{
    ThreadPool pool(std::thread::hardware_concurrency());

    int value = 0;

    auto id = pool.addInfiniteJob(
        Job(std::bind(resultLessCounter, std::ref(value)))
    );

    std::this_thread::sleep_for(
        std::chrono::milliseconds(1000)
    );

    pool.removeJob(id);

    auto copy = value;

    std::this_thread::sleep_for(
        std::chrono::milliseconds(1000)
    );

    ASSERT_GT(value, copy);
}

static Job::Result resultLessCounter2(int& counter)
{
    std::this_thread::sleep_for(
        std::chrono::milliseconds(1000)
    );

    counter += 100;
}

TEST(ThreadPool, RemovingInfiniteJob)
{
    ThreadPool pool(std::thread::hardware_concurrency());

    std::this_thread::sleep_for(
        std::chrono::milliseconds(500)
    );

    int value = 0;

    auto id = pool.addInfiniteJob(
        Job(std::bind(resultLessCounter, std::ref(value)))
    );

    std::this_thread::sleep_for(
        std::chrono::milliseconds(500)
    );

    pool.removeJob(id);

    auto copy = value;

    std::this_thread::sleep_for(
        std::chrono::milliseconds(3000)
    );

    ASSERT_GT(value, copy);
}