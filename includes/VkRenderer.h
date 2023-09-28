#pragma once
#include "Defines.h"
#include <Mesh.h>
#include <Material.h>
#include <Transform.h>
#include<GObject.h>
#include<GComp.h>
#include<VkPool.h>
#include<VkAssetWrapper.h>
#include<GfxHandlerInterface.h>
#include<GfxRenderDataset.h>
_VOXEL_BEGIN
class Material;
class Mesh;
class VkMeshWrapper;
class VkShaderWrapper;
class VkBufferWrapper;
class GfxRenderDataset;
class VkRenderer
{
private:
	int id;
	GfxRenderDataset* dataset;
	std::map<std::string, VkBufferWrapper> vk_ubo_buffer_info;
	BuiltinPropUbo mvp_ubo;
	void gen_ubo();
	void update_ubo(BuiltinPropUbo& ubo);
	void update_descriptor_set(VkDescriptorSet desc_set);
public:
	VkRenderer(GfxRenderDataset* ds);
	void init(GfxRenderDataset* dataSet);
	void draw(VkCommandBuffer command_buffer, VkRenderPass& pass);
	GfxRenderDataset* get_dataset();
	~VkRenderer();
};
_VOXEL_END