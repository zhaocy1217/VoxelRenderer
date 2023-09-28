#pragma once
#include<Defines.h>
#include<GComp.h>
#include<VkHelper.h>
_VOXEL_BEGIN
class RendererComp: public GComp
{
public:
	static std::string name;
	std::string mat_path;
	RendererComp();
	void init(std::string&& mat_path);
	void init(std::string& mat_path);
	
};
_VOXEL_END