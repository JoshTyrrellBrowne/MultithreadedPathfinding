#pragma once

#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <queue>
#include <functional>
#include "Globals.h"
//#include "NPC.h"


class ThreadPool
{
public:
	ThreadPool();
	~ThreadPool() { shutdown(); };

	void addJob(std::function<void()> func);
	void loopingFunction(); // the looping function for the threads, look for a job then do it
	void initThreads();
	void shutdown();


private:
	int num_threads = std::thread::hardware_concurrency() - 1;

	std::vector<std::thread> m_thread_pool;    // the vector conataining all the threads
	
	
	std::queue<std::function<void()>> m_function_queue;  // Job Queue
	
	
	std::mutex queue_Mutex;  // mutex lock for thread taking a job & adding a job
	std::condition_variable m_condition;
	bool is_terminante_pool;
};