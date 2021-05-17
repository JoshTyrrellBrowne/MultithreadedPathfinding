#include "ThreadPool.h"


ThreadPool::ThreadPool()
{
    is_terminante_pool = false; // initially false
    initThreads();
}

void ThreadPool::addJob(std::function<void()> newJob)
{
    std::unique_lock<std::mutex> lock(queue_Mutex);
    m_function_queue.push(newJob);
    // when we send the notification immediately, the consumer will try to get the lock , so unlock asap
    lock.unlock();
    m_condition.notify_one();  // notify the next thread waiting for lock
}

void ThreadPool::loopingFunction()
{
    while (true)
    {
        std::unique_lock<std::mutex> lock(queue_Mutex);

        m_condition.wait(lock, [this]() {return !m_function_queue.empty() || is_terminante_pool; });  // wait until queue is not empty or pool is terminated
        auto Job = m_function_queue.front();  // get next job
        m_function_queue.pop();  // pop next job, because this thread has just taken it 
        
        Job(); // function<void()> type, DO THE JOB
    }
}

void ThreadPool::initThreads()
{
    for (int i = 0; i < num_threads; i++)
    {
        std::thread thread = std::thread(&ThreadPool::loopingFunction, this);
        m_thread_pool.push_back(move(thread));
    }
}

void ThreadPool::shutdown()
{
    std::unique_lock<std::mutex> lock(queue_Mutex);
    is_terminante_pool = true;

    m_condition.notify_all();  // tell all threads

    // Join all threads
    for (std::thread &thread : m_thread_pool)
    {
        thread.join();
    }

    m_thread_pool.clear();
}

