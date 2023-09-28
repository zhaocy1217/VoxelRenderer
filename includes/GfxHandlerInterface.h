#pragma once
#include<Defines.h>
_VOXEL_BEGIN
class GfxHandlerInterface
{
public:
	GfxHandlerInterface();
	GfxHandlerInterface(void* handler) :handler(handler) {};
	void* handler = nullptr;
};
_VOXEL_END