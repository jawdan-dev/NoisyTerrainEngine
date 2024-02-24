#pragma once
#include <NoisyTerrain/Core/Core.hpp>

#include <NoisyTerrain/ThreadPool/ThreadJob.hpp>

class ThreadPoolManager {
	J_SINGLETON(ThreadPoolManager)
#	define ThreadPool J_SINGLETON_GET(ThreadPoolManager)

private:
	bool m_terminateThreads;
	Mutex m_dispatchMutex;
	ConditionVariable m_dispatchCondition;
	List<std::thread> m_threads;

private:
	Mutex m_waitMutex;
	ConditionVariable m_waitCondition;

private:
	ThreadJobID m_jobIDCounter;
	Map<ThreadJobID, ThreadJob*> m_jobs;
	List<ThreadJobID> m_inactiveJobs;

public:
	ThreadPoolManager();
	ThreadPoolManager(const ThreadPoolManager& other) = delete;
	~ThreadPoolManager();

public:
	ThreadJob* const getJob(const ThreadJobID jobID);
	const bool getJobAvailable(const ThreadJobID jobID);

public:
	const bool getJobActive(const ThreadJobID jobID);
	const bool getJobsActive(const List<ThreadJobID>& jobIDs);
	const bool getJobsActiveMut(List<ThreadJobID>& jobIDs);

	void waitForJob(const ThreadJobID jobID);
	void waitForJobs(const List<ThreadJobID>& jobIDs);

public:
	const ThreadJobID enqueueJob(const Function<void()>& process, const List<ThreadJobID> dependencies = {});

public:
	const bool tryLock();
	void lock();
	void unlock();

public:
	void createThreads();
	void terminateThreads();

private:
	void threadLoop(const uint32_t _threadID);
	const ThreadJobID takeJob();
	void completeJob(const ThreadJobID jobID);
};