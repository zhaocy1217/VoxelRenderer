#pragma once
#include<PostProcessing.h>
_VOXEL_BEGIN
class BlurAndACESPostProcessing :public PostProcessing
{
public:
	bool blur = true;
	bool aces = true;
	void on_render_post(RenderContext& context);

};
_VOXEL_END