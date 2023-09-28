#include "GObject.h"
_VOXEL_BEGIN
void GObject::update(float dt)
{
	for (auto& pair : comps)
	{
		pair.second->update(dt);
	}
}
GObject::~GObject()
{
	comps.clear();
}
_VOXEL_END
