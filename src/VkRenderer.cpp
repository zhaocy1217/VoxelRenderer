#include<Defines.h>
#include<VkRenderer.h>
#include <Resources.h>
#include <Camera.h>
#include<chrono>
#include<Util.h>
_VOXEL_BEGIN

VkRenderer::VkRenderer(GfxRenderDataset* ds)
{
	id = Util2::getuid();
	init(ds);

}
void VkRenderer::init(GfxRenderDataset* dataset)
{
	this->dataset = dataset;
	gen_ubo();
}
void VkRenderer::draw(VkCommandBuffer command_buffer, VkRenderPass& pass)
{
	auto m_vk_shader = (VkShaderWrapper*)(dataset->shader_gfx_handler);
	auto m_vk_mesh = (VkMeshWrapper*)(dataset->mesh_gfx_handler);
	auto shader_layout = m_vk_shader->layout;
	VkPipeline pipeline;
    VkHelper::vk_instance->pool->acquire_pipeline(pass, *m_vk_shader,&pipeline, dataset->is_transparent, 1);
	bool cached = false;
    auto descSet = VkHelper::vk_instance->pool->acquire_descriptor_set(m_vk_shader->layout, id, &cached);
	if (!cached)
	{
		update_descriptor_set(descSet);
	}
    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    VkBuffer vertexBuffers[] = { m_vk_mesh->vertex_buffer.buffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(command_buffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(command_buffer, m_vk_mesh->index_buffer.buffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_vk_shader->pipeline_layout, 0, 1, &descSet, 0, nullptr);
    vkCmdDrawIndexed(command_buffer, static_cast<uint32_t>(m_vk_mesh->triangle_size), 1, 0, 0, 0);
}
GfxRenderDataset* VkRenderer::get_dataset()
{
	return dataset;
}
void VkRenderer::update_descriptor_set(VkDescriptorSet desc_set)
{
	std::vector<VkWriteDescriptorSet> descriptorWrites(dataset->binding->size());
	std::vector<VkDescriptorBufferInfo> vkBufferInfo(dataset->binding->size());
	std::vector<VkDescriptorImageInfo> vkImgInfo(dataset->binding->size());
	for (int i = 0; i < dataset->binding->size(); i++) {
		auto& binding = (*dataset->binding)[i];
		VkWriteDescriptorSet& descriptorWriteSet = descriptorWrites[i];
		descriptorWriteSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWriteSet.dstSet = desc_set;
		descriptorWriteSet.dstBinding = binding.binding_location;
		descriptorWriteSet.dstArrayElement = 0;
		descriptorWriteSet.descriptorCount = 1;
		descriptorWriteSet.pNext = nullptr;
		if (binding.binding_type == BindingTypes::UNIFORMBUFFER_VERTEX|| binding.binding_type == BindingTypes::UNIFORMBUFFER_FRAGMENT)
		{
			VkDescriptorBufferInfo& bufferInfo = vkBufferInfo[i];
			bufferInfo.buffer = vk_ubo_buffer_info[binding.name].buffer;
			bufferInfo.offset = 0;
			bufferInfo.range = binding.block_size;
			descriptorWriteSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWriteSet.pBufferInfo = &bufferInfo;
		}
		else if (binding.binding_type == BindingTypes::IMAGE_SAMPLER)
		{
			auto gfx_tex = dataset->get_tex_gfx_func(binding.name);
			auto* vkhandler = (VkTextureWrapper*)(gfx_tex->handler);
			VkDescriptorImageInfo& imageInfo = vkImgInfo[i];
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = vkhandler->image_view;
			imageInfo.sampler = vkhandler->sampler;
			descriptorWriteSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWriteSet.pImageInfo = &imageInfo;
		}
	}
	VkHelper::vk_instance->update_descriptor_sets(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}
void VkRenderer::gen_ubo()
{
	auto itor = dataset->ubo_buffer_data->begin();
	for (itor; itor != dataset->ubo_buffer_data->end(); itor++)
	{
		auto size = itor->second.size() * sizeof(float);
		VkBufferWrapper info = VkHelper::vk_instance->pool->acquire_buffer2(size);
		VkHelper::vk_instance->update_uniform_buffer_array_obj(itor->second, info.mem);
		vk_ubo_buffer_info[itor->first] = info;
	}
	int mvpSize = sizeof(BuiltinPropUbo);
	BuiltinPropUbo ubo = {};
	ubo.model = dataset->rod.od.model;
	ubo.proj = dataset->rod.cd.proj;
	ubo.view = dataset->rod.cd.view;
	ubo.camera_posi = glm::vec4(dataset->rod.cd.cameraPosi,1);
	ubo.lightdir = glm::vec4(dataset->rod.rd.lightDir, 1);
	VkBufferWrapper mvpBuffer = VkHelper::vk_instance->pool->acquire_buffer(ubo);
	vk_ubo_buffer_info[BuiltinPropUbo::GetName()] = mvpBuffer;
};
void VkRenderer::update_ubo(BuiltinPropUbo& ubo)
{
	auto buffer = vk_ubo_buffer_info[BuiltinPropUbo::GetName()];
    VkHelper::vk_instance->update_uniform_buffer_obj(ubo, buffer.mem);
}
VkRenderer::~VkRenderer()
{
	//delete dataset;
}
_VOXEL_END

