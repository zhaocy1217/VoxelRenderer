#pragma once
#include "GComp.h"
class TestComp:public VoxelRendering::GComp
{
public:
	static std::string name;
	glm::vec3 offset;
	void update(float dt) override;
};