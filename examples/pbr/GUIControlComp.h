#pragma once
#include<GComp.h>
#include <BlurAndACESPostProcessing.h>
class GUIControlComp : public  VoxelRendering::GComp
{
private:
	bool show_blur;
	bool show_aces;
	float posi[3];
	float rot[3];
public:
	static std::string name;
	VoxelRendering::BlurAndACESPostProcessing* pp;
	void gui();
	
};