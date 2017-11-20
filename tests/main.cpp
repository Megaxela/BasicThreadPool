#include <iostream>
#include "ThreadPool.hpp"

Job::Result counter1()
{
    std::this_thread::sleep_for(
        std::chrono::milliseconds(1000)
    );

    return std::make_shared<int>(100);
}

Job::Result counter2()
{
    std::this_thread::sleep_for(
        std::chrono::milliseconds(250)
    );

    return std::make_shared<int>(120);
}

int main()
{
    ThreadPool pool(5);

    auto r1 = pool.addJob(Job(counter1));
    auto r2 = pool.addJob(Job(counter2));

    std::cout << r1.get<int>() + r2.get<int>() << std::endl;

    return 0;
}