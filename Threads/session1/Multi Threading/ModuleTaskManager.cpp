#include "ModuleTaskManager.h"


void ModuleTaskManager::threadMain()
{
	while (true)
	{
		// TODO 3:
		// - Wait for new tasks to arrive
		{
			std::unique_lock<std::mutex> lock(mtx);
			while (scheduledTasks.empty()) {
				if (exitFlag)
					break;
				event.wait(lock);
			}
			if (exitFlag)
				break;
		}
		// - Retrieve a task from scheduledTasks
		{
			std::unique_lock<std::mutex> lock(mtx);
			if (!scheduledTasks.empty()) {
				auto task = scheduledTasks.front();
				scheduledTasks.pop();
				// - Execute it
				task->execute();
				// - Insert it into finishedTasks
				finishedTasks.push(task);
			}
		}
	}
}

bool ModuleTaskManager::init()
{
	// TODO 1: Create threads (they have to execute threadMain())

	for (int i = 0; i < MAX_THREADS; ++i) {
		threads[i] = std::thread(&ModuleTaskManager::threadMain, this);
	}

	return true;
}

bool ModuleTaskManager::update()
{
	// TODO 4: Dispatch all finished tasks to their owner module (use Module::onTaskFinished() callback)
	std::unique_lock<std::mutex> lock(mtx);
	while (!finishedTasks.empty()) {
		auto task = finishedTasks.front();
		finishedTasks.pop();
		task->owner->onTaskFinished(task);
	}

	return true;
}

bool ModuleTaskManager::cleanUp()
{
	{
		std::unique_lock<std::mutex> lock(mtx);
		exitFlag = true;
	}
	// TODO 5: Notify all threads to finish and join them
	event.notify_all();
	for (int i = 0; i < MAX_THREADS; ++i) {
		threads[i].join();
	}

	return true;
}

void ModuleTaskManager::scheduleTask(Task *task, Module *owner)
{
	task->owner = owner;

	// TODO 2: Insert the task into scheduledTasks so it is executed by some thread
	std::unique_lock<std::mutex> lock(mtx);
	scheduledTasks.push(task);
	event.notify_one();
}
