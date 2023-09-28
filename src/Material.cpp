#include "material.h"
#include "yaml-cpp/yaml.h"
#include <iostream>
#include <fstream>
#include "resources.h"
#include <VkHelper.h>
#include <algorithm>
_VOXEL_BEGIN
using namespace YAML;
const std::string Material::MAT_SHADER_STR = "m_shader";
const std::string Material::MAT_RENDER_TYPE_STR = "m_renderType";
const std::string Material::MAT_OPAQUE_STR = "Opaque";
const std::string Material::MAT_TEX_PARAM_STR = "m_textureParams";
const std::string Material::MAT_FLOAT_PARAM_STR = "m_floatParams";
const std::string Material::MAT_PROPERTY_STR = "properties";
const std::string Material::DEFAULT_SKYBOX_MAP = "_EnvMap";
const std::string Material::DEFAULT_SKYBOX_PATH = "assets/textures/cubemap.png";
std::shared_ptr<Material> Material::emptyMat = nullptr;
std::map<std::string, std::vector<aiTextureType>> Material::material_tex_finder =
{
	{"_BaseMap", {aiTextureType::aiTextureType_DIFFUSE, aiTextureType::aiTextureType_BASE_COLOR}},
	{"_NormalMap", {aiTextureType::aiTextureType_NORMALS}},
	{"_RoughnessMap", {aiTextureType::aiTextureType_DIFFUSE_ROUGHNESS, aiTextureType::aiTextureType_METALNESS, aiTextureType::aiTextureType_UNKNOWN}},
};
void Material::set_shader(std::shared_ptr<VoxelRendering::Shader> newShader)
{
	shader = newShader;
	auto size = shader->shader_uniform_buffers.size();
	for (size_t i = 0; i < size; i++)
	{
		auto& type = shader->shader_uniform_buffers[i];
		if (type.name == MAT_PROPERTY_STR)
		{
			std::vector<float> ubo;
			for (size_t j = 0; j < type.members.size(); j++)
			{
				auto& m = type.members[j];
				auto& name = m.name;
				if (float_params.find(name) != float_params.end())//find in material
				{
					auto& v = float_params[name];
					for (size_t k = 0; k < v.size(); k++)
					{
						ubo.push_back(v[k]);
					}
				}
				else {//not find in material config add default value
					int count = Util2::get_vec_len(m.type);
					for (size_t k = 0; k < count; k++)
					{
						ubo.push_back(1);
					}
				}
			}
			if (type.block_size != ubo.size() * sizeof(float))
			{
				std::cerr << "shader type block size not correct";
			}
			ubo_buffer_data[type.name] = ubo;
		}
	}
	for (auto& binding : shader->resources_bindings)
	{
		if (binding.binding_type == BindingTypes::IMAGE_SAMPLER)
		{
			if (get_texture(binding.name) == nullptr)
			{
				set_texture(binding.name, Texture::white);
			}
		}
	}
}
std::shared_ptr<GfxTexture> Material::get_texture(std::string& tex)
{
	auto itor = texture_params.begin();
	while (itor != texture_params.end())
	{
		if (itor->first == tex)
		{
			return itor->second;
		}
		itor++;
	}
	return nullptr;
}
GfxHandlerInterface* Material::get_gfx_tex_handler(std::string& tex)
{
	auto itor = texture_params.begin();
	while (itor != texture_params.end())
	{
		if (itor->first == tex)
		{
			return static_cast<GfxHandlerInterface*>(itor->second.get());
		}
		itor++;
	}
	return nullptr;
}
void Material::set_texture(const std::string& texName, std::shared_ptr<GfxTexture> tex)
{
	texture_params[texName] = tex;
	
}

Material::Material(const std::string& materialPath):AssetItem(materialPath)
{
}
Material::Material()
{
}
Material::~Material()
{
	Material::shader = nullptr;
	texture_params.clear();
}
std::shared_ptr<Material>  Material::serialize(char* mat_str,int& length, std::shared_ptr<Material> mat)
{
	std::string s(mat_str, length);
	auto node = Load(s);
	auto shaderPathNode = node[MAT_SHADER_STR];
	auto shaderPath = shaderPathNode.as<std::string>();
	auto Shader = Resources::get_shader(shaderPath);
	auto renderTypeNode = node[MAT_RENDER_TYPE_STR];
	auto renderType = renderTypeNode.as<std::string>();
	if (renderType == MAT_OPAQUE_STR)
	{
		mat->render_type = RenderType::OPAQUE;
	}
	else {
		mat->render_type = RenderType::TRANSPARENT;
	}
	mat->set_shader(Shader);
	auto textureParams = node[MAT_TEX_PARAM_STR];
	for (const auto& pair : textureParams) {
		std::string key = pair.first.as<std::string>();
		std::string value = pair.second.as<std::string>();
		bool cubemap = Shader->isCubemap(key);
		auto tex = Resources::get_texture(value, Texture::is_srgb_texture(key), cubemap);
		mat->set_texture(key, tex);
		//mat->texture_params.insert(std::make_pair(key, tex));
	}
	auto vecParams = node[Material::MAT_FLOAT_PARAM_STR];
	for (const auto& pair : vecParams) {
		std::string key = pair.first.as<std::string>();
		auto& secondNode = pair.second;
		int size = secondNode.size();
		std::vector<float> v;
		for (size_t i = 0; i < size; i++)
		{
			v.push_back(secondNode[i].as<float>());
		}
		mat->float_params[key] = v;
	}
	return mat;
}

std::shared_ptr<Material> Material::load(const std::string& materialPath)
{
	std::ifstream t;
	t.open(materialPath, std::ios_base::binary);      // open input file  
	t.seekg(0, std::ios::end);    // go to the end  
	int length = t.tellg();           // report location (this is the length)  
	t.seekg(0, std::ios::beg);    // go back to the beginning  
	auto buffer = new char[length];    // allocate memory for a buffer of appropriate dimension  
	t.read(buffer, length);       // read the whole file into the buffer  
	t.close();
	auto mat = std::make_shared<Material>(materialPath);
	serialize(buffer, length, mat);
	delete[] buffer;
	return mat;
}

_VOXEL_END

