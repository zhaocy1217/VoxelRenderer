#pragma once
#include "Defines.h"
#include<memory>
#include"GObject.h"
_VOXEL_BEGIN
class GObject;
class GComp
{
public:
	VoxelRendering::GObject* go;
	virtual void update(float dt);
	virtual ~GComp();
};
_VOXEL_END