#pragma once
#include <NoisyTerrain/Core/Core.hpp>

typedef uint64_t ThreadJobID;
static constexpr ThreadJobID ThreadJobID_INVALID = UINT64_MAX;

class ThreadPoolManager;

class ThreadJob {
private:
	Function<void()> m_process;
	Stack<ThreadJobID> m_dependencies;

public:
	ThreadJob(const Function<void()>& process, const List<ThreadJobID>& dependencies = {});
	ThreadJob(const ThreadJob& other) = delete;
	~ThreadJob();

public:
	J_GETTER_DIRECT(getDependencies, m_dependencies, Stack<ThreadJobID>&);
	J_GETTER_DIRECT_MUT(getDependencies, m_dependencies, Stack<ThreadJobID>&);

public:
	void process();
};