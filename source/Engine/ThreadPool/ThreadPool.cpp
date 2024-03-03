#include "ThreadPool.hpp"

J_SINGLETON_DEF(ThreadPoolManager)

ThreadPoolManager::ThreadPoolManager() :
	m_terminateThreads(false),
	m_dispatchMutex(), m_dispatchCondition(),
	m_threads(),
	m_waitMutex(), m_waitCondition(),
	m_jobIDCounter(0), m_jobs(), m_inactiveJobs() {
	// Create threads.
	createThreads();
}
ThreadPoolManager::~ThreadPoolManager() {
	// Cleanup threads.
	terminateThreads();

	// Cleanup jobs.
	for (auto it = m_jobs.begin(); it != m_jobs.end(); it++)
		delete it->second;
	m_jobs.clear();
}

ThreadJob* const ThreadPoolManager::getJob(const ThreadJobID jobID) {
	// Get job.
	auto it = m_jobs.find(jobID);
	if (it == m_jobs.end()) return nullptr;

	// Return job.
	return it->second;
}
const bool ThreadPoolManager::getJobAvailable(const ThreadJobID jobID) {
	// Get job.
	ThreadJob* const job = getJob(jobID);
	if (job == nullptr) return false;

	// Check dependencies.
	Stack<ThreadJobID>& dependencies = job->getDependencies();
	while (dependencies.size() > 0) {
		// Check if job has completed.
		if (getJob(dependencies.top()) != nullptr)
			return false;

		// Remove completed job.
		dependencies.pop();
	}

	// Job has no more dependencies, all good to go.
	return true;
}

const bool ThreadPoolManager::getJobActive(const ThreadJobID jobID) {
	// Find job.
	return m_jobs.find(jobID) != m_jobs.end();
}
const bool ThreadPoolManager::getJobsActive(const List<ThreadJobID>& jobIDs) {
	// Check for active jobs.
	for (size_t i = 0; i < jobIDs.size(); i++)
		if (getJobActive(jobIDs[i]))
			return true;

	// No active jobs found.
	return false;
}
const bool ThreadPoolManager::getJobsActiveMut(List<ThreadJobID>& jobIDs) {
	// Remove any inactive jobs.
	for (auto it = jobIDs.begin(); it != jobIDs.end();) {
		if (!getJobActive(*it)) {
			// Remove job.
			it = jobIDs.erase(it);
			continue;
		}
		// Job active, check next.
		it++;
	}

	// Check for remaining active jobs.
	return jobIDs.size() > 0;
}

void ThreadPoolManager::waitForJob(const ThreadJobID jobID) {
	// Wait for job.
	MutexLock lock(m_waitMutex);
	m_waitCondition.wait(lock, [this, jobID]() {
		return !getJobActive(jobID) || m_threads.size() > 0;
	});
}
void ThreadPoolManager::waitForJobs(const List<ThreadJobID>& jobIDs) {
	// Wait for jobs.
	for (size_t i = 0; i < jobIDs.size(); i++)
		waitForJob(jobIDs[i]);
}

const ThreadJobID ThreadPoolManager::enqueueJob(const Function<void()>& process, const List<ThreadJobID> dependencies) {
	// Get job ID.
	const ThreadJobID jobID = m_jobIDCounter;
	m_jobIDCounter = (m_jobIDCounter + 1) % (ThreadJobID_INVALID - 1);

	// Push job into storage.
	m_jobs.emplace(jobID, new ThreadJob(process, dependencies));
	m_inactiveJobs.emplace_back(jobID);

	// Dispatch a thread.
	m_dispatchCondition.notify_one();

	// Return job ID.
	return jobID;
}

const bool ThreadPoolManager::tryLock() {
	return m_dispatchMutex.try_lock();
}
void ThreadPoolManager::lock() {
	m_dispatchMutex.lock();
}
void ThreadPoolManager::unlock() {
	m_dispatchMutex.unlock();
}

void ThreadPoolManager::createThreads() {
	if (m_threads.size() > 0) return;

	// Get max hardware thread count.
	static constexpr size_t minimumThreadCount = 4;
	size_t targetThreadCount = (size_t)std::thread::hardware_concurrency();
	if (targetThreadCount <= 0) {
		// Set to default & report.
		targetThreadCount = minimumThreadCount;
		J_WARNING("ThreadPool.cpp: Unable to get hardware concurrency information, defaulting to %zu threads.\n", targetThreadCount);
	} else if (targetThreadCount < minimumThreadCount) {
		// Set to default & report.
		J_WARNING("ThreadPool.cpp: Hardware concurrency of %zu is less than minimum, defaulting to %zu threads.\n", targetThreadCount, minimumThreadCount);
		targetThreadCount = minimumThreadCount;
	}

	// Create threads.
	for (size_t i = 0; i < targetThreadCount; i++)
		m_threads.emplace_back(&ThreadPoolManager::threadLoop, this);

	// Report success.
	J_LOG("ThreadPool.cpp: Thread pool created with %zu threads.\n", targetThreadCount);
}
void ThreadPoolManager::terminateThreads() {
	if (m_threads.size() <= 0) return;

	{
		// Set termination flag.
		MutexLock lock(m_dispatchMutex);
		m_terminateThreads = true;
		// Notify all threads.
		m_dispatchCondition.notify_all();
	}

	// Join + clear all threads.
	for (size_t i = 0; i < m_threads.size(); i++)
		if (m_threads[i].joinable()) m_threads[i].join();
	m_threads.clear();

	// Notify waiting.
	m_waitCondition.notify_all();

	// Debug log.
	J_LOG("ThreadPool.cpp: Successfully cleared thread pool.");
}
void ThreadPoolManager::threadLoop() {
	// Job storage.
	ThreadJobID jobID;
	ThreadJob* job;

	while (!m_terminateThreads) {
		{
			// Wait for job.
			MutexLock lock(m_dispatchMutex);
			m_dispatchCondition.wait(lock, [this]() {
				return !m_inactiveJobs.empty() || m_terminateThreads;
			});

			// Check for termination flag.
			if (m_terminateThreads) break;

			// Take job.
			jobID = takeJob();
			job = getJob(jobID);

			// Check if another one can be dispatched.
			if (!m_inactiveJobs.empty())
				// Dispatch another thread.
				m_dispatchCondition.notify_one();
		}

		if (job != nullptr) {
			// Run job.
			job->process();

			{
				// Complete job.
				MutexLock lock(m_dispatchMutex);
				completeJob(jobID);
			}
		}
	}
}
const ThreadJobID ThreadPoolManager::takeJob() {
	// Find available job.
	for (auto it = m_inactiveJobs.begin(); it != m_inactiveJobs.end(); it++) {
		// Check if job available.
		const ThreadJobID jobID = *it;
		if (!getJobAvailable(jobID)) continue;

		// Remove job from inactive queue.
		m_inactiveJobs.erase(it);

		// Return job.
		return jobID;
	}

	// No jobs found.
	return ThreadJobID_INVALID;
}

void ThreadPoolManager::completeJob(const ThreadJobID jobID) {
	// Get job.
	auto it = m_jobs.find(jobID);
	if (it == m_jobs.end()) return;

	// Cleanup.
	delete it->second;
	// Remove from list.
	m_jobs.erase(it);

	// Notify wait.
	m_waitCondition.notify_all();
}