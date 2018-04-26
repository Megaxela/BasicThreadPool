//
// Created by megaxela on 11/14/17.
//

#pragma once

#include <thread>
#include <vector>
#include <functional>
#include <deque>
#include <mutex>
#include <shared_mutex>
#include "JobResult.hpp"
#include "Job.hpp"
#include <ringbuffer.hpp>

/**
 * @brief Main thread pool class.
 */
class ThreadPool
{

    /**
     * @brief Container for job.
     */
    struct JobContainer
    {
        JobContainer() :
            job(),
            isInfinite(false),
            result()
        {}

        JobContainer(Job j, bool isInfinite) :
            job(std::move(j)),
            isInfinite(isInfinite),
            result()
        {}

        JobContainer(Job j, JobResult result) :
            job(std::move(j)),
            isInfinite(false),
            result(std::move(result))
        {}

        Job job{};
        bool isInfinite;
        JobResult result{};
    };

    struct ThreadContainer
    {
        ThreadContainer() :
            thread(),
            running(false)
        {}

        explicit ThreadContainer(std::thread thread) :
            thread(std::move(thread)),
            running(true)
        {}

        std::thread thread;
        bool running;
    };

public:

    using JobsContainer = ringbuffer<JobContainer, 512>;

    /**
     * @brief Constructor.
     * @param threads Number of threads.
     */
    explicit ThreadPool(uint32_t threads=1);

    /**
     * @brief Destructor.
     */
    ~ThreadPool();

    /**
     * @brief Method for changing number of
     * active threads.
     * @param threads Threads.
     */
    void changeNumberOfThreads(uint32_t threads);

    /**
     * @brief Method for getting number of threads.
     * @return Number of threads.
     */
    uint32_t numberOfThreads() const;

    /**
     * @brief Method for adding job to thread pool.
     * @param job Job.
     * @return Job result. It will contain job result
     * when job will be finished.
     */
    JobResult addJob(Job job);

    /**
     * @brief Method for adding job that does not has
     * result and will be pushed to job queue after it'll be finished.
     * @param job Job.
     * @param Job index.
     */
    Job::Index addInfiniteJob(Job job);

    /**
     * @brief Method for removing job from event queue.
     * If there is no such queue nothing happen.
     * @param index Job index.
     */
    void removeJob(Job::Index index);

    /**
     * @brief Method for checking is job inside thread
     * job pool. (If job is executing it's not in job pool.
     * @param index Job index.
     * @return Is job in job pool.
     */
    bool containsJob(Job::Index index) const;

private:

    /**
     * @brief Workers threads job.
     * @param index Index in m_threadContainer.
     */
    void workerThread(int index);

    std::vector<ThreadContainer> m_threadContainer;
    mutable std::shared_mutex m_threadMutex;

    JobsContainer m_jobs;
    std::condition_variable_any m_jobsCondition;
    mutable std::mutex m_jobsMutex;

    std::vector<Job::Index> m_removedJobs;
    mutable std::mutex m_removedJobsMutex;

    Job::Index m_indexCounter;
    std::mutex m_indexMutex;
};

