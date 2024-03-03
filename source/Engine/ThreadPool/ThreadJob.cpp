#include "ThreadJob.hpp"

ThreadJob::ThreadJob(const Function<void()>& process, const List<ThreadJobID>& dependencies) :
	m_process(process), m_dependencies() {
	// Add dependencies to stack.
	for (size_t i = 0; i < dependencies.size(); i++)
		m_dependencies.push(dependencies[i]);
}
ThreadJob::~ThreadJob() {}

void ThreadJob::process() {
	m_process();
}