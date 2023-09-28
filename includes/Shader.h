#pragma once
#include <string>
#include "Defines.h"
#include "AssetItem.h"
#include <GfxHandlerInterface.h>
_VOXEL_BEGIN

class Shader:AssetItem, public GfxHandlerInterface
{
private:
	static GenShaderGfxFuncType gen_shader_gfx_func;
	static DestroyGfxAssetFuncPtr destroy_shader_gfx_func;
public:
	static const std::string BASE_MAP_NAME_STR;
	static std::shared_ptr<Shader> blit;
	std::vector<char> vert_shader_code;
	std::vector<char> frag_shader_code;
	bool isCubemap(std::string& name);
	std::vector<ShaderBindingObject> resources_bindings;//binding info of textures and ubos
	std::vector<ShaderUniformBuffer> shader_uniform_buffers;
	ShaderVertexInput vertex_inputs;
	void* gen_gfx_handler();
	Shader(const std::string& path);
	std::shared_ptr<Shader> static load(const std::string& path);
	static void set_gfx_func(GenShaderGfxFuncType m_gen_shader_gfx_func, DestroyGfxAssetFuncPtr del_func)
	{
		gen_shader_gfx_func = m_gen_shader_gfx_func;
		destroy_shader_gfx_func = del_func;
	}
	~Shader();
};

_VOXEL_END