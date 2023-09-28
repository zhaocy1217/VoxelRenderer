#include "shader.h"
#include "resources.h"
#include <iostream>
#include<string>
#include<json.hpp>
#include <algorithm>
#include <util.h>
_VOXEL_BEGIN
const std::string Shader::BASE_MAP_NAME_STR = "_BaseMap";
std::shared_ptr<Shader> Shader::blit = nullptr;
GenShaderGfxFuncType  Shader::gen_shader_gfx_func = nullptr;
DestroyGfxAssetFuncPtr Shader::destroy_shader_gfx_func = nullptr;
Shader::Shader(const std::string& path) :AssetItem(path) {

}
bool Shader::isCubemap(std::string& name)
{
	int bindingCount = resources_bindings.size();
	bool cubemap = false;
	for (int i = 0; i < bindingCount; i++)
	{
		auto& binding = resources_bindings[i];
		if (binding.name == name)
		{
			if (binding.type == "samplerCube")
			{
				cubemap = true;
				break;
			}
		}
	}
	return cubemap;
}
void AnalysisShaderMeta(std::shared_ptr<Shader> Shader, std::string&& path, ShaderType shaderType)
{
	auto metaBuffer = readFileIntoString(path);
	auto metaJson = json::JSON::Load(*metaBuffer);
	delete metaBuffer;
	if (shaderType == ShaderType::VERTEX_STAGE)
	{
		auto inputs = metaJson.at("inputs");
		auto size = inputs.length();
		auto& vertex_inputs = Shader->vertex_inputs.inputs;
		vertex_inputs.resize(size);
		for (size_t i = 0; i < size; i++)
		{
			auto input = inputs[i];
			auto name = input.at("name").ToString();
			auto t = input.at("type").ToString();
			auto& vertex_input = vertex_inputs[i];
			vertex_input.size = Util2::get_vec_len(t) * sizeof(float);
			vertex_input.location = input.at("location").ToInt();
			if (name == "position")
			{
				vertex_input.ele = MeshElement::VERTEX;
			}
			if (name == "color")
			{
				vertex_input.ele = MeshElement::COLOR;
			}
			if (name == "texcoord")
			{
				vertex_input.ele = MeshElement::UV;
			}
			if (name == "normal")
			{
				vertex_input.ele = MeshElement::NORMAL;
			}
			if (name == "tangent")
			{
				vertex_input.ele = MeshElement::TANGENT;
			}
		}

		std::sort(vertex_inputs.begin(), vertex_inputs.end(), [](VertexInfo& a, VertexInfo& b) {return a.location < b.location; });
	}
	auto ubos = metaJson.at("ubos");
	if (!ubos.IsNull())
	{
		auto size = ubos.length();
		for (size_t i = 0; i < size; i++)
		{
			auto ubo = ubos[i];
			auto name = ubo.at("name").ToString();
			auto bindingLocation = ubo.at("binding").ToInt();
			auto blockSize = ubo.at("block_size").ToInt();
			auto type = ubo.at("type").ToString();
			ShaderBindingObject shader_ubo;
			shader_ubo.binding_location = bindingLocation;
			shader_ubo.name = name;
			shader_ubo.block_size = blockSize;
			shader_ubo.binding_type = shaderType == ShaderType::VERTEX_STAGE ? BindingTypes::UNIFORMBUFFER_VERTEX : BindingTypes::UNIFORMBUFFER_FRAGMENT;
			Shader->resources_bindings.push_back(shader_ubo);
			auto memberType = metaJson.at("types").at(type);
			ShaderUniformBuffer st;
			st.name = memberType.at("name").ToString();
			auto& members = memberType.at("members");
			auto memberSize = members.length();
			for (size_t j = 0; j < memberSize; j++)
			{
				ShaderUniformBufferMember sm;
				sm.name = members[j].at("name").ToString();
				sm.type = members[j].at("type").ToString();
				sm.offset = members[j].at("offset").ToInt();
				st.members.push_back(sm);
			}
			st.block_size = blockSize;
			std::sort(st.members.begin(), st.members.end(), [](ShaderUniformBufferMember& a, ShaderUniformBufferMember& b) {return a.offset < b.offset; });
			Shader->shader_uniform_buffers.push_back(st);
		}
	}
	auto textures = metaJson.at("textures");
	if (!textures.IsNull())
	{
		auto size = textures.length();
		for (size_t i = 0; i < size; i++)
		{
			auto tex = textures[i];
			auto name = tex.at("name").ToString();
			auto bindingLocation = tex.at("binding").ToInt();
			auto type = tex.at("type").ToString();
			ShaderBindingObject shader_tex;
			shader_tex.binding_location = bindingLocation;
			shader_tex.name = name;
			shader_tex.type = type;
			shader_tex.binding_type = BindingTypes::IMAGE_SAMPLER;
			Shader->resources_bindings.push_back(shader_tex);
		}
	}
	std::sort(Shader->resources_bindings.begin(), Shader->resources_bindings.end(), [](ShaderBindingObject& a, ShaderBindingObject& b) {return a.binding_location < b.binding_location; });
}
void* Shader::gen_gfx_handler()
{
	if (handler != nullptr)
	{
		return handler;
	}
	handler = gen_shader_gfx_func(vert_shader_code, frag_shader_code, vertex_inputs, resources_bindings);
	vert_shader_code.clear();
	frag_shader_code.clear();
	return handler;
}
std::shared_ptr<Shader> Shader::load(const std::string& path)
{
	std::shared_ptr<Shader> m_shader = std::make_shared<Shader>(path);
	auto vertShaderPath = path + ".vert.spv";
	auto fragShaderPath = path + ".frag.spv";
	m_shader->vert_shader_code = readTextFile(vertShaderPath);
	m_shader->frag_shader_code = readTextFile(fragShaderPath);
	AnalysisShaderMeta(m_shader, path + ".vert.spv.meta", ShaderType::VERTEX_STAGE);
	AnalysisShaderMeta(m_shader, path + ".frag.spv.meta", ShaderType::FRAGMENT_STAGE);
	m_shader->gen_gfx_handler();
	return m_shader;
}
Shader::~Shader()
{
	destroy_shader_gfx_func(handler);
	//delete path;
}
;
_VOXEL_END