//
// Created by megaxela on 11/14/17.
//

#include <utility>

#include "Job.hpp"

Job::Job() :
    m_function(nullptr),
    m_index(0)
{

}

Job::Job(Job::FunctionType function) :
    m_function(std::move(function)),
    m_index(0)
{

}

Job::Index Job::index() const
{
    return m_index;
}

void Job::setIndex(Job::Index index)
{
    m_index = index;
}

Job::FunctionType Job::function() const
{
    return m_function;
}
