#pragma once
#include "vulkan/vulkan_core.h"
#include<Defines.h>
#include <map>
_VOXEL_BEGIN
struct MemoryStatus
{
	void* location;
	bool in_use;
};
struct ProfilingAllocationCallbacks : public VkAllocationCallbacks {
	static bool is_cleaningup;
	static std::map<uint32_t, std::vector<MemoryStatus>> memory_cache;
	static void* get_memory(uint32_t size);
	static void* allocation(void* pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope);
	static void* reallocation(void* pUserData, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope);
	static void free_mem(void* pUserData, void* pMemory);
};
_VOXEL_END