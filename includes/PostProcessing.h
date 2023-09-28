#pragma once 
#include<RenderContext.h>
#include<RendererComp.h>
class RenderContext;
_VOXEL_BEGIN
class PostProcessing// :public RendererComp
{
public:
	PostProcessing() {}
	virtual void on_render_post(RenderContext& context);
	virtual ~PostProcessing();
};
_VOXEL_END