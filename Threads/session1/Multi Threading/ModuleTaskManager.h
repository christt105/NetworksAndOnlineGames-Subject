#pragma once

#include "Module.h"
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

class Task
{
public:

	virtual void execute() = 0;

	Module *owner = nullptr;
};

class ModuleTaskManager : public Module
{
public:

	// Virtual method

	bool init() override;

	bool update() override;

	bool cleanUp() override;


	// To schedule new tasks

	void scheduleTask(Task *task, Module *owner);

	void threadMain();

private:

	static const int MAX_THREADS = 4;
	std::thread threads[MAX_THREADS];

	std::queue<Task*> scheduledTasks;
	std::queue<Task*> finishedTasks;
	std::mutex mtx;
	std::condition_variable event;

	bool exitFlag = false;
};

