//
// Created by megaxela on 11/14/17.
//

#pragma once
#include <cstdint>
#include <functional>
#include <memory>

/**
 * @brief Class, that describes job for thread pool.
 */
class Job
{
    friend class ThreadPool;
public:
    using Index = uint32_t;

    using Result = std::shared_ptr<void>;

    using FunctionType = std::function<Result()>;

    /**
     * @brief Default constructor.
     */
    Job();

    /**
     * @brief Constructor.
     * @param function Function for job.
     */
    explicit Job(FunctionType function);

    /**
     * @brief Method for getting job index.
     * @return Job index.
     */
    Index index() const;

    /**
     * @brief Method for getting job function to execute.
     * @return
     */
    FunctionType function() const;

private:
    void setIndex(Index index);

    FunctionType m_function;
    Index m_index;
};

