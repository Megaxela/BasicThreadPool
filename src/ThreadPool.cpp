//
// Created by megaxela on 11/14/17.
//

#include <utility>
#include <algorithm>
#include <iostream>

#include "ThreadPool.hpp"

ThreadPool::ThreadPool(uint32_t threads) :
    m_threadContainer(),
    m_threadMutex(),
    m_jobs(),
    m_jobsCondition(),
    m_jobsMutex(),
    m_indexCounter(0),
    m_indexMutex()
{
    changeNumberOfThreads(threads);
}

ThreadPool::~ThreadPool()
{
    changeNumberOfThreads(0);
}

void ThreadPool::changeNumberOfThreads(uint32_t threads)
{
    std::unique_lock<std::mutex> lock(m_threadMutex);
    if (m_threadContainer.size()  > threads)
    {
        // Removing threads
        auto difference = m_threadContainer.size() - threads;

        // Disabling threads
        for (decltype(difference) i = 0;
             i < difference;
             ++i)
        {
            m_threadContainer[m_threadContainer.size() - i - 1].running = false;
        }

        m_jobsCondition.notify_all();

        // Waiting for threads to end
        lock.unlock();

        for (decltype(difference) i = 0;
             i < difference;
             ++i)
        {
            if (m_threadContainer[m_threadContainer.size() - i - 1].thread.joinable())
            {
                m_threadContainer[m_threadContainer.size() - i - 1].thread.join();
            }
        }

        lock.lock();

        // Removing em
        m_threadContainer.erase(
            m_threadContainer.begin() + (m_threadContainer.size() - difference),
            m_threadContainer.end()
        );
    }
    else if (m_threadContainer.size() < threads)
    {
        // Adding new threads
        auto difference = threads - m_threadContainer.size();

        for (decltype(difference) i = 0;
             i < difference;
             ++i)
        {
            m_threadContainer.emplace_back(
                std::thread(&ThreadPool::workerThread, this, m_threadContainer.size())
            );
        }
    }
}

uint32_t ThreadPool::numberOfThreads() const
{
    std::unique_lock<std::mutex> lock(m_threadMutex);
    return static_cast<uint32_t>(m_threadContainer.size());
}

JobResult ThreadPool::addJob(Job job)
{

    {
        std::unique_lock<std::mutex> lock(m_indexMutex);
        job.setIndex(m_indexCounter++);
    }

    auto result = JobResult(job);

    {
        std::unique_lock<std::mutex> lock(m_jobsMutex);
        m_jobs.emplace_back(job, result);
    }

    m_jobsCondition.notify_all();

    return result;
}

Job::Index ThreadPool::addInfiniteJob(Job job)
{
    {
        std::unique_lock<std::mutex> lock(m_indexMutex);
        job.setIndex(m_indexCounter++);
    }

    {
        std::unique_lock<std::mutex> lock(m_jobsMutex);
        m_jobs.emplace_back(job, true);
    }

    m_jobsCondition.notify_all();

    return job.index();
}

void ThreadPool::removeJob(Job::Index index)
{
    std::unique_lock<std::mutex> lock(m_jobsMutex);

    auto result = std::find_if(
        m_jobs.begin(),
        m_jobs.end(),
        [index](const JobContainer& c)
        {
            return c.job.index() == index;
        }
    );

    if (result == m_jobs.end())
    {
        return;
    }

    m_jobs.erase(result);
}

bool ThreadPool::containsJob(Job::Index index) const
{
    std::unique_lock<std::mutex> lock(m_jobsMutex);

    auto result = std::find_if(
        m_jobs.begin(),
        m_jobs.end(),
        [index](const JobContainer& c)
        {
            return c.job.index() == index;
        }
    );

    return result != m_jobs.end();
}

void ThreadPool::workerThread(int index)
{
    std::unique_lock<std::mutex> threadLock(m_threadMutex);

    while (m_threadContainer[index].running)
    {
        threadLock.unlock();

        // Taking job and executing it
        JobContainer jobContainer;

        {
            std::unique_lock<std::mutex> jobsLock(m_jobsMutex);

            threadLock.lock();
            // If there is no jobs, going to sleep.
            while (m_jobs.empty() &&
                   m_threadContainer[index].running)
            {
                threadLock.unlock();

                m_jobsCondition.wait(jobsLock);

                threadLock.lock();
            }

            if (!m_threadContainer[index].running)
            {
                return;
            }

            threadLock.unlock();

            // When wake up, take that job and execute it
            jobContainer = m_jobs.front();
            m_jobs.pop_front();
        }

        if (jobContainer.isInfinite)
        {
            // If it's infinite, just
            // execute it and push back.

            jobContainer.job.function()();

            std::unique_lock<std::mutex> jobsLock(m_jobsMutex);
            m_jobs.push_back(jobContainer);
        }
        else
        {
            // If it's not infinite, execute and update JobResult
            // object

            jobContainer.result.m_impl->set(jobContainer.job.function()());
        }

        threadLock.lock();
    }
}
