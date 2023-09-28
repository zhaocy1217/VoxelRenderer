#include<MemAllocator.h>
_VOXEL_BEGIN

std::map<uint32_t, std::vector<MemoryStatus>> ProfilingAllocationCallbacks::memory_cache;
bool ProfilingAllocationCallbacks::is_cleaningup;
void* ProfilingAllocationCallbacks::allocation(void* pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope) {
	void* addr = get_memory(size);
	return addr;
}
void* ProfilingAllocationCallbacks::reallocation(void* pUserData, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope) {
	return realloc(pOriginal, size);
}
void ProfilingAllocationCallbacks::free_mem(void* pUserData, void* pMemory) {
	bool found = false;
	for (auto& pair : memory_cache)
	{
		auto& vec = pair.second;
		for (auto& ms : vec)
		{
			if (ms.location == pMemory)
			{
				ms.in_use = false;
				return;
			}
		}
	}
	free(pMemory);
}
void* ProfilingAllocationCallbacks::get_memory(uint32_t size)
{
	uint32_t containerSize = pow(2, ceill(log2(size)));
	auto rst = memory_cache.find(containerSize);
	if (rst == memory_cache.end())
	{
		memory_cache[containerSize] = std::vector<MemoryStatus>();
	}
	std::vector<MemoryStatus>& caches = memory_cache[containerSize];
	for (size_t i = 0; i < caches.size(); i++)
	{
		if (!caches[i].in_use)
		{
			caches[i].in_use = true;
			return caches[i].location;
		}
	}
	MemoryStatus ms;
	ms.in_use = true;
	ms.location = malloc(containerSize);
	caches.push_back(ms);
	return ms.location;
}
_VOXEL_END


