//
// Created by megaxela on 11/14/17.
//

#pragma once
#include <cstdint>
#include <memory>
#include <condition_variable>
#include "Job.hpp"

/**
 * @brief Class, that describes job result.
 */
class JobResult
{
public:

    /**
     * @brief Default constructor.
     */
    JobResult();

    /**
     * @brief Constructor from job object.
     * @param job Job object.
     */
    JobResult(const Job &job);

    /**
     * @brief Method for waiting until worker thread
     * will complete task.
     */
    void waitForResult() const
    {
        if (m_impl)
        {
            m_impl->waitForResult();
        }
    }

    /**
     * @brief Method for getting value from result.
     * If result is not ready, wait for it.
     * @tparam T Result type.
     * @return Copy of value.
     */
    template<typename T>
    T get()
    {
        if (m_impl)
        {
            waitForResult();
            return *m_impl->get<T>();
        }
    }

private:

    friend class ThreadPool;

    /**
     * @brief Thread safe job result implementation.
     */
    class Implementation
    {
    public:
        /**
         * @brief Constructor.
         */
        Implementation();

        /**
         * @brief Constructor.
         * @param job Job object.
         */
        Implementation(const Job& job);

        /**
         * @brief Method for getting result value.
         * @tparam T
         * @return
         */
        template<typename T>
        std::shared_ptr<T> get()
        {
            return std::static_pointer_cast<T>(m_data);
        }

        /**
         * @brief Method for setting result value
         * and notify all waiting threads.
         * @param data Pointer to data.
         */
        void set(std::shared_ptr<void> data);

        /**
         * @brief Method for waiting until
         * job will return result.
         */
        void waitForResult();

    private:
        Job m_job;

        std::shared_ptr<void> m_data;
        bool m_resultReceived;
        std::condition_variable m_conditionVariable;
        mutable std::mutex m_mutex;
    };

    std::shared_ptr<Implementation> m_impl;

};

