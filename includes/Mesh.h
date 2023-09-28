#pragma once
#include <string>
#include <vector>
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"
#include <Defines.h>
#include <Shader.h>
#include "AssetItem.h"
#include "VkHelper.h"
#include<assimp/scene.h>
_VOXEL_BEGIN
class Mesh : AssetItem, public GfxHandlerInterface
{
private:
	static GenMeshGfxFuncPtr gen_mesh_gfx_func;
	static DestroyGfxAssetFuncPtr destroy_mesh_gfx_func;
public:
	static std::shared_ptr<Mesh> quad;
	std::vector<glm::vec3> vertex;
	std::vector<glm::vec4> color;
	std::vector<glm::vec2> uv;
	std::vector<glm::vec3> normal;
	std::vector<glm::vec3> tangent;
	std::vector<glm::uint32> triangle;
	Mesh(const std::string& mesh_path);
	Mesh();
	virtual ~Mesh();
	void* gen_gfx_handler(std::vector<VertexInfo> filters);
	static std::shared_ptr<Mesh> load(const std::string& mesh_path);
	static std::shared_ptr<Mesh> load(const aiMesh* src_mesh);
	static void set_gfx_func(GenMeshGfxFuncPtr m_gen_shader_gfx_func, DestroyGfxAssetFuncPtr destroy_func)
	{
		gen_mesh_gfx_func = m_gen_shader_gfx_func;
		destroy_mesh_gfx_func = destroy_func;
	}
};
_VOXEL_END