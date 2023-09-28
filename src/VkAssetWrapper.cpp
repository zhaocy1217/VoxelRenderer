#include <VkAssetWrapper.h>
#include <util.h>
#include<iostream>
_VOXEL_BEGIN
VkMeshWrapper::VkMeshWrapper(std::vector<float>& vertexData, std::vector<unsigned int>& indices)
{
	VkHelper::vk_instance->create_vertex_buffer(vertexData, vertex_buffer.buffer, vertex_buffer.mem);
	if (indices.size() > 0)
	{
		VkHelper::vk_instance->create_index_buffer(indices, index_buffer.buffer, index_buffer.mem);
	}
	triangle_size = indices.size();
}
VkMeshWrapper::~VkMeshWrapper()
{
	auto inst = VkHelper::vk_instance;
	inst->destroy_buffer(vertex_buffer.buffer, vertex_buffer.mem);
	if (triangle_size > 0)
		inst->destroy_buffer(index_buffer.buffer, index_buffer.mem);
}

VkShaderWrapper::VkShaderWrapper(std::vector<char>& v, std::vector<char>& f,
	ShaderVertexInput& vertexInput,
	std::vector<ShaderBindingObject>& resources_bindings)
{
	id = Util2::vec_char_hash(v) << 4 + Util2::vec_char_hash(f);
	VkHelper::vk_instance->create_shader_module(v, vert_shader_module);
	VkHelper::vk_instance->create_shader_module(f, frag_shader_module);
	layout = VkHelper::vk_instance->pool->acquire_layout(resources_bindings);
	binding_desc.binding = 0;
	binding_desc.stride = vertexInput.get_size();
	binding_desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	int attrDescSize = vertexInput.inputs.size();
	attr_desc.resize(attrDescSize);
	int offset = 0;
	for (size_t i = 0; i < attrDescSize; i++)
	{
		attr_desc[i].binding = 0;
		attr_desc[i].location = i;
		if (vertexInput.inputs[i].ele == MeshElement::VERTEX || vertexInput.inputs[i].ele == MeshElement::NORMAL || vertexInput.inputs[i].ele == MeshElement::TANGENT)
		{
			attr_desc[i].format = VK_FORMAT_R32G32B32_SFLOAT;
		}
		else if (vertexInput.inputs[i].ele == MeshElement::COLOR)
		{
			attr_desc[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		}
		else if (vertexInput.inputs[i].ele == MeshElement::UV)
		{
			attr_desc[i].format = VK_FORMAT_R32G32_SFLOAT;
		}
		attr_desc[i].offset = offset;
		offset += vertexInput.inputs[i].size;
	}
	VkHelper::vk_instance->create_pipeline_layout(layout, &pipeline_layout);
}

VkShaderWrapper::~VkShaderWrapper()
{
	VkHelper::vk_instance->destroy_pipeline_layout(pipeline_layout);
	VkHelper::vk_instance->destroy_shader_module(vert_shader_module);
	VkHelper::vk_instance->destroy_shader_module(frag_shader_module);

}

VkTextureWrapper::VkTextureWrapper(unsigned char* data, TextureBaseInfo& info)
{
	size = info.size;
	format = info.srgb ? VkFormat::VK_FORMAT_R8G8B8A8_SRGB : VkFormat::VK_FORMAT_R8G8B8A8_UNORM;
	VkHelper::vk_instance->create_image(image, image_memory, data, size.x, size.y, info.channels, info.mipLevels, info.cubemap, format);
	VkHelper::vk_instance->create_texture_sampler(info.mipLevels, sampler);
	VkHelper::vk_instance->create_image_view(image, image_view, info.cubemap ? VkImageViewType::VK_IMAGE_VIEW_TYPE_CUBE : VkImageViewType::VK_IMAGE_VIEW_TYPE_2D,
		format, VK_IMAGE_ASPECT_COLOR_BIT, info.mipLevels);
}
VkTextureWrapper::VkTextureWrapper(TextureBaseInfo& info)
{
	size = info.size;
	info.msaa = std::max(info.msaa, (uint32_t)1);
	auto inst = VkHelper::vk_instance;
	if (info.depth)
	{
		inst->create_image(size.x, size.y, 1, (VkSampleCountFlagBits)info.msaa, inst->get_depth_format(), VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image, image_memory, 1);
		format = inst->get_depth_format();
		inst->create_image_view(image, image_view, VK_IMAGE_VIEW_TYPE_2D, inst->get_depth_format(), VK_IMAGE_ASPECT_DEPTH_BIT, 1);
	}
	else
	{

		format = info.srgb ? (info.isAttachment ? VkFormat::VK_FORMAT_R8G8B8A8_SRGB : VkFormat::VK_FORMAT_R8G8B8A8_SRGB) : VkFormat::VK_FORMAT_R8G8B8A8_UNORM;
		VkImageUsageFlagBits useage = (VkImageUsageFlagBits)(VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT
			| VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT);
		if (info.isAttachment)
		{
			useage = (VkImageUsageFlagBits)(useage | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
		}
		inst->create_image(size.x, size.y, 1, (VkSampleCountFlagBits)1, format,
			VK_IMAGE_TILING_OPTIMAL, useage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			(image), image_memory, 1);
		inst->create_texture_sampler(info.mipLevels, sampler);
		inst->create_image_view(image, image_view, VkImageViewType::VK_IMAGE_VIEW_TYPE_2D,
			format, VK_IMAGE_ASPECT_COLOR_BIT, info.mipLevels);
	}
}
VkTextureWrapper::VkTextureWrapper()
{
}
VkTextureWrapper::~VkTextureWrapper()
{
	//std::cout << "destory image " + (long)image << std::endl;
	VkHelper::vk_instance->destroy_image(image, image_view, sampler, image_memory);

}
VkFrameBufferWrapper::VkFrameBufferWrapper(TextureBaseInfo& info)
{
	size = info.size;
	srgb = info.srgb;
	color = std::make_shared<GfxTexture>(info);
	info.depth = true;
	depth = std::make_shared<GfxTexture>(info);
	destroy_on_dispose = true;
}
VkFrameBufferWrapper::VkFrameBufferWrapper(std::shared_ptr<GfxTexture> color, std::shared_ptr<GfxTexture> depth)
{
	this->color = color;
	this->depth = depth;
	if (color != nullptr)
	{
		auto t = (VkTextureWrapper*)(color->handler);
		this->size = t->size;
	}
	else
	{
		if (depth != nullptr)
		{
			auto t = (VkTextureWrapper*)(depth->handler);
			this->size = t->size;
		}
	}
	destroy_on_dispose = false;
}
VkFrameBufferWrapper::~VkFrameBufferWrapper()
{
	if (destroy_on_dispose)
	{
		color = nullptr;
		depth = nullptr;
	}
}
//FrameBufferCreateInfo::FrameBufferCreateInfo()
//{
//	
//}

_VOXEL_END

