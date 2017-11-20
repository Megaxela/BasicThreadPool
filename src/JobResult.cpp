//
// Created by megaxela on 11/14/17.
//

#include <utility>

#include "JobResult.hpp"

JobResult::JobResult() :
    m_impl(nullptr)
{

}

JobResult::JobResult(const Job& job) :
    m_impl(std::make_shared<Implementation>(job))
{

}

JobResult::Implementation::Implementation(const Job& job) :
    m_job(job),
    m_data(nullptr),
    m_resultReceived(false),
    m_conditionVariable(),
    m_mutex()
{

}

JobResult::Implementation::Implementation() :
    m_job(),
    m_data(nullptr),
    m_resultReceived(false),
    m_conditionVariable(),
    m_mutex()
{

}

void JobResult::Implementation::set(std::shared_ptr<void> data)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_data = std::move(data);
    m_conditionVariable.notify_all();
    m_resultReceived = true;
}

void JobResult::Implementation::waitForResult()
{
    std::unique_lock<std::mutex> lock(m_mutex);

    while (!m_resultReceived)
    {
        m_conditionVariable.wait(lock);
    }
}

